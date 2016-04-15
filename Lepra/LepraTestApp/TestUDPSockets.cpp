/*
	File:   TestUDPSockets.cpp
	Class:  -
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../Include/Log.h"
#include "../Include/Network.h"
#include "../Include/Socket.h"
#include "../Include/String.h"
#include "../Include/Thread.h"
#include "../Include/Reader.h"
#include "../Include/Writer.h"

using namespace Lepra;

void ReportTestResult(const Lepra::LogDecorator& pLog, const str& pTestName, const str& pContext, bool pResult);

class ServerThread : public Thread
{
public:
	inline ServerThread():
		Thread("ServerThread"),
		mUdpMuxSocket(0)
	{
		mTestOk = false;

		SocketAddress lLocalAddress;
		if (lLocalAddress.Resolve(":10000"))
		{
			mUdpMuxSocket = new UdpMuxSocket("Srv ", lLocalAddress, true);
			deb_assert(mUdpMuxSocket->IsOpen());
		}
	}

	inline ~ServerThread()
	{
		delete mUdpMuxSocket;
	}

	inline bool GetTestOK()
	{
		return (mTestOk);
	}

protected:
	void Run();
private:

	UdpMuxSocket* mUdpMuxSocket;
	bool mTestOk;
};

void ServerThread::Run()
{
	UdpVSocket* lSocket = mUdpMuxSocket->Accept();
	deb_assert(lSocket);
	Reader lReader(lSocket);
	Writer lWriter(lSocket);

	lSocket->WaitAvailable(0.5);

	str lString;
	lReader.ReadLine(lString);

//	printf("\nServer received \"%s\".", lString.c_str());
	
	mTestOk = (lString == "Hi Server! I am Client!");
	deb_assert(mTestOk);

	lWriter.WriteString("Hi Client! I am Server!\n");
	lSocket->Flush();

	mUdpMuxSocket->CloseSocket(lSocket);
}




class ClientThread : public Thread
{
public:
	inline ClientThread() :
		Thread("ClientThread"),
		mUdpMuxSocket(0)
	{
		mTestOk = false;

		SocketAddress lLocalAddress;
		if (lLocalAddress.Resolve(":10001"))
		{
			mUdpMuxSocket = new UdpMuxSocket("Client ", lLocalAddress, false);
			deb_assert(mUdpMuxSocket->IsOpen());
		}
	}

	inline ~ClientThread()
	{
		delete mUdpMuxSocket;
	}

	inline bool GetTestOK()
	{
		return mTestOk;
	}

protected:
	void Run();
private:

	UdpMuxSocket* mUdpMuxSocket;
	bool mTestOk;
};

void ClientThread::Run()
{
	SocketAddress lLocalAddress;
	if (lLocalAddress.Resolve(":10000"))
	{
		UdpVSocket* lSocket = mUdpMuxSocket->Connect(lLocalAddress, "", 0.5);
		deb_assert(lSocket);
		Reader lReader(lSocket);
		Writer lWriter(lSocket);

		lWriter.WriteString("Hi Server! I am Client!\n");
		lSocket->Flush();

		lSocket->WaitAvailable(0.5);
		str lString;
		lReader.ReadLine(lString);

	//	printf("Client received \"%s\".", lString.c_str());
		
		mTestOk = (lString == "Hi Client! I am Server!");
		deb_assert(mTestOk);

		mUdpMuxSocket->CloseSocket(lSocket);
	}
}





bool TestUDPSockets(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = Network::Start();

	if (lTestOk)
	{
		ServerThread lServer;
		lServer.Start();

		ClientThread lClient;
		lClient.Start();

		lServer.GraceJoin(10.0);
		lClient.GraceJoin(10.0);

		lContext = "Client/Server UdpMuxSocket communication failed";
		lTestOk = lClient.GetTestOK() && lServer.GetTestOK();
		deb_assert(lTestOk);

		Network::Stop();
	}

	ReportTestResult(pAccount, "UDPSockets", lContext, lTestOk);
	return (lTestOk);
}
