
// Author: Jonas Bystr�m
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

	virtual bool Connect(const str& pLocalAddress, const str& pServerAddress, double pTimeout);
	void Disconnect(bool pSendDisconnect);

	void StartConnectLogin(const str& pServerHost, double pConnectTimeout, const Cure::LoginId& pLoginToken);
	// Only use this dummy login for testing.
	RemoteStatus WaitLogin();
	bool IsActive() const;
	bool IsConnecting() const;
	bool IsLoggingIn() const;
	uint32 GetLoginAccountId() const;
	void SetLoginAccountId(uint32 pLoginAccountId);

	GameSocket* GetSocket() const;

	virtual bool SendAll();
	virtual ReceiveStatus ReceiveNonBlocking(Packet* pPacket);
	virtual ReceiveStatus ReceiveTimeout(Packet* pPacket, double pTimeout);

private:
	bool SendLoginRequest(const LoginId& pLoginId);
	void LoginEntry();
	void StopLoginThread();

	void OnCloseSocket(GameSocket*);

	GameSocket* mSocket;
	uint32 mLoginAccountId;
	bool mIsConnecting;
	bool mIsLoggingIn;
	bool mPollSafeSource;
	str mServerHost;
	double mConnectTimeout;
	Cure::LoginId mLoginToken;
	double mLoginTimeout;
	MemberThread<NetworkClient> mLoginThread;

	LOG_CLASS_DECLARE();
};



}
