
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games
 


#include "MasterServer.h"
#include "../../Lepra/Include/Socket.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../ServerInfo.h"
#include "MasterServerNetworkParser.h"



namespace Life
{



MasterServer::MasterServer():
	mMuxSocket(0)
{
}

MasterServer::~MasterServer()
{
	SystemManager::AddQuitRequest(+1);
	delete (mMuxSocket);
	mMuxSocket = 0;
}

bool MasterServer::Run()
{
	SocketAddress lAddress;
	str lAcceptAddress = _T("0.0.0.0:") _T(MASTER_SERVER_PORT);
	if (!lAddress.Resolve(lAcceptAddress))
	{
		mLog.Warningf(_T("Could not resolve address '%s'."), lAcceptAddress.c_str());
		lAcceptAddress = _T(":") _T(MASTER_SERVER_PORT);
		if (!lAddress.Resolve(lAcceptAddress))
		{
			mLog.Errorf(_T("Could not resolve address '%s'!"), lAcceptAddress.c_str());
			return false;
		}
	}
	mMuxSocket = new UdpMuxSocket(_T("MasterServer"), lAddress, true);
	if (!mMuxSocket->IsOpen())
	{
		mLog.Errorf(_T("Address '%s' seems busy. Terminating."), lAcceptAddress.c_str());
		return false;
	}
	mLog.Headline(_T("Up and running, awaiting connections."));
	SystemManager::SetQuitRequestCallback(SystemManager::QuitRequestCallback(this, &MasterServer::OnQuitRequest));
	while (SystemManager::GetQuitRequest() == 0 && mMuxSocket)
	{
		UdpVSocket* lSocket;
		if (mMuxSocket->GetConnectionCount() == 0)
		{
			lSocket = mMuxSocket->Accept();
		}
		else
		{
			lSocket = mMuxSocket->PollAccept();
		}
		if (lSocket)
		{
			mSocketTable.insert(lSocket);
		}
		KillDeadSockets();
		KillDeadServers();
		while ((lSocket = mMuxSocket->PopReceiverSocket()) != 0)
		{
			uint8 lReceiveBuffer[1024];
			int lReceivedBytes = lSocket->Receive(lReceiveBuffer, sizeof(lReceiveBuffer));
			if (lReceivedBytes > 0)
			{
				mSocketTimeoutTable.erase(lSocket);
				HandleReceive(lSocket, lReceiveBuffer, lReceivedBytes);
			}
		}
		Thread::Sleep(0.05);
	}
	mLog.Headline(_T("Terminating master server..."));
	return true;
}



void MasterServer::KillDeadServers()
{
	if (mServerIdleTimer.QueryTimeDiff() < 20.0)
	{
		return;
	}
	mServerIdleTimer.ClearTimeDiff();

	GameServerTable::iterator x = mGameServerTable.begin();
	if (x != mGameServerTable.end())
	{
		GameServerInfo& lInfo = x->second;
		if (lInfo.mIdleTime.QueryTimeDiff() > MASTER_SERVER_TIMEOUT)
		{
			mGameServerTable.erase(x++);
		}
		else
		{
			++x;
		}
	}
}

void MasterServer::KillDeadSockets()
{
	if (mKeepaliveTimer.QueryTimeDiff() < 20.0)
	{
		return;
	}
	mKeepaliveTimer.ClearTimeDiff();

	// Kill all old and dead connections.
	while (!mSocketTimeoutTable.empty())
	{
		UdpVSocket* lSocket = *mSocketTimeoutTable.begin();
		DropSocket(lSocket);
	}
	mSocketTimeoutTable.insert(mSocketTable.begin(), mSocketTable.end());
}

void MasterServer::OnQuitRequest(int)
{
	if (mMuxSocket)
	{
		mMuxSocket->Close();
		mMuxSocket->ReleaseSocketThreads();
	}
}

void MasterServer::HandleReceive(UdpVSocket* pRemote, const uint8* pCommand, unsigned pCommandLength)
{
	wstr lWideData;
	if (!MasterServerNetworkParser::RawToStr(lWideData, pCommand, pCommandLength))
	{
		mLog.Error(_T("Got garbled data from game server!"));
		DropSocket(pRemote);
		return;
	}
	const str lCommandLine = strutil::Encode(lWideData);
	if (!HandleCommandLine(pRemote, lCommandLine))
	{
		mLog.Error(_T("Got invalid command from game server!"));
	}
}

bool MasterServer::HandleCommandLine(UdpVSocket* pRemote, const str& pCommandLine)
{
	ServerInfo lServerInfo;
	if (!MasterServerNetworkParser::ExtractServerInfo(pCommandLine, lServerInfo, &pRemote->GetTargetAddress()))
	{
		return false;
	}
	if (lServerInfo.mCommand == _T(MASTER_SERVER_USI))
	{
		if (lServerInfo.mRemotePort < 0 || lServerInfo.mPlayerCount < 0 || lServerInfo.mId.empty())
		{
			mLog.Errorf(_T("Got bad parameters to command (%s) from game server!"), lServerInfo.mCommand.c_str());
			return false;
		}
		return RegisterGameServer(!lServerInfo.mRemove, pRemote, lServerInfo.mName,
			lServerInfo.mPlayerCount, lServerInfo.mId);
	}
	else if (lServerInfo.mCommand == _T(MASTER_SERVER_DSL))
	{
		return SendServerList(pRemote);
	}
	else if (lServerInfo.mCommand == _T(MASTER_SERVER_OF))
	{
		if (OpenFirewall(lServerInfo))
		{
			mLog.Infof(_T("Asking game server to open firewall!"), lServerInfo.mCommand.c_str());
			Send(pRemote, _T("OK"));
			return true;
		}
		mLog.Errorf(_T("Got request for connecting to offline game server!"), lServerInfo.mCommand.c_str());
		Send(pRemote, _T("Server offline."));
	}
	else if (lServerInfo.mCommand == _T(MASTER_SERVER_DC))
	{
		DropSocket(pRemote);
		return true;
	}
	else
	{
		mLog.Errorf(_T("Got bad command (%s) from game server!"), lServerInfo.mCommand.c_str());
	}
	return false;
}

bool MasterServer::RegisterGameServer(bool pActivate, UdpVSocket* pRemote, const str& pName,
	int pPlayerCount, const str& pId)
{
	bool lOk = false;
	const str lAddress = pRemote->GetTargetAddress().GetAsString();

	{
		ScopeLock lLock(&mLock);
		GameServerTable::iterator x = mGameServerTable.find(lAddress);
		if (x != mGameServerTable.end())
		{
			GameServerInfo& lInfo = x->second;
			if (pActivate)
			{
				if (lInfo.mId == pId)
				{
					lInfo.mName = pName;
					lInfo.mPlayerCount = pPlayerCount;
					lInfo.mIdleTime.PopTimeDiff();
					lOk = true;
				}
				else
				{
					mLog.Errorf(_T("Got bad ID (%s) from game server %s!"), pId.c_str(), lInfo.mName.c_str());
				}
			}
			else
			{
				if (lInfo.mName == pName && lInfo.mId == pId)
				{
					mGameServerTable.erase(x);
					lOk = true;
				}
				else
				{
					mLog.Errorf(_T("Could not drop game server %s, due to mismatching name and/or id."), lInfo.mName.c_str());
				}
			}
		}
		else if (pActivate)
		{
			GameServerInfo lInfo(pName, pPlayerCount, pId);
			mGameServerTable.insert(GameServerTable::value_type(lAddress, lInfo));
			lOk = true;
		}
		else
		{
			mLog.Errorf(_T("Could not drop game server %s, not found."), pName.c_str());
		}
		if (lOk)
		{
			mLog.RawPrint(Log::LEVEL_DEBUG, _T("----------------------------------------\nServer list:\n"));
			GameServerTable::iterator x = mGameServerTable.begin();
			for (; x != mGameServerTable.end(); ++x)
			{
				mLog.RawPrint(Log::LEVEL_DEBUG, x->second.mName + _T(" @ ") + x->first + _T("\n"));
			}
		}
	}

	if (lOk)
	{
		Send(pRemote, _T("OK"));
		return true;
	}
	return false;
}

bool MasterServer::SendServerList(UdpVSocket* pRemote)
{
	str lServerList;
	lServerList.reserve(1024);
	{
		ScopeLock lLock(&mLock);
		GameServerTable::iterator x = mGameServerTable.begin();
		for (; x != mGameServerTable.end(); ++x)
		{
			const strutil::strvec lFullAddress = strutil::Split(x->first, _T(":"));
			lServerList += _T("--name \"") + x->second.mName + _T("\" --address ") + lFullAddress[0] +
				_T("\" --port ") + lFullAddress[1] +
				_T(" --player-count ") + strutil::IntToString(x->second.mPlayerCount, 10) + _T("\n");
		}
	}
	lServerList += _T("OK");
	return Send(pRemote, lServerList);
}

bool MasterServer::OpenFirewall(const ServerInfo& pServerInfo)
{
	const str lAddress = pServerInfo.mGivenAddress + strutil::Format(_T(":%u"), pServerInfo.mGivenPort);
	SocketAddress lSocketAddress;
	if (lSocketAddress.Resolve(lAddress))
	{
		ScopeLock lLock(&mLock);
		GameServerTable::iterator x = mGameServerTable.find(lAddress);
		if (x != mGameServerTable.end())
		{
			UdpVSocket* lGameServerSocket = mMuxSocket->GetVSocket(lSocketAddress);
			if (lGameServerSocket)
			{
				return Send(lGameServerSocket, _T(MASTER_SERVER_OF) _T(" --address ") + pServerInfo.mRemoteAddress + _T(" --port ") + strutil::IntToString(pServerInfo.mRemotePort, 10));
			}
		}
	}
	return false;
}
bool MasterServer::Send(UdpVSocket* pRemote, const str& pData)
{
	uint8 lRawData[1024];
	if (pData.size() > sizeof(lRawData)/3)
	{
		mLog.Warning(_T("Trying to send too big chunk to game server."));
		return false;
	}
	unsigned lSendByteCount = MasterServerNetworkParser::StrToRaw(lRawData, wstrutil::Encode(pData));
	if ((unsigned)pRemote->DirectSend(lRawData, lSendByteCount) != lSendByteCount)
	{
		mLog.Warning(_T("Transmission to game server failed."));
		return false;
	}
	return true;
}

void MasterServer::DropSocket(UdpVSocket* pRemote)
{
	assert(mSocketTable.find(pRemote) != mSocketTable.end());
	mSocketTimeoutTable.erase(pRemote);
	mSocketTable.erase(pRemote);
	mMuxSocket->CloseSocket(pRemote);
}



MasterServer::GameServerInfo::GameServerInfo(const str& pName, int pPlayerCount, const str& pId):
	mName(pName),
	mPlayerCount(pPlayerCount),
	mId(pId)
{
}



LOG_CLASS_DEFINE(NETWORK_SERVER, MasterServer);



}
