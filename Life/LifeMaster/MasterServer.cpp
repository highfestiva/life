
// Author: Jonas Byström
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



bool MasterServer::Initialize()
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
	return true;
}

bool MasterServer::Tick()
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
	return true;
}

void MasterServer::PollRoundTrip()
{
}

float MasterServer::GetTickTimeReduction() const
{
	return 0;
}

float MasterServer::GetPowerSaveAmount() const
{
	return 0.05f;
}



void MasterServer::KillDeadServers()
{
	if (mServerIdleTimer.QueryTimeDiff() < MASTER_SERVER_TIMEOUT)
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
	if (mKeepaliveTimer.QueryTimeDiff() < MASTER_SERVER_TIMEOUT)
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
	mLog.Debugf(_T("Got command: '%s'."), pCommandLine.c_str());
	if (lServerInfo.mCommand == _T(MASTER_SERVER_USI))
	{
		if (lServerInfo.mRemotePort < 0 || lServerInfo.mPlayerCount < 0 || lServerInfo.mId.empty())
		{
			mLog.Errorf(_T("Got bad parameters to command (%s) from game server!"), lServerInfo.mCommand.c_str());
			return false;
		}
		return RegisterGameServer(lServerInfo, pRemote);
	}
	else if (lServerInfo.mCommand == _T(MASTER_SERVER_DSL))
	{
		return SendServerList(pRemote);
	}
	else if (lServerInfo.mCommand == _T(MASTER_SERVER_OF))
	{
		return OpenFirewall(pRemote, lServerInfo);
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

bool MasterServer::RegisterGameServer(const ServerInfo& pServerInfo, UdpVSocket* pRemote)
{
	const bool lActivate = !pServerInfo.mRemove;

	bool lOk = false;
	const str lAddress = pRemote->GetTargetAddress().GetAsString();

	{
		ScopeLock lLock(&mLock);
		GameServerTable::iterator x = mGameServerTable.find(lAddress);
		if (x != mGameServerTable.end())
		{
			GameServerInfo& lInfo = x->second;
			if (lActivate)
			{
				if (lInfo.mId == pServerInfo.mId)
				{
					lInfo = pServerInfo;
					lInfo.mIdleTime.PopTimeDiff();
					lOk = true;
				}
				else
				{
					mLog.Errorf(_T("Got bad ID (%s) from game server %s!"), pServerInfo.mId.c_str(), lInfo.mName.c_str());
				}
			}
			else
			{
				if (lInfo.mName == pServerInfo.mName && lInfo.mId == pServerInfo.mId)
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
		else if (lActivate)
		{
			GameServerInfo lInfo(pServerInfo);
			mGameServerTable.insert(GameServerTable::value_type(lAddress, lInfo));
			lOk = true;
		}
		else
		{
			mLog.Errorf(_T("Could not drop game server %s, not found."), pServerInfo.mName.c_str());
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
				_T(" --port ") + lFullAddress[1] +
				_T(" --player-count ") + strutil::IntToString(x->second.mPlayerCount, 10) + _T("\n");
		}
	}
	lServerList += _T("OK");
	return Send(pRemote, lServerList);
}

bool MasterServer::OpenFirewall(UdpVSocket* pRemote, const ServerInfo& pServerInfo)
{
	const str lAddress = pServerInfo.mGivenIpAddress + strutil::Format(_T(":%u"), pServerInfo.mGivenPort);
	SocketAddress lSocketAddress;
	if (lSocketAddress.Resolve(lAddress))
	{
		ScopeLock lLock(&mLock);
		GameServerTable::iterator x = mGameServerTable.find(lAddress);
		if (x != mGameServerTable.end())
		{
			if (pServerInfo.mGivenIpAddress == pServerInfo.mRemoteIpAddress)
			{
				// The client has the same IP as the server. This means it should try LAN connect instead of
				// going through the firewall. At least the risk of getting caught in a NAT without hairpin
				// is significantly decreased.
				const GameServerInfo& lServerInfo = x->second;
				mLog.AInfo("Asking game client to use LAN instead, since they share IP.");
				mLog.Debugf(_T("Internal address is '%s'."), lServerInfo.mInternalIpAddress.c_str());
				return Send(pRemote, _T(MASTER_SERVER_UL) _T(" --internal-address ") + lServerInfo.mInternalIpAddress +
					_T(" --internal-port ") + strutil::IntToString(lServerInfo.mInternalPort, 10));
			}

			UdpVSocket* lGameServerSocket = mMuxSocket->GetVSocket(lSocketAddress);
			if (lGameServerSocket)
			{
				if (Send(lGameServerSocket, _T(MASTER_SERVER_OF) _T(" --address ") + pServerInfo.mRemoteIpAddress + _T(" --port ") + strutil::IntToString(pServerInfo.mRemotePort, 10)))
				{
					mLog.AInfo("Asked game server to open firewall!");
					return Send(pRemote, _T("OK"));
				}
			}
		}
	}
	mLog.AError("Got request for connecting to offline game server!");
	Send(pRemote, _T("Server offline."));
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



MasterServer::GameServerInfo::GameServerInfo(const ServerInfo& pServerInfo):
	ServerInfo(pServerInfo)
/*	mCommand(pServerInfo.mCommand),
	mName(pServerInfo.mName),
	mId(pServerInfo.mId),
	mGivenIpAddress(pServerInfo.mGivenIpAddress),
	mInternalIpAddress(pServerInfo.mInternalIpAddress),
	mRemoteIpAddress(pServerInfo.mRemoteIpAddress),
	mGivenPort(pServerInfo.mGivenPort),
	mInternalPort(pServerInfo.mInternalPort),
	mRemotePort(pServerInfo.mRemotePort),
	mPlayerCount(pServerInfo.mPlayerCount),
	mRemove(pServerInfo.mRemove),
	mPing(pServerInfo.mPing)*/
{
	mIdleTime.PopTimeDiff();
}



LOG_CLASS_DEFINE(NETWORK_SERVER, MasterServer);



}
