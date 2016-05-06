
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/memberthread.h"
#include "networkagent.h"
#include "remotestatus.h"
#include "socketiohandler.h"
#include "useraccount.h"



namespace cure {



class Packet;



class NetworkClient: public NetworkAgent, public SocketIoHandler {
public:
	typedef NetworkAgent Parent;

	NetworkClient(RuntimeVariableScope* variable_scope);
	virtual ~NetworkClient();

	bool Open(const str& local_address);
	virtual void Stop();

	virtual bool Connect(const str& server_address, double timeout);
	void Disconnect(bool send_disconnect);

	void StartConnectLogin(const str& server_host, double connect_timeout, const cure::LoginId& login_token);
	// Only use this dummy login for testing.
	RemoteStatus WaitLogin();
	bool IsActive() const;
	bool IsConnecting() const;
	bool IsLoggingIn() const;
	uint32 GetLoginAccountId() const;
	void SetLoginAccountId(uint32 login_account_id);

	VSocket* GetSocket() const;

	virtual bool SendAll();
	virtual ReceiveStatus ReceiveNonBlocking(Packet* packet);
	virtual ReceiveStatus ReceiveTimeout(Packet* packet, double timeout);
	ReceiveStatus ReceiveMore(Packet* packet);

private:
	bool SendLoginRequest(const LoginId& login_id);
	void SendDisconnect();
	void LoginEntry();
	void StopLoginThread();

	virtual MuxIoSocket* GetMuxIoSocket() const;
	virtual void AddFilterIoSocket(VIoSocket* socket, const DropFilterCallback& on_drop_callback);
	virtual void RemoveAllFilterIoSockets();
	virtual void KillIoSocket(VIoSocket* socket);

	//void OnCloseSocket(VSocket*);

	typedef std::unordered_map<VSocket*, DropFilterCallback, LEPRA_VOIDP_HASHER> SocketReceiveFilterTable;

	VSocket* socket_;
	uint32 login_account_id_;
	bool is_connecting_;
	bool is_logging_in_;
	bool is_socket_connecting_;
	str server_host_;
	double connect_timeout_;
	cure::LoginId login_token_;
	double login_timeout_;
	MemberThread<NetworkClient> login_thread_;
	bool safe_receive_toggle_;
	SocketReceiveFilterTable socket_receive_filter_table_;

	logclass();
};



}
