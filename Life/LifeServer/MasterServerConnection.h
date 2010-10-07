
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
	MasterServerConnection();
	virtual ~MasterServerConnection();

	void SetSocketInfo(Cure::SocketIoHandler* pMuxSocket, double pConnectTimeout);

	void SendLocalInfo(const str& pLocalServerInfo);
	void AppendLocalInfo(const str& pExtraServerInfo);
	void RequestServerList(const str& pCriterias);
	bool UpdateServerList(ServerInfoList& pServerList) const;
	str GetServerListAsText() const;
	bool IsConnectError() const;
	double WaitUntilDone(double pTimeout, bool pAllowReconnect);
	void GraceClose(double pTimeout, bool pWaitUntilDone);
	bool CloseUnlessUploaded();
	void Tick();

private:
	enum State
	{
		CONNECT,
		CONNECTED,
		UPLOAD_INFO,
		DOWNLOAD_LIST,
		DISCONNECTED,
		WORKING,
	};

	void QueryAddState(State pState);
	void StepState();
	void ConnectEntry();
	bool UploadServerInfo();
	bool DownloadServerList();
	bool SendAndAck(const str& pData);
	bool SendAndRecv(const str& pData, str& pReply);
	bool Send(const str& pData);
	bool Receive(str& pData);
	void Close(bool pError);
	bool QueryMuxValid();

	void OnDropSocket(Cure::SocketIoHandler::VIoSocket* pSocket);

	State mState;
	std::list<State> mStateList;
	str mLocalServerInfo;
	str mUploadedServerInfo;
	str mServerSortCriterias;
	str mServerList;
	double mConnectTimeout;
	Cure::SocketIoHandler* mSocketIoHandler;
	Cure::SocketIoHandler::MuxIoSocket* mMuxSocket;
	Cure::SocketIoHandler::VIoSocket* mVSocket;
	MemberThread<MasterServerConnection>* mConnecter;
	volatile int mDisconnectCounter;
	HiResTimer mIdleTimer;
	bool mIsConnectError;
	const static double mConnectedIdleTimeout;
	const static double mDisconnectedIdleTimeout;

	LOG_CLASS_DECLARE();
};



}
