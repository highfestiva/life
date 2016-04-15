
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
 


#include "pch.h"
#include "MasterServerConnection.h"
#include "../../Lepra/Include/Socket.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../LifeMaster/MasterServer.h"
#include "../LifeMaster/MasterServerNetworkParser.h"



namespace Life
{



MasterServerConnection::MasterServerConnection(const str& pMasterAddress):
	mMasterServerAddress(pMasterAddress),
	mState(DISCONNECTED),
	mSocketIoHandler(0),
	mMuxSocket(0),
	mVSocket(0),
	mConnecter(new MemberThread<MasterServerConnection>("MasterServerConnecter")),
	mDisconnectCounter(0),
	mIsConnectError(false),
	mLastFirewallOpenStatus(FIREWALL_ERROR)
{
}

MasterServerConnection::~MasterServerConnection()
{
	GraceClose(2.0, true);
	delete mConnecter;
	mConnecter = 0;
}

const str& MasterServerConnection::GetMasterAddress() const
{
	return mMasterServerAddress;
}


void MasterServerConnection::SetSocketInfo(Cure::SocketIoHandler* pSocketIoHandler, double pConnectTimeout)
{
	mConnectTimeout = pConnectTimeout;
	deb_assert(!mSocketIoHandler || mSocketIoHandler == pSocketIoHandler);	// If this is not valid, you must at least remove *this* as a FilterIo listener from the socket before replacing it.
	mSocketIoHandler = pSocketIoHandler;
	if (!pSocketIoHandler)
	{
		mMuxSocket = 0;
		OnDropSocket(mVSocket);
	}
	else
	{
		Cure::SocketIoHandler::MuxIoSocket* lMuxSocket = pSocketIoHandler->GetMuxIoSocket();
		if (!mMuxSocket || !lMuxSocket || mMuxSocket->GetSysSocket() != lMuxSocket->GetSysSocket() ||
			!mMuxSocket->IsOpen())
		{
			OnDropSocket(mVSocket);
		}
		mMuxSocket = lMuxSocket;
	}
}



void MasterServerConnection::SendLocalInfo(const str& pLocalServerInfo)
{
	deb_assert(!pLocalServerInfo.empty());
	if (mLocalServerInfo != pLocalServerInfo || mUploadTimeout.QueryTimeDiff() >= mServerInfoTimeout)
	{
		mLocalServerInfo = pLocalServerInfo;
		mUploadTimeout.ClearTimeDiff();
		QueryAddState(UPLOAD_INFO);
	}
}

void MasterServerConnection::AppendLocalInfo(const str& pExtraServerInfo)
{
	if (!mUploadedServerInfo.empty())
	{
		mLocalServerInfo += pExtraServerInfo;
		QueryAddState(UPLOAD_INFO);
		TriggerConnectTimer();
	}
}

void MasterServerConnection::RequestServerList(const str& pServerCriterias)
{
	mServerSortCriterias = pServerCriterias;
	QueryAddState(DOWNLOAD_LIST);
	TriggerConnectTimer();
}

void MasterServerConnection::RequestOpenFirewall(const str& pGameServerConnectAddress)
{
	mLastFirewallOpenStatus = FIREWALL_ERROR;
	mLanGameServerAddress.clear();
	mGameServerConnectAddress = pGameServerConnectAddress;
	QueryAddState(OPEN_FIREWALL);
	TriggerConnectTimer();
	Tick();	// We want this to happen real fast.
}

bool MasterServerConnection::UpdateServerList(ServerInfoList& pServerList) const
{
	strutil::strvec lServers = strutil::Split(mServerList, "\n");
	if (lServers.size() >= 1)
	{
		if (lServers.back() != "OK")
		{
			bool lIsUpdated = !pServerList.empty();
			pServerList.clear();
			return lIsUpdated;
		}
	}
	bool lIsUpdated = false;
	ServerInfoList lNewServerList;
	strutil::strvec::const_iterator x = lServers.begin();
	ServerInfoList::const_iterator y = pServerList.begin();
	for (; x != lServers.end(); ++x)
	{
		ServerInfo lInfo;
		MasterServerNetworkParser::ExtractServerInfo(*x, lInfo, 0);
		lNewServerList.push_back(lInfo);
		if (!lIsUpdated && (y != pServerList.end() && lInfo != *y))
		{
			lIsUpdated = true;
		}
		if (y != pServerList.end())
		{
			++y;
		}
		else
		{
			lIsUpdated = true;
		}
	}
	lIsUpdated |= (lNewServerList.size() != pServerList.size());
	if (lIsUpdated)
	{
		pServerList = lNewServerList;
	}
	return lIsUpdated;
}

str MasterServerConnection::GetServerListAsText() const
{
	return mServerList;
}

bool MasterServerConnection::IsConnectError() const
{
	return mIsConnectError;
}

MasterServerConnection::FirewallStatus MasterServerConnection::GetFirewallOpenStatus() const
{
	return mLastFirewallOpenStatus;
}

const str& MasterServerConnection::GetLanServerConnectAddress() const
{
	return mLanGameServerAddress;
}

double MasterServerConnection::WaitUntilDone(double pTimeout, bool pAllowReconnect)
{
	const int lStartDisonnectCounter = mDisconnectCounter;
	HiResTimer lTimer;
	while (lTimer.QueryTimeDiff() < pTimeout && (pAllowReconnect || mVSocket) &&
		lStartDisonnectCounter == mDisconnectCounter && !mStateList.empty())
	{
		Tick();
		Thread::Sleep(0.001);
	}
	if (lTimer.QueryTimeDiff() < pTimeout && mVSocket && lStartDisonnectCounter == mDisconnectCounter)
	{
		Tick();
	}
	return lTimer.QueryTimeDiff();
}

void MasterServerConnection::GraceClose(double pTimeout, bool pWaitUntilDone)
{
	const double lWaitedTime = pWaitUntilDone? WaitUntilDone(pTimeout, false) : 0;
	mConnecter->Join(pTimeout - lWaitedTime);
	Close(false);
}

bool MasterServerConnection::CloseUnlessUploaded()
{
	if (mUploadedServerInfo.empty())
	{
		GraceClose(1.0, false);
	}
	return mUploadedServerInfo.empty();	// TRICKY: some time has gone by, might have been updated.
}

void MasterServerConnection::Tick()
{
	switch (mState)
	{
		case UPLOAD_INFO:
		{
			if (!UploadServerInfo())
			{
				Close(true);
			}
		}
		break;
		case DOWNLOAD_LIST:
		{
			if (!DownloadServerList())
			{
				Close(true);
			}
		}
		break;
		case OPEN_FIREWALL:
		{
			mLastFirewallOpenStatus = OpenFirewall();
		}
		break;
	}
	StepState();
}

bool MasterServerConnection::TickReceive(ServerInfo& pServerInfo)
{
	if (mState != CONNECTED || !mVSocket || !QueryMuxValid())
	{
		return false;
	}
	if (!mVSocket->NeedInputPeek())
	{
		return false;
	}
	str lCommandLine;
	if (!Receive(lCommandLine))
	{
		mLog.Error("Someone snatched data received from master server!");
		deb_assert(false);
		return false;
	}
	if (!MasterServerNetworkParser::ExtractServerInfo(lCommandLine, pServerInfo, &mVSocket->GetTargetAddress()))
	{
		mLog.Error("Got bad formatted command from master server!");
		deb_assert(false);
		return false;
	}
	return true;
}



void MasterServerConnection::QueryAddState(State pState)
{
	if (mState == pState)
	{
		return;
	}
	std::list<State>::iterator x = mStateList.begin();
	for (; x != mStateList.end(); ++x)
	{
		if (*x == pState)
		{
			return;
		}
	}
	mIdleTimer.PopTimeDiff();
	mStateList.push_back(pState);
}

void MasterServerConnection::TriggerConnectTimer()
{
	mIdleTimer.PopTimeDiff();
	mIdleTimer.ReduceTimeDiff(-1.1*mDisconnectedIdleTimeout);
}

void MasterServerConnection::StepState()
{
	switch (mState)
	{
		case CONNECT:
		{
			if (!mConnecter->IsRunning() && QueryMuxValid())
			{
				mState = CONNECTING;
				if (!mConnecter->Start(this, &MasterServerConnection::ConnectEntry))
				{
					mLog.Warning("Could not start connecter.");
					mIsConnectError = true;
				}
			}
			else
			{
				mLog.Warning("Trying to connect while connecter still running!");
				deb_assert(!mVSocket);
				deb_assert(false);
			}
		}
		break;
		case CONNECTED:
		{
			if (!mStateList.empty())
			{
				mState = mStateList.front();
				mStateList.pop_front();
				mIdleTimer.PopTimeDiff();
			}
			else if (mIdleTimer.QueryTimeDiff() >= mConnectedIdleTimeout)
			{
				// This may happen if the player idles too long at the server list GUI.
				Close(false);
			}
		}
		break;
		case UPLOAD_INFO:
		case DOWNLOAD_LIST:
		case OPEN_FIREWALL:
		{
			mState = CONNECTED;
		}
		break;
		case DISCONNECTED:
		{
			if (mIdleTimer.QueryTimeDiff() >= mDisconnectedIdleTimeout && !mStateList.empty())
			{
				// Anything we want done, we must be connected.
				mState = CONNECT;
			}
		}
		break;
	}
}

void MasterServerConnection::ConnectEntry()
{
	if (mState != CONNECTING || mVSocket != 0)
	{
		mLog.Warning("Starting connector thread while already working/connected!");
		deb_assert(false);
		return;
	}

	SocketAddress lTargetAddress;
	if (!lTargetAddress.Resolve(mMasterServerAddress))
	{
		mLog.Warningf("Could not resolve master server address '%s'.", mMasterServerAddress.c_str());
		Close(true);
		return;
	}
	if (mConnecter->GetStopRequest() || !QueryMuxValid())
	{
		mLog.Warningf("Connect to master server '%s' was aborted.", mMasterServerAddress.c_str());
		Close(true);
		return;
	}
	const std::string lConnectionId = SystemManager::GetRandomId();
	Cure::SocketIoHandler::DropFilterCallback lDropCallback(this, &MasterServerConnection::OnDropSocket);
	mSocketIoHandler->AddFilterIoSocket(0, lDropCallback);
	mVSocket = mMuxSocket->Connect(lTargetAddress, lConnectionId, mConnectTimeout);
	if (!mVSocket)
	{
		mLog.Warningf("Could not connect to master server address '%s'.", mMasterServerAddress.c_str());
		Close(true);
		return;
	}
	mSocketIoHandler->AddFilterIoSocket(mVSocket, lDropCallback);
	deb_assert(mState == CONNECTING);
	mState = CONNECTED;
	mIdleTimer.PopTimeDiff();
	mLog.Headlinef("Connected to master server @ %s.", mMasterServerAddress.c_str());
}

bool MasterServerConnection::UploadServerInfo()
{
	if (!SendAndAck(MASTER_SERVER_USI " " + mLocalServerInfo))
	{
		return false;
	}
	mLog.Info("Uploaded server info...");
	mUploadedServerInfo = mLocalServerInfo;
	mIsConnectError = false;
	return true;
}

bool MasterServerConnection::DownloadServerList()
{
	if (!SendAndRecv(MASTER_SERVER_DSL " " + mServerSortCriterias, mServerList))
	{
		return false;
	}
	mIsConnectError = false;
	return true;
}

MasterServerConnection::FirewallStatus MasterServerConnection::OpenFirewall()
{
	strutil::strvec lVector = strutil::Split(mGameServerConnectAddress, ":");
	if (lVector.size() != 2)
	{
		return FIREWALL_ERROR;
	}
	str lReply;
	if (!SendAndRecv(MASTER_SERVER_OF " --address " + lVector[0] + " --port " + lVector[1], lReply))
	{
		return FIREWALL_ERROR;
	}
	ServerInfo lInfo;
	if (lReply == "OK")
	{
		mIsConnectError = false;
		return FIREWALL_OPENED;
	}
	else if (MasterServerNetworkParser::ExtractServerInfo(lReply, lInfo, 0))
	{
		if (lInfo.mCommand == MASTER_SERVER_UL)
		{
			mLanGameServerAddress = lInfo.mInternalIpAddress + strutil::Format(":%i", lInfo.mInternalPort);
			return FIREWALL_USE_LAN;
		}
	}
	return FIREWALL_ERROR;
}

bool MasterServerConnection::SendAndAck(const str& pData)
{
	str lReply;
	if (!SendAndRecv(pData, lReply))
	{
		return false;
	}
	return lReply == "OK";
}

bool MasterServerConnection::SendAndRecv(const str& pData, str& pReply)
{
	if (!Send(pData))
	{
		return false;
	}
	return Receive(pReply);
}

bool MasterServerConnection::Send(const str& pData)
{
	if (!mVSocket || !QueryMuxValid())
	{
		mLog.Warning("Trying to send to master server even though unconnected.");
		Close(false);
		return false;
	}
	if (pData.size() > 300)
	{
		mLog.Warning("Trying to send too big chunk to master server.");
		return false;
	}
	uint8 lRawData[1024];
	unsigned lSendByteCount = MasterServerNetworkParser::StrToRaw(lRawData, pData);
	if ((unsigned)mVSocket->DirectSend(lRawData, lSendByteCount) != lSendByteCount)
	{
		mLog.Warning("Transmission to master server failed.");
		return false;
	}
	return true;
}

bool MasterServerConnection::Receive(str& pData)
{
	if (!mVSocket || !QueryMuxValid())
	{
		mLog.Warning("Trying to receive master server data even though unconnected.");
		return false;
	}
	mVSocket->WaitAvailable(4.0);
	uint8 lRawData[1024];
	int lRawSize = mVSocket->Receive(lRawData, sizeof(lRawData));
	if (lRawSize <= 0)
	{
		mLog.Warning("Tried to receive data from master server, but it didn't send us any!");
		return false;
	}
	str lData;
	if (!MasterServerNetworkParser::RawToStr(lData, lRawData, lRawSize))
	{
		mLog.Error("Got garbled data from master server - something is seriously wrong!");
		return false;
	}
	pData = lData;
	return true;
}

void MasterServerConnection::Close(bool pError)
{
	if (pError)
	{
		mIsConnectError = true;
	}
	++mDisconnectCounter;
	if (mMuxSocket && mVSocket)
	{
		Send(MASTER_SERVER_DC);
		if (mSocketIoHandler)
		{
			mSocketIoHandler->KillIoSocket(mVSocket);
		}
	}
	if (mSocketIoHandler)
	{
		mSocketIoHandler->RemoveAllFilterIoSockets();
	}
	deb_assert(mVSocket == 0);
	mState = DISCONNECTED;
	mStateList.clear();	// A) we lost connection, no use trying for a while, or B) nothing more to do (=already empty).
}

bool MasterServerConnection::QueryMuxValid()
{
	bool lInvalid = (!mMuxSocket || !mMuxSocket->IsOpen());
	if (lInvalid)
	{
		OnDropSocket(mVSocket);
	}
	return !lInvalid;
}



void MasterServerConnection::OnDropSocket(Cure::SocketIoHandler::VIoSocket* pSocket)
{
	//deb_assert(pSocket == mVSocket);
	deb_assert(mState == CONNECT || mState == CONNECTED || mState == UPLOAD_INFO || mState == DOWNLOAD_LIST ||
		mState == OPEN_FIREWALL || mState == DISCONNECTED || mState == CONNECTING);	// Just to make sure we're not deleted and gets callbacked anyhoo.
	if (pSocket == mVSocket || pSocket == 0)	// NULL means MUX socket is closing down!
	{
		if (mState == CONNECTING)
		{
			mConnecter->Join(0.5);
		}
		mVSocket = 0;
		mState = DISCONNECTED;
	}
}



const double MasterServerConnection::mConnectedIdleTimeout = MASTER_SERVER_TIMEOUT;
const double MasterServerConnection::mDisconnectedIdleTimeout = 10.0;
const double MasterServerConnection::mServerInfoTimeout = MASTER_SERVER_TIMEOUT/3-1;

loginstance(NETWORK_SERVER, MasterServerConnection);



}
