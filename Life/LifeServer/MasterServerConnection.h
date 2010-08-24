
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <list>
#include "../../Lepra/Include/HiResTimer.h"
#include "../../Lepra/Include/MemberThread.h"
#include "../Life.h"



namespace Lepra
{
class TcpSocket;
}



namespace Life
{



class MasterServerConnection
{
public:
	MasterServerConnection();
	virtual ~MasterServerConnection();

	void SendLocalInfo(const str& pLocalServerInfo);
	void AppendLocalInfo(const str& pExtraServerInfo);
	void RequestServerList(const str& pCriterias);
	str GetServerList() const;
	double WaitUntilDone(double pTimeout, bool pAllowReconnect);
	void GraceClose(double pTimeout);
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
	bool Send(const str& pData, str& pReply);
	bool Receive(str& pData);
	void Close();

	State mState;
	std::list<State> mStateList;
	str mLocalServerInfo;
	str mServerSortCriterias;
	TcpSocket* mSocket;
	MemberThread<MasterServerConnection>* mConnecter;
	volatile int mDisconnectCounter;
	HiResTimer mIdleTimer;
	const static double mConnectedIdleTimeout;
	const static double mDisconnectedIdleTimeout;

	LOG_CLASS_DECLARE();
};



}
