
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games
 


#include "../../Lepra/Include/IOBuffer.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Reader.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../Lepra/Include/Timer.h"
#include "../Include/NetworkClient.h"
#include "../Include/Packet.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



NetworkClient::NetworkClient(RuntimeVariableScope* pVariableScope):
	NetworkAgent(pVariableScope),
	mSocket(0),
	mLoginAccountId(0),
	mIsConnecting(false),
	mIsLoggingIn(false),
	mServerHost(),
	mConnectTimeout(0),
	mLoginToken(),
	mLoginTimeout(0),
	mLoginThread("LoginThread"),
	mSafeReceiveToggle(true)
{
}

NetworkClient::~NetworkClient()
{
	Disconnect(true);
}

bool NetworkClient::Open(const str& pLocalAddress)
{
	ScopeLock lLock(&mLock);

	SendDisconnect();
	Stop();

	bool lOk = true;
	SocketAddress lLocalAddress;
	uint16 lEndPort = 0;
	if (lOk)
	{
		lOk = lLocalAddress.ResolveRange(pLocalAddress, lEndPort);
		assert(lOk);
	}
	if (lOk)
	{
		ScopeLock lLock(&mLock);
		for (; lLocalAddress.GetPort() <= lEndPort; lLocalAddress.SetPort(lLocalAddress.GetPort()+1))
		{
			SetMuxSocket(new MuxSocket(_T("Client "), lLocalAddress, false));
			if (mMuxSocket->IsOpen())
			{
				break;
			}
			delete (mMuxSocket);
			mMuxSocket = 0;
		}
		lOk = (mMuxSocket != 0);
	}
	return (lOk);
}

void NetworkClient::Stop()
{
	ScopeLock lLock(&mLock);
	if (mSocket)
	{
		mMuxSocket->CloseSocket(mSocket);
		mSocket = 0;
	}
	if (mMuxSocket)
	{
		mMuxSocket->Close();
	}
	StopLoginThread();
	Parent::Stop();
}



bool NetworkClient::Connect(const str& pServerAddress, double pTimeout)
{
	ScopeLock lLock(&mLock);

	bool lOk = true;

	SocketAddress lTargetAddress;
	if (lOk)
	{
		lOk = lTargetAddress.Resolve(pServerAddress);
		if (!lOk)
		{
			mLog.Warningf(_T("Could not resolve server address '%s'."), pServerAddress.c_str());
		}
	}
	if (lOk)
	{
		const std::string lConnectionId = SystemManager::GetRandomId();
		lLock.Release();
		mSocket = mMuxSocket->Connect(lTargetAddress, lConnectionId, pTimeout);
		lLock.Acquire();
		lOk = (mSocket != 0);
	}
	return (lOk);
}

void NetworkClient::Disconnect(bool pSendDisconnect)
{
	StopLoginThread();

	ScopeLock lLock(&mLock);
	while (!mSocketReceiveFilterTable.empty())
	{
		VSocket* lSocket = mSocketReceiveFilterTable.begin()->first;
		KillIoSocket(lSocket);
	}
	if (pSendDisconnect)
	{
		SendDisconnect();
	}
	SetLoginAccountId(0);
	Thread::Sleep(0.1);	// Try to wait until data sent. SO_LINGER doesn't seem trustworthy.
	Stop();
}



void NetworkClient::StartConnectLogin(const str& pServerHost, double pConnectTimeout, const Cure::LoginId& pLoginToken)
{
	assert(!mIsConnecting);
	assert(!mIsLoggingIn);
	mLoginAccountId = 0;
	mIsConnecting = true;
	mIsLoggingIn = true;

	if (pConnectTimeout != 0)
	{
		mServerHost = pServerHost;
	}
	assert(pConnectTimeout >= 0);
	mConnectTimeout = pConnectTimeout;
	mLoginToken = pLoginToken;
	CURE_RTVAR_GET(mLoginTimeout, =, mVariableScope, RTVAR_NETWORK_LOGIN_TIMEOUT, 3.0);
	assert(mLoginTimeout > 0);
	StopLoginThread();
	mLoginThread.Start(this, &NetworkClient::LoginEntry);
}

