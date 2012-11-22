
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <list>
#include "../../Cure/Include/SocketIoHandler.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../Lepra/Include/MemberThread.h"
#include "../ServerInfo.h"



namespace Life
{



class MasterServerConnection
{
public:
	enum FirewallStatus
	{
		FIREWALL_ERROR,
		FIREWALL_OPENED,
		FIREWALL_USE_LAN,
	};

	MasterServerConnection(const str& pMasterAddress);
	virtual ~MasterServerConnection();

	void SetSocketInfo(Cure::SocketIoHandler* pMuxSocket, double pConnectTimeout);

	void SendLocalInfo(const str& pLocalServerInfo);
	void AppendLocalInfo(const str& pExtraServerInfo);
	void RequestServerList(const str& pCriterias);
	void RequestOpenFirewall(const str& pGameServerConnectAddress);
	bool UpdateServerList(ServerInfoList& pServerList) const;
	str GetServerListAsText() const;
	bool IsConnectError() const;
	FirewallStatus GetFirewallOpenStatus() const;
	const str& GetLanServerConnectAddress() const;
	double WaitUntilDone(double pTimeout, bool pAllowReconnect);
	void GraceClose(double pTimeout, bool pWaitUntilDone);
	bool CloseUnlessUploaded();
	void Tick();
	bool TickReceive(ServerInfo& pServerInfo);

private:
	enum State
	{
		CONNECT,
		CONNECTED,
		UPLOAD_INFO,
		DOWNLOAD_LIST,
		OPEN_FIREWALL,
		DISCONNECTED,
		CONNECTING,
	};

	void QueryAddState(State pState);
	void TriggerConnectTimer();
	void StepState();
	void ConnectEntry();
	bool UploadServerInfo();
	bool DownloadServerList();
	FirewallStatus OpenFirewall();
	bool SendAndAck(const str& pData);
	bool SendAndRecv(const str& pData, str& pReply);
	bool Send(const str& pData);
	bool Receive(str& pData);
	void Close(bool pError);
	bool QueryMuxValid();

	void OnDropSocket(Cure::SocketIoHandler::VIoSocket* pSocket);

	str mMasterServerAddress;
	State mState;
	std::list<State> mStateList;
	str mLocalServerInfo;
	str mUploadedServerInfo;
	str mServerSortCriterias;
	str mServerList;
	str mGameServerConnectAddress;
	str mLanGameServerAddress;
	double mConnectTimeout;
	Cure::SocketIoHandler* mSocketIoHandler;
	Cure::SocketIoHandler::MuxIoSocket* mMuxSocket;
	Cure::SocketIoHandler::VIoSocket* mVSocket;
	MemberThread<MasterServerConnection>* mConnecter;
	volatile int mDisconnectCounter;
	HiResTimer mIdleTimer;
	HiResTimer mUploadTimeout;
	bool mIsConnectError;
	FirewallStatus mLastFirewallOpenStatus;
	const static double mConnectedIdleTimeout;
	const static double mDisconnectedIdleTimeout;
	const static double mServerInfoTimeout;

	LOG_CLASS_DECLARE();
};



}
