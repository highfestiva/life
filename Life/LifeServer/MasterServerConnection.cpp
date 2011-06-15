
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games
 


#include "MasterServerConnection.h"
#include "../../Lepra/Include/Socket.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../LifeMaster/MasterServer.h"
#include "../LifeMaster/MasterServerNetworkParser.h"



namespace Life
{



MasterServerConnection::MasterServerConnection():
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



void MasterServerConnection::SetSocketInfo(Cure::SocketIoHandler* pSocketIoHandler, double pConnectTimeout)
{
	mConnectTimeout = pConnectTimeout;
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
	assert(!pLocalServerInfo.empty());
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
	strutil::strvec lServers = strutil::Split(mServerList, _T("\n"));
	if (lServers.size() >= 1)
	{
		if (lServers.back() != _T("OK"))
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
		mLog.Error(_T("Someone snatched data received from master server!"));
		assert(false);
		return false;
	}
	if (!MasterServerNetworkParser::ExtractServerInfo(lCommandLine, pServerInfo, &mVSocket->GetTargetAddress()))
	{
		mLog.Error(_T("Got bad formatted command from master server!"));
		assert(false);
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
					mLog.Warning(_T("Could not start connecter."));
					mIsConnectError = true;
				}
			}
			else
			{
				mLog.Warning(_T("Trying to connect while connecter still running!"));
				assert(!mVSocket);
				assert(false);
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
		mLog.Warning(_T("Starting connector thread while already working/connected!"));
		assert(false);
		return;
	}

	const str lMasterServerAddress(_T(MASTER_SERVER_NAME) _T(":") _T(MASTER_SERVER_PORT));
	SocketAddress lTargetAddress;
	if (!lTargetAddress.Resolve(lMasterServerAddress))
	{
		mLog.Warningf(_T("Could not resolve master server address '%s'."), lMasterServerAddress.c_str());
		Close(true);
		return;
	}
	if (mConnecter->GetStopRequest() || !QueryMuxValid())
	{
		mLog.Warningf(_T("Connect to master server '%s' was aborted."), lMasterServerAddress.c_str());
		Close(true);
		return;
	}
	const std::string lConnectionId = SystemManager::GetRandomId();
	Cure::SocketIoHandler::DropFilterCallback lDropCallback(this, &MasterServerConnection::OnDropSocket);
	mSocketIoHandler->AddFilterIoSocket(0, lDropCallback);
	mVSocket = mMuxSocket->Connect(lTargetAddress, lConnectionId, mConnectTimeout);
	if (!mVSocket)
	{
		mLog.Warningf(_T("Could not connect to master server address '%s'."), lMasterServerAddress.c_str());
		Close(true);
		return;
	}
	mSocketIoHandler->AddFilterIoSocket(mVSocket, lDropCallback);
	assert(mState == CONNECTING);
	mState = CONNECTED;
	mIdleTimer.PopTimeDiff();
}

bool MasterServerConnection::UploadServerInfo()
{
	if (!SendAndAck(_T(MASTER_SERVER_USI) _T(" ") + mLocalServerInfo))
	{
		return false;
	}
	mLog.Info(_T("Uploaded server info..."));
	mUploadedServerInfo = mLocalServerInfo;
	mIsConnectError = false;
	return true;
}

bool MasterServerConnection::DownloadServerList()
{
	if (!SendAndRecv(_T(MASTER_SERVER_DSL) _T(" ") + mServerSortCriterias, mServerList))
	{
		return false;
	}
	mIsConnectError = false;
	return true;
}

MasterServerConnection::FirewallStatus MasterServerConnection::OpenFirewall()
{
	strutil::strvec lVector = strutil::Split(mGameServerConnectAddress, _T(":"));
	if (lVector.size() != 2)
	{
		return FIREWALL_ERROR;
	}
	str lReply;
	if (!SendAndRecv(_T(MASTER_SERVER_OF) _T(" --address ") + lVector[0] + _T(" --port ") + lVector[1], lReply))
	{
		return FIREWALL_ERROR;
	}
	ServerInfo lInfo;
	if (lReply == _T("OK"))
	{
		mIsConnectError = false;
		return FIREWALL_OPENED;
	}
	else if (MasterServerNetworkParser::ExtractServerInfo(lReply, lInfo, 0))
	{
		if (lInfo.mCommand == _T(MASTER_SERVER_UL))
		{
			mLanGameServerAddress = lInfo.mInternalIpAddress + strutil::Format(_T(":%i"), lInfo.mInternalPort);
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
	return lReply == _T("OK");
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
		mLog.Warning(_T("Trying to send to master server even though unconnected."));
		Close(false);
		return false;
	}
	if (pData.size() > 300)
	{
		mLog.Warning(_T("Trying to send too big chunk to master server."));
		return false;
	}
	uint8 lRawData[1024];
	unsigned lSendByteCount = MasterServerNetworkParser::StrToRaw(lRawData, wstrutil::Encode(pData));
	if ((unsigned)mVSocket->DirectSend(lRawData, lSendByteCount) != lSendByteCount)
	{
		mLog.Warning(_T("Transmission to master server failed."));
		return false;
	}
	return true;
}

bool MasterServerConnection::Receive(str& pData)
{
	if (!mVSocket || !QueryMuxValid())
	{
		mLog.Warning(_T("Trying to receive master server data even though unconnected."));
		return false;
	}
	mVSocket->WaitAvailable(1.0);
	uint8 lRawData[1024];
	int lRawSize = mVSocket->Receive(lRawData, sizeof(lRawData));
	if (lRawSize <= 0)
	{
		mLog.Warning(_T("Tried to receive data from master server, but it didn't send us any!"));
		return false;
	}
	wstr lWideData;
	if (!MasterServerNetworkParser::RawToStr(lWideData, lRawData, lRawSize))
	{
		mLog.Error(_T("Got garbled data from master server - something is seriously wrong!"));
		return false;
	}
	pData = strutil::Encode(lWideData);
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
		Send(_T(MASTER_SERVER_DC));
		mSocketIoHandler->KillIoSocket(mVSocket);
	}
	assert(mVSocket == 0);
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
	//assert(pSocket == mVSocket);
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

LOG_CLASS_DEFINE(NETWORK_SERVER, MasterServerConnection);



}
