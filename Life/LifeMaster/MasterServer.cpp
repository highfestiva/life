
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games
 


#include "MasterServer.h"
#include "../../Lepra/Include/Socket.h"
#include "../../Lepra/Include/SystemManager.h"
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
	if (!lAddress.Resolve(_T("0.0.0.0:") _T(MASTER_SERVER_PORT)))
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
		uint8 lReceiveBuffer[1024];
		SocketAddress lRemoteAddress;
		int lReceivedBytes = mMuxSocket->Accept();
		ööö - ReceiveFrom(lReceiveBuffer, sizeof(lReceiveBuffer), lRemoteAddress);
		if (lReceivedBytes > 0)
		{
			HandleReceive(lRemoteAddress, lReceiveBuffer, lReceivedBytes);
		}
	}
	mLog.Headline(_T("Terminating master server..."));
	return true;
}



void MasterServer::OnQuitRequest(int)
{
	UdpSocket* lSocket = mMuxSocket;
	mMuxSocket = 0;
	delete lSocket;
}

void MasterServer::HandleReceive(const SocketAddress& pRemoteAddress, const uint8* pCommand, unsigned pCommandLength)
{
	wstr lWideData;
	if (!MasterServerNetworkParser::RawToStr(lWideData, pCommand, pCommandLength))
	{
		mLog.Error(_T("Got garbled data from game server!"));
		return;
	}
	const str lCommandLine = strutil::Encode(lWideData);
	if (!HandleCommandLine(pRemoteAddress, lCommandLine))
	{
		mLog.Error(_T("Got invalid command from game server!"));
	}
}

bool MasterServer::HandleCommandLine(const SocketAddress& pRemoteAddress, const str& pCommandLine)
{
	ServerInfo lServerInfo;
	if (!MasterServerNetworkParser::ExtractServerInfo(pCommandLine, lServerInfo))
	{
		return false;
	}
	if (lServerInfo.mCommand == _T(MASTER_SERVER_USI))
	{
		if (lServerInfo.mPort < 0 || lServerInfo.mPlayerCount < 0 || lServerInfo.mId.empty())
		{
			mLog.Errorf(_T("Got bad parameters to command (%s) from game server!"), lServerInfo.mCommand.c_str());
			return false;
		}
		return RegisterGameServer(!lServerInfo.mRemove, pRemoteAddress, lServerInfo.mName, lServerInfo.mPort,
			lServerInfo.mPlayerCount, lServerInfo.mId);
	}
	else if (lServerInfo.mCommand == _T(MASTER_SERVER_DSL))
	{
		return SendServerList(pRemoteAddress);
	}
	else
	{
		mLog.Errorf(_T("Got bad command (%s) from game server!"), lServerInfo.mCommand.c_str());
	}
	return false;
}

bool MasterServer::RegisterGameServer(bool pActivate, const SocketAddress& pRemoteAddress, const str& pName, int pPort,
	int pPlayerCount, const str& pId)
{
	bool lOk = false;
	const str lAddress = pRemoteAddress.GetAsString();

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
					lInfo.mPort = pPort;
					lInfo.mPlayerCount = pPlayerCount;
					lInfo.mIdleTime.PopTimeDiffF();
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
			GameServerInfo lInfo(pName, pPort, pPlayerCount, pId);
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
		Send(pRemoteAddress, _T("OK"));
		return true;
	}
	return false;
}

bool MasterServer::SendServerList(const SocketAddress& pRemoteAddress)
{
	str lServerList;
	{
		ScopeLock lLock(&mLock);
		GameServerTable::iterator x = mGameServerTable.begin();
		for (; x != mGameServerTable.end(); ++x)
		{
			lServerList += _T("--name \"") + x->second.mName + _T("\" --address ") + x->first +
				_T(" --player-count ") + strutil::IntToString(x->second.mPlayerCount, 10) + _T("\n");
		}
	}
	lServerList += _T("OK");
	return Send(pRemoteAddress, lServerList);
}

bool MasterServer::Send(const SocketAddress& pRemoteAddress, const str& pData)
{
	uint8 lRawData[1024];
	if (pData.size() > sizeof(lRawData)/3)
	{
		mLog.Warning(_T("Trying to send too big chunk to game server."));
		return false;
	}
	unsigned lSendByteCount = MasterServerNetworkParser::StrToRaw(lRawData, wstrutil::Encode(pData));
	if ((unsigned)mMuxSocket->SendTo(lRawData, lSendByteCount, pRemoteAddress) != lSendByteCount)
	{
		mLog.Warning(_T("Transmission to game server failed."));
		return false;
	}
	return true;
}



MasterServer::GameServerInfo::GameServerInfo(const str& pName, int pPort, int pPlayerCount, const str& pId):
	mName(pName),
	mPort(pPort),
	mPlayerCount(pPlayerCount),
	mId(pId)
{
}



LOG_CLASS_DEFINE(NETWORK_SERVER, MasterServer);



}
