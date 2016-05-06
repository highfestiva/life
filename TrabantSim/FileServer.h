
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/memberthread.h"
#include "trabantsim.h"



namespace lepra {
class TcpSocket;
class TcpListenerSocket;
}



namespace TrabantSim {



struct SyncDelegate {
	virtual bool WillSync(const str& hostname) = 0;
	virtual void DidSync() = 0;
};



class FileServer {
public:
	FileServer(SyncDelegate* sync_delegate);
	virtual ~FileServer();
	void Start();
	void Stop();

private:
	char ReadCommand(TcpSocket* socket, str& data);
	char WriteCommand(TcpSocket* socket, char command, const str& data);
	void ClientCommandEntry(TcpSocket* socket);
	void AcceptThreadEntry();

	MemberThread<FileServer>* accept_thread_;
	TcpListenerSocket* accept_socket_;
	SyncDelegate* sync_delegate_;

	logclass();
};



}
