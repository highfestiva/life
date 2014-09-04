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
		if (lLocalAddress.Resolve(_T(":10000")))
		{
			mUdpMuxSocket = new UdpMuxSocket(_T("Srv "), lLocalAddress, true);
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

//	printf("\nServer received \"%s\".", astrutil::Encode(lString).c_str());
	
	mTestOk = (lString == _T("Hi Server! I am Client!"));
	deb_assert(mTestOk);

	lWriter.WriteString<Lepra::tchar>(_T("Hi Client! I am Server!\n"));
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
		if (lLocalAddress.Resolve(_T(":10001")))
		{
			mUdpMuxSocket = new UdpMuxSocket(_T("Client "), lLocalAddress, false);
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
	if (lLocalAddress.Resolve(_T(":10000")))
	{
		UdpVSocket* lSocket = mUdpMuxSocket->Connect(lLocalAddress, "", 0.5);
		deb_assert(lSocket);
		Reader lReader(lSocket);
		Writer lWriter(lSocket);

		lWriter.WriteString<Lepra::tchar>(_T("Hi Server! I am Client!\n"));
		lSocket->Flush();

		lSocket->WaitAvailable(0.5);
		str lString;
		lReader.ReadLine(lString);

	//	printf("Client received \"%s\".", astrutil::Encode(lString).c_str());
		
		mTestOk = (lString == _T("Hi Client! I am Server!"));
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

		lContext = _T("Client/Server UdpMuxSocket communication failed");
		lTestOk = lClient.GetTestOK() && lServer.GetTestOK();
		deb_assert(lTestOk);

		Network::Stop();
	}

	ReportTestResult(pAccount, _T("UDPSockets"), lContext, lTestOk);
	return (lTestOk);
}