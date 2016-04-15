
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/MemberThread.h"
#include "TrabantSim.h"



namespace Lepra
{
class TcpSocket;
class TcpListenerSocket;
}



namespace TrabantSim
{



struct SyncDelegate
{
	virtual bool WillSync(const str& pHostname) = 0;
	virtual void DidSync() = 0;
};



class FileServer
{
public:
	FileServer(SyncDelegate* pSyncDelegate);
	virtual ~FileServer();
	void Start();
	void Stop();

private:
	char ReadCommand(TcpSocket* pSocket, str& pData);
	char WriteCommand(TcpSocket* pSocket, char pCommand, const str& pData);
	void ClientCommandEntry(TcpSocket* pSocket);
	void AcceptThreadEntry();

	MemberThread<FileServer>* mAcceptThread;
	TcpListenerSocket* mAcceptSocket;
	SyncDelegate* mSyncDelegate;

	logclass();
};



}