RemoteStatus NetworkClient::WaitLogin()
{
	const int lCheckCount = 50;
	const double lSleepTime = mConnectTimeout/lCheckCount;
	for (int x = 0; IsConnecting() && x < lCheckCount; ++x)
	{
		Thread::Sleep(lSleepTime);
	}

	RemoteStatus lStatus = REMOTE_UNKNOWN;
	if (mSocket == 0)
	{
		lStatus = REMOTE_NO_CONNECTION;
	}
	else
	{
		Cure::Packet* lPacket = GetPacketFactory()->Allocate();
		for (int x = 0; x < 100000 && IsLoggingIn(); ++x)
		{
			ReceiveStatus lReceived = ReceiveNonBlocking(lPacket);
			switch (lReceived)
			{
				case Cure::NetworkAgent::RECEIVE_OK:
				{
					const size_t lMessageCount = lPacket->GetMessageCount();
					if (lMessageCount >= 1)
					{
						Cure::Message* lMessage = lPacket->GetMessageAt(0);
						if (lMessage->GetType() == MESSAGE_TYPE_STATUS)
						{
							Cure::MessageStatus* lMessageStatus = (Cure::MessageStatus*)lMessage;
							lStatus = lMessageStatus->GetRemoteStatus();
							SetLoginAccountId(lMessageStatus->GetInteger());
						}
					}
				}
				break;
				case Cure::NetworkAgent::RECEIVE_PARSE_ERROR:
				{
					mLog.AError("Problem with receiving crap data during login wait!");
					lStatus = REMOTE_LOGIN_ERRONOUS_DATA;
					SetLoginAccountId(0);
				}
				break;
				case Cure::NetworkAgent::RECEIVE_CONNECTION_BROKEN:
				{
					mLog.AError("Disconnected from server while waiting for login!");
					lStatus = REMOTE_UNKNOWN;
					SetLoginAccountId(0);
				}
				break;
				case Cure::NetworkAgent::RECEIVE_NO_DATA:
				{
					// Nothing, really.
				}
				break;
			}

			if (IsLoggingIn())
			{
				Thread::Sleep(0.05);
			}
		}
		GetPacketFactory()->Release(lPacket);
	}
	StopLoginThread();

	return (lStatus);
}

bool NetworkClient::IsActive() const
{
	return (IsConnecting() || IsLoggingIn() || GetLoginAccountId() != 0);
}

bool NetworkClient::IsConnecting() const
{
	return (mIsConnecting);
}

bool NetworkClient::IsLoggingIn() const
{
	return (mIsLoggingIn);
}

uint32 NetworkClient::GetLoginAccountId() const
{
	return (mLoginAccountId);
}

void NetworkClient::SetLoginAccountId(uint32 pLoginAccountId)
{
	mLoginAccountId = pLoginAccountId;
	mIsLoggingIn = false;
}



NetworkAgent::VSocket* NetworkClient::GetSocket() const
{
	return (mSocket);
}



bool NetworkClient::SendAll()
{
	ScopeLock lLock(&mLock);
	bool lOk = true;
	VSocket* lSocket;
	while (mMuxSocket && (lSocket = mMuxSocket->PopSenderSocket()) != 0)
	{
		lOk &= (lSocket->SendBuffer() > 0);
	}
	return (lOk);
}

NetworkAgent::ReceiveStatus NetworkClient::ReceiveNonBlocking(Packet* pPacket)
{
	ReceiveStatus lResult = RECEIVE_CONNECTION_BROKEN;
	ScopeLock lLock(&mLock);
	if (mSocket)
	{
		pPacket->Release();
		mSafeReceiveToggle = !mSafeReceiveToggle;
		const int lDataLength = mSocket->Receive(mSafeReceiveToggle, pPacket->GetWriteBuffer(), pPacket->GetBufferSize());
		if (lDataLength == 0)
		{
			lResult = RECEIVE_NO_DATA;
		}
		else if (lDataLength > 0)
		{
			pPacket->SetPacketSize(lDataLength);
			if (pPacket->Parse() == Packet::PARSE_OK)
			{
				lResult = RECEIVE_OK;
			}
			else
			{
				lResult = RECEIVE_PARSE_ERROR;
			}
		}
		else
		{
			lResult = RECEIVE_CONNECTION_BROKEN;
		}
	}
	return (lResult);
}

NetworkAgent::ReceiveStatus NetworkClient::ReceiveTimeout(Packet* pPacket, double pTimeout)
{
	const double lSingleLoopTime = 0.05;
	const int lLoopCount = (int)(pTimeout/lSingleLoopTime)+1;
	ReceiveStatus lStatus = RECEIVE_NO_DATA;
	for (int x = 0; lStatus == RECEIVE_NO_DATA && x < lLoopCount; ++x)
	{
		lStatus = ReceiveNonBlocking(pPacket);
		if (lStatus == RECEIVE_NO_DATA)
		{
			Thread::Sleep(lSingleLoopTime);
		}
	}
	return (lStatus);
}

NetworkAgent::ReceiveStatus NetworkClient::ReceiveMore(Packet* pPacket)
{
	ScopeLock lLock(&mLock);
	if (!mSocket)
	{
		return RECEIVE_CONNECTION_BROKEN;
	}

	ReceiveStatus lResult = RECEIVE_CONNECTION_BROKEN;
	const int lDataLength = mSocket->Receive(true, pPacket->GetWriteBuffer() + pPacket->GetPacketSize(),
		pPacket->GetBufferSize() - pPacket->GetPacketSize());
	if (lDataLength == 0)
	{
		lResult = RECEIVE_NO_DATA;
	}
	else if (lDataLength > 0)
	{
		pPacket->SetPacketSize(pPacket->GetPacketSize() + lDataLength);
		lResult = RECEIVE_OK;
	}
	return lResult;
}



