
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/MemberThread.h"
#include "NetworkAgent.h"
#include "RemoteStatus.h"
#include "UserAccount.h"



namespace Cure
{



class Packet;



class NetworkClient: public NetworkAgent
{
public:
	typedef NetworkAgent Parent;

	NetworkClient(RuntimeVariableScope* pVariableScope);
	virtual ~NetworkClient();

	virtual void Stop();

	virtual bool Connect(const Lepra::String& pLocalAddress, const Lepra::String& pServerAddress, double pTimeout);
	void Disconnect(bool pSendDisconnect);

	void StartConnectLogin(const Lepra::String& pServerHost, double pConnectTimeout, const Cure::LoginId& pLoginToken);
	// Only use this dummy login for testing.
	RemoteStatus WaitLogin();
	bool IsActive() const;
	bool IsConnecting() const;
	bool IsLoggingIn() const;
	Lepra::uint32 GetLoginAccountId() const;
	void SetLoginAccountId(Lepra::uint32 pLoginAccountId);

	Lepra::GameSocket* GetSocket() const;

	virtual bool SendAll();
	virtual ReceiveStatus ReceiveNonBlocking(Packet* pPacket);
	virtual ReceiveStatus ReceiveTimeout(Packet* pPacket, double pTimeout);

private:
	bool SendLoginRequest(const LoginId& pLoginId);
	void LoginEntry();
	void StopLoginThread();

	void OnCloseSocket(Lepra::GameSocket*);

	Lepra::GameSocket* mSocket;
	Lepra::uint32 mLoginAccountId;
	bool mIsConnecting;
	bool mIsLoggingIn;
	bool mPollSafeSource;
	Lepra::String mServerHost;
	double mConnectTimeout;
	Cure::LoginId mLoginToken;
	double mLoginTimeout;
	Lepra::MemberThread<NetworkClient> mLoginThread;

	LOG_CLASS_DECLARE();
};



}
