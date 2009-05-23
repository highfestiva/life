
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games
 


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
	mPollSafeSource(true),
	mServerHost(),
	mConnectTimeout(0),
	mLoginToken(),
	mLoginTimeout(0),
	mLoginThread(_T("LoginThread"))
{
}

NetworkClient::~NetworkClient()
{
	Disconnect(true);
}

void NetworkClient::Stop()
{
	Lepra::ScopeLock lLock(&mLock);
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



bool NetworkClient::Connect(const Lepra::String& pLocalAddress, const Lepra::String& pServerAddress, double pTimeout)
{
	Lepra::ScopeLock lLock(&mLock);

	Stop();

	bool lOk = true;
	Lepra::SocketAddress lLocalAddress;
	Lepra::uint16 lEndPort = 0;
	if (lOk)
	{
		lOk = lLocalAddress.ResolveRange(pLocalAddress, lEndPort);
	}
	Lepra::SocketAddress lTargetAddress;
	if (lOk)
	{
		lOk = lTargetAddress.Resolve(pServerAddress);
	}
	if (lOk)
	{
		Lepra::ScopeLock lLock(&mLock);
		for (; lLocalAddress.GetPort() <= lEndPort; lLocalAddress.SetPort(lLocalAddress.GetPort()+1))
		{
			SetMuxSocket(new Lepra::GameMuxSocket(_T("Client "), lLocalAddress, false));
			if (mMuxSocket->IsOpen())
			{
				mMuxSocket->SetCloseCallback(this, &NetworkClient::OnCloseSocket);
				break;
			}
			delete (mMuxSocket);
			mMuxSocket = 0;
		}
		lOk = (mMuxSocket != 0);
	}
	if (lOk)
	{
		mSocket = mMuxSocket->Connect(lTargetAddress, pTimeout);
		lOk = (mSocket != 0);
	}
	return (lOk);
}

void NetworkClient::Disconnect(bool pSendDisconnect)
{
	StopLoginThread();

	Lepra::ScopeLock lLock(&mLock);
	if (mSocket && pSendDisconnect)
	{
		Cure::Packet* lPacket = GetPacketFactory()->Allocate();
		SendStatusMessage(mSocket, 0, Cure::REMOTE_NO_CONNECTION, L"", lPacket);
		GetPacketFactory()->Release(lPacket);
	}
	SetLoginAccountId(0);
	Stop();
}



void NetworkClient::StartConnectLogin(const Lepra::String& pServerHost, double pConnectTimeout, const Cure::LoginId& pLoginToken)
{
	assert(!mIsConnecting);
	assert(!mIsLoggingIn);
	mLoginAccountId = 0;
	mIsConnecting = true;
	mIsLoggingIn = true;

	mServerHost = pServerHost;
	mConnectTimeout = pConnectTimeout;
	mLoginToken = pLoginToken;
	mLoginTimeout = CURE_RTVAR_GET(mVariableScope, RTVAR_NETWORK_LOGIN_TIMEOUT, 3.0);
	StopLoginThread();
	mLoginThread.Start(this, &NetworkClient::LoginEntry);
}

RemoteStatus NetworkClient::WaitLogin()
{
	const int lCheckCount = 50;
	for (int x = 0; IsConnecting() && x < lCheckCount; ++x)
	{
		Lepra::Thread::Sleep(mConnectTimeout/lCheckCount);
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
				case Cure::NetworkAgent::RECEIVE_NO_DATA:
				case Cure::NetworkAgent::RECEIVE_CONNECTION_BROKEN:
				{
					mLog.AError("Disconnected from server while waiting for login!");
					lStatus = REMOTE_UNKNOWN;
					SetLoginAccountId(0);
				}
				break;
			}

			if (IsLoggingIn())
			{
				Lepra::Thread::Sleep(0.05);
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

Lepra::uint32 NetworkClient::GetLoginAccountId() const
{
	return (mLoginAccountId);
}

void NetworkClient::SetLoginAccountId(Lepra::uint32 pLoginAccountId)
{
	mIsLoggingIn = false;
	mLoginAccountId = pLoginAccountId;
}



Lepra::GameSocket* NetworkClient::GetSocket() const
{
	return (mSocket);
}



bool NetworkClient::SendAll()
{
	Lepra::ScopeLock lLock(&mLock);
	bool lOk = true;
	if (mSocket && mSocket->HasSendData())
	{
		// We need to pop our socket, otherwise the MUX socket will treat us as
		// if we're going to handle this send asynchronously (later on).
		mMuxSocket->PopSenderSocket();
		lOk = (mSocket->SendBuffer() > 0);
	}
	return (lOk);
}

NetworkAgent::ReceiveStatus NetworkClient::ReceiveNonBlocking(Packet* pPacket)
{
	ReceiveStatus lResult = RECEIVE_CONNECTION_BROKEN;
	Lepra::ScopeLock lLock(&mLock);
	if (mSocket)
	{
		pPacket->Release();
		mPollSafeSource = !mPollSafeSource;
		const int lDataLength = mSocket->Receive(mPollSafeSource, pPacket->GetWriteBuffer(), pPacket->GetBufferSize());
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
			Lepra::Thread::Sleep(lSingleLoopTime);
		}
	}
	return (lStatus);
}



bool NetworkClient::SendLoginRequest(const LoginId& pLoginId)
{
	bool lOk = false;
	Lepra::ScopeLock lLock(&mLock);
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

void NetworkClient::LoginEntry()
{
	bool lOk = true;
	if (lOk && mConnectTimeout >= 0)
	{
		mLog.Info(_T("Connecting to ") + mServerHost + _T("."));
		int x = 0;
		do
		{
			if (!lOk)
			{
				mLog.AInfo("Retrying connect...");
			}
			Lepra::String lPortRange = CURE_RTVAR_GET(mVariableScope, RTVAR_NETWORK_CONNECT_LOCALPORTRANGE, _T("1025-65535"));
			lOk = Connect(_T(":")+lPortRange, mServerHost, mConnectTimeout);
		}
		while (++x <= CURE_RTVAR_GET(mVariableScope, RTVAR_NETWORK_CONNECT_RETRYCOUNT, 1) && !lOk &&
			!Lepra::SystemManager::GetQuitRequest() && !mLoginThread.GetStopRequest());
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
		Lepra::Timer lTimer;
		while (mIsLoggingIn && lTimer.GetTimeDiffF() < mLoginTimeout &&
			!Lepra::SystemManager::GetQuitRequest() && !mLoginThread.GetStopRequest())
		{
			Lepra::Thread::Sleep(0.05);
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
	if (Lepra::Thread::GetCurrentThreadId() != mLoginThread.GetThreadId())
	{
		mLoginThread.Join(mConnectTimeout+mLoginTimeout+0.5);
		mLoginThread.Kill();
	}
}



void NetworkClient::OnCloseSocket(Lepra::GameSocket*)
{
	Disconnect(false);
}



LOG_CLASS_DEFINE(NETWORK_CLIENT, NetworkClient);



}