bool NetworkClient::SendLoginRequest(const LoginId& pLoginId)
{
	bool lOk = false;
	ScopeLock lLock(&mLock);
	if (mSocket)
	{
		Packet* lPacket = mPacketFactory->Allocate();
		lPacket->Release();
		MessageLoginRequest* lLoginRequest = (MessageLoginRequest*)mPacketFactory->GetMessageFactory()->Allocate(MESSAGE_TYPE_LOGIN_REQUEST);
		lPacket->AddMessage(lLoginRequest);	// Leave ownership of message with packet.
		lLoginRequest->Store(lPacket, pLoginId.GetName(), pLoginId.GetMangledPassword());
		lOk = Parent::PlaceInSendBuffer(true, mSocket, lPacket);
		if (lOk)
		{
			lOk = SendAll();
		}
		mPacketFactory->Release(lPacket);
	}
	return (lOk);
}

void NetworkClient::SendDisconnect()
{
	if (mSocket)
	{
		Cure::Packet* lPacket = GetPacketFactory()->Allocate();
		SendStatusMessage(mSocket, 0, Cure::REMOTE_NO_CONNECTION, Cure::MessageStatus::INFO_LOGIN, L"", lPacket);
		GetPacketFactory()->Release(lPacket);
	}
	SendAll();
}

void NetworkClient::LoginEntry()
{
	bool lOk = true;
	if (lOk && mConnectTimeout > 0)
	{
		mLog.Info(_T("Connecting to ") + mServerHost + _T("."));
		int x = 0;
		do
		{
			if (!lOk)
			{
				mLog.AInfo("Retrying connect...");
			}
			lOk = Connect(mServerHost, mConnectTimeout);
		}
		while (++x <= CURE_RTVAR_SLOW_GET(mVariableScope, RTVAR_NETWORK_CONNECT_RETRYCOUNT, 1) && !lOk &&
			!SystemManager::GetQuitRequest() && !mLoginThread.GetStopRequest());
	}
	mIsConnecting = false;
	if (lOk)
	{
		lOk = (mSocket != 0);	// In case we skipped trying to connect; we need to be connected prior to this.
	}
	const bool lConnectOk = lOk;
	if (lOk)
	{
		mLog.AInfo("Connected.");
		lOk = SendLoginRequest(mLoginToken);
	}
	if (lOk)
	{
		Timer lTimer;
		while (mIsLoggingIn && lTimer.GetTimeDiff() < mLoginTimeout &&
			!SystemManager::GetQuitRequest() && !mLoginThread.GetStopRequest())
		{
			Thread::Sleep(0.05);
			lTimer.UpdateTimer();
		}
		lOk = (mLoginAccountId != 0);
	}
	mIsLoggingIn = false;
	if (lOk)
	{
		mLog.Infof(_T("Logged in on account # %u."), mLoginAccountId);
		// TODO: handshaking for pingtime with server.
	}
	if (!lOk)
	{
		if (!lConnectOk)
		{
			mLog.AError("Could not connect.");
		}
		else
		{
			mLog.AError("Could not login.");
		}
		Disconnect(false);
	}
}

void NetworkClient::StopLoginThread()
{
	if (Thread::GetCurrentThreadId() != mLoginThread.GetThreadId())
	{
		mLoginThread.Join(mConnectTimeout+mLoginTimeout+0.5);
		mLoginThread.Kill();
	}
}



NetworkClient::MuxIoSocket* NetworkClient::GetMuxIoSocket() const
{
	return mMuxSocket;
}

void NetworkClient::AddFilterIoSocket(VIoSocket* pSocket, const DropFilterCallback& pOnDropCallback)
{
	mSocketReceiveFilterTable.insert(SocketReceiveFilterTable::value_type(pSocket, pOnDropCallback));
}

void NetworkClient::KillIoSocket(VIoSocket* pSocket)
{
	if (pSocket)
	{
		pSocket->SendBuffer();
	}
	SocketReceiveFilterTable::iterator x = mSocketReceiveFilterTable.find(pSocket);
	if (x != mSocketReceiveFilterTable.end())
	{
		x->second(x->first);
		mSocketReceiveFilterTable.erase(x);
	}
	if (pSocket)
	{
		mMuxSocket->CloseSocket(pSocket);
	}
}



/*void NetworkClient::OnCloseSocket(VSocket*)
{
	Disconnect(false);
}*/



LOG_CLASS_DEFINE(NETWORK_CLIENT, NetworkClient);



}
