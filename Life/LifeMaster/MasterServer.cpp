
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
 


#include "pch.h"
#include "MasterServer.h"
#include "../../Lepra/Include/Socket.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../ServerInfo.h"
#include "MasterServerNetworkParser.h"



namespace Life
{



MasterServer::MasterServer(const str& pPort):
	mMuxSocket(0),
	mPort(pPort)
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
	str lAcceptAddress = "0.0.0.0:" + mPort;
	if (!lAddress.Resolve(lAcceptAddress))
	{
		mLog.Warningf("Could not resolve address '%s'.", lAcceptAddress.c_str());
		lAcceptAddress = ":" + mPort;
		if (!lAddress.Resolve(lAcceptAddress))
		{
			mLog.Errorf("Could not resolve address '%s'!", lAcceptAddress.c_str());
			return false;
		}
	}
	mMuxSocket = new UdpMuxSocket("MasterServer", lAddress, true);
	if (!mMuxSocket->IsOpen())
	{
		mLog.Errorf("Address '%s' seems busy. Terminating.", lAcceptAddress.c_str());
		return false;
	}
	mLog.Headline("Up and running, awaiting connections.");
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
	Tick();
}

float MasterServer::GetTickTimeReduction() const
{
	return 0;
}

float MasterServer::GetPowerSaveAmount() const
{
	if (!mGameServerTable.empty())
	{
		return 0;
	}
	if (!mSocketTimeoutTable.empty())
	{
		return 0.1f;
	}
	return 1;
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
	str lWideData;
	if (!MasterServerNetworkParser::RawToStr(lWideData, pCommand, pCommandLength))
	{
		mLog.Error("Got garbled data from game server!");
		DropSocket(pRemote);
		return;
	}
	const str lCommandLine = lWideData;
	if (!HandleCommandLine(pRemote, lCommandLine))
	{
		mLog.Error("Got invalid command from game server!");
	}
}

bool MasterServer::HandleCommandLine(UdpVSocket* pRemote, const str& pCommandLine)
{
	ServerInfo lServerInfo;
	if (!MasterServerNetworkParser::ExtractServerInfo(pCommandLine, lServerInfo, &pRemote->GetTargetAddress()))
	{
		return false;
	}
	log_volatile(mLog.Debugf("Got command: '%s'.", pCommandLine.c_str()));
	if (lServerInfo.mCommand == MASTER_SERVER_USI)
	{
		if (lServerInfo.mRemotePort < 0 || lServerInfo.mPlayerCount < 0 || lServerInfo.mId.empty())
		{
			mLog.Errorf("Got bad parameters to command (%s from game server)!", lServerInfo.mCommand.c_str());
			return false;
		}
		return RegisterGameServer(lServerInfo, pRemote);
	}
	else if (lServerInfo.mCommand == MASTER_SERVER_DSL)
	{
		return SendServerList(pRemote);
	}
	else if (lServerInfo.mCommand == MASTER_SERVER_OF)
	{
		return OpenFirewall(pRemote, lServerInfo);
	}
	else if (lServerInfo.mCommand == MASTER_SERVER_DC)
	{
		DropSocket(pRemote);
		return true;
	}
	else
	{
		mLog.Errorf("Got bad command (%s from game server)!", lServerInfo.mCommand.c_str());
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
					mLog.Errorf("Got bad ID (%s from game server %s)!", pServerInfo.mId.c_str(), lInfo.mName.c_str());
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
					mLog.Errorf("Could not drop game server %s, due to mismatching name and/or id.", lInfo.mName.c_str());
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
			mLog.Errorf("Could not drop game server %s, not found.", pServerInfo.mName.c_str());
		}
		if (lOk)
		{
			mLog.RawPrint(LEVEL_DEBUG, "----------------------------------------\nServer list:\n");
			GameServerTable::iterator x = mGameServerTable.begin();
			for (; x != mGameServerTable.end(); ++x)
			{
				mLog.RawPrint(LEVEL_DEBUG, x->second.mName + " @ " + x->first + "\n");
			}
		}
	}

	if (lOk)
	{
		Send(pRemote, "OK");
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
			const strutil::strvec lFullAddress = strutil::Split(x->first, ":");
			lServerList += "--name \"" + x->second.mName + "\" --address " + lFullAddress[0] +
				" --port " + lFullAddress[1] +
				" --player-count " + strutil::IntToString(x->second.mPlayerCount, 10) + "\n";
		}
	}
	lServerList += "OK";
	return Send(pRemote, lServerList);
}

bool MasterServer::OpenFirewall(UdpVSocket* pRemote, const ServerInfo& pServerInfo)
{
	const str lAddress = pServerInfo.mGivenIpAddress + strutil::Format(":%u", pServerInfo.mGivenPort);
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
				mLog.Info("Asking game client to use LAN instead, since they share IP.");
				log_volatile(mLog.Debugf("Internal address is '%s'.", lServerInfo.mInternalIpAddress.c_str()));
				return Send(pRemote, MASTER_SERVER_UL " --internal-address " + lServerInfo.mInternalIpAddress +
					" --internal-port " + strutil::IntToString(lServerInfo.mInternalPort, 10));
			}

			UdpVSocket* lGameServerSocket = mMuxSocket->GetVSocket(lSocketAddress);
			if (lGameServerSocket)
			{
				if (Send(lGameServerSocket, MASTER_SERVER_OF " --address " + pServerInfo.mRemoteIpAddress + " --port " + strutil::IntToString(pServerInfo.mRemotePort, 10)))
				{
					mLog.Info("Asked game server to open firewall!");
					return Send(pRemote, "OK");
				}
			}
		}
	}
	mLog.Error("Got request for connecting to offline game server!");
	Send(pRemote, "Server offline.");
	return false;
}
bool MasterServer::Send(UdpVSocket* pRemote, const str& pData)
{
	uint8 lRawData[1024];
	if (pData.size() > sizeof(lRawData)/3)
	{
		mLog.Warning("Trying to send too big chunk to game server.");
		return false;
	}
	unsigned lSendByteCount = MasterServerNetworkParser::StrToRaw(lRawData, pData);
	if ((unsigned)pRemote->DirectSend(lRawData, lSendByteCount) != lSendByteCount)
	{
		mLog.Warning("Transmission to game server failed.");
		return false;
	}
	return true;
}

void MasterServer::DropSocket(UdpVSocket* pRemote)
{
	deb_assert(mSocketTable.find(pRemote) != mSocketTable.end());
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



loginstance(NETWORK_SERVER, MasterServer);



}
