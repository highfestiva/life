
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

	VSocket* GetSocket() const;

	virtual bool SendAll();
	virtual ReceiveStatus ReceiveNonBlocking(Packet* pPacket);
	virtual ReceiveStatus ReceiveTimeout(Packet* pPacket, double pTimeout);
	ReceiveStatus ReceiveMore(Packet* pPacket);

private:
	bool SendLoginRequest(const LoginId& pLoginId);
	void SendDisconnect();
	void LoginEntry();
	void StopLoginThread();

	void OnCloseSocket(VSocket*);

	VSocket* mSocket;
	uint32 mLoginAccountId;
	bool mIsConnecting;
	bool mIsLoggingIn;
	str mServerHost;
	double mConnectTimeout;
	Cure::LoginId mLoginToken;
	double mLoginTimeout;
	MemberThread<NetworkClient> mLoginThread;
	bool mSafeReceiveToggle;

	LOG_CLASS_DECLARE();
};



}
