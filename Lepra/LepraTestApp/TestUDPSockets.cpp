/*
	File:   TestUDPSockets.cpp
	Class:  -
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include <assert.h>
#include "../Include/Log.h"
#include "../Include/String.h"
#include "../Include/Network.h"
#include "../Include/Thread.h"
#include "../Include/Reader.h"
#include "../Include/Writer.h"

using namespace Lepra;

void ReportTestResult(const Lepra::LogDecorator& pLog, const String& pTestName, const String& pContext, bool pResult);

class ServerThread : public Thread
{
public:
	inline ServerThread():
		Thread(_T("ServerThread")),
		mUdpMuxSocket(0)
	{
		mTestOK = false;

		SocketAddress lLocalAddress;
		if (lLocalAddress.Resolve(_T("localhost:10000")))
		{
			mUdpMuxSocket = new UdpMuxSocket(_T("Srv "), lLocalAddress);
		}
	}

	inline ~ServerThread()
	{
		delete mUdpMuxSocket;
	}

	inline bool GetTestOK()
	{
		return (mTestOK);
	}

protected:
	void Run();
private:

	UdpMuxSocket* mUdpMuxSocket;
	bool mTestOK;
};

void ServerThread::Run()
{
	UdpVSocket* lSocket = mUdpMuxSocket->Accept();
	assert(lSocket);
	Reader lReader(lSocket);
	Writer lWriter(lSocket);

	lSocket->WaitAvailable(0.5);

	String lString;
	lReader.ReadLine(lString);

//	printf("\nServer received \"%s\".", AnsiStringUtility::ToOwnCode(lString).c_str());
	
	mTestOK = (lString == _T("Hi Server! I am Client!"));

	lWriter.WriteLine(_T("Hi Client! I am Server!\n"));
	lSocket->Flush();

	mUdpMuxSocket->CloseSocket(lSocket);
}




class ClientThread : public Thread
{
public:
	inline ClientThread() :
		Thread(_T("ClientThread")),
		mUdpMuxSocket(0)
	{
		mTestOK = false;

		SocketAddress lLocalAddress;
		if (lLocalAddress.Resolve(_T("localhost:10001")))
		{
			mUdpMuxSocket = new UdpMuxSocket(_T("Client "), lLocalAddress);
		}
	}

	inline ~ClientThread()
	{
		delete mUdpMuxSocket;
	}

	inline bool GetTestOK()
	{
		return mTestOK;
	}

protected:
	void Run();
private:

	UdpMuxSocket* mUdpMuxSocket;
	bool mTestOK;
};

void ClientThread::Run()
{
	SocketAddress lLocalAddress;
	if (lLocalAddress.Resolve(_T("localhost:10000")))
	{
		UdpVSocket* lSocket = mUdpMuxSocket->Connect(lLocalAddress, "", 0.5);
		assert(lSocket);
		Reader lReader(lSocket);
		Writer lWriter(lSocket);

		lWriter.WriteLine(_T("Hi Server! I am Client!\n"));
		lSocket->Flush();

		lSocket->WaitAvailable(0.5);
		String lString;
		lReader.ReadLine(lString);

	//	printf("Client received \"%s\".", AnsiStringUtility::ToOwnCode(lString).c_str());
		
		mTestOK = (lString == _T("Hi Client! I am Server!"));

		mUdpMuxSocket->CloseSocket(lSocket);
	}
}





bool TestUDPSockets(const LogDecorator& pAccount)
{
	String lContext;
	bool lTestOk = Network::Start();

	if (lTestOk)
	{
		ServerThread lServer;
		lServer.Start();

		ClientThread lClient;
		lClient.Start();

		lServer.Join(10.0);
		lClient.Join(10.0);

		lContext = _T("Client/Server UdpMuxSocket communication failed");
		lTestOk = lClient.GetTestOK() && lServer.GetTestOK();
		assert(lTestOk);

		Network::Stop();
	}

	ReportTestResult(pAccount, _T("UDPSockets"), lContext, lTestOk);
	return (lTestOk);
}