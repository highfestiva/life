
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



class FileServer
{
public:
	FileServer();
	virtual ~FileServer();
	void Start();
	void Stop();

private:
	char ReadCommand(TcpSocket* pSocket, astr& pData);
	char WriteCommand(TcpSocket* pSocket, char pCommand, const astr& pData);
	void ClientCommandEntry(TcpSocket* pSocket);
	void AcceptThreadEntry();

	MemberThread<FileServer>* mAcceptThread;
	TcpListenerSocket* mAcceptSocket;
};



}
