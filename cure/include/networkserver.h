
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/unordered.h"
#include "../../lepra/include/unordered.h"
#include "../../lepra/include/memberthread.h"
#include "../../lepra/include/string.h"
#include "../../lepra/include/timer.h"
#include "networkagent.h"
#include "socketiohandler.h"
#include "userconnection.h"
#include "useraccountmanager.h"



namespace cure {



class ContextObject;
class MessageLoginRequest;



class NetworkServer: public NetworkAgent, public SocketIoHandler {
public:
	typedef NetworkAgent Parent;

	class LoginListener {
	public:
		LoginListener();
		virtual ~LoginListener();
		virtual UserAccount::Availability QueryLogin(const LoginId& login_id, UserAccount::AccountId& account_id) = 0;
		virtual void OnLogin(UserConnection* user_connection) = 0;
		virtual void OnLogout(UserConnection* user_connection) = 0;
	};

	NetworkServer(RuntimeVariableScope* variable_scope, LoginListener* login_listener);
	virtual ~NetworkServer();

	virtual bool Start(const str& host_address);
	virtual void Stop();

	void Disconnect(UserAccount::AccountId account_id, const str& reason, bool send_disconnect);

	bool PlaceInSendBuffer(bool safe, Packet* packet, UserAccount::AccountId account_id);
	bool SendAll();
	ReceiveStatus ReceiveFirstPacket(Packet* packet, UserAccount::AccountId& account_id);
	ReceiveStatus ReceiveMore(UserAccount::AccountId account_id, Packet* packet);

private:
	void PollAccept();
	void TryLogin(VSocket* socket, Packet* packet, int data_length);
	RemoteStatus QueryLogin(const str& login_name, MessageLoginRequest* login_request, UserAccount::AccountId& account_id);
	void Login(const str& user_name, UserAccount::AccountId account_id, VSocket* socket, Packet* packet);
	RemoteStatus ManageLogin(VSocket* socket, Packet* packet);
	void AddUser(UserConnection* user_connection, UserAccount::AccountId& account_id);
	bool RemoveUser(UserAccount::AccountId account_id, bool destroy);
	void KillDeadSockets();
	void DropSocket(VSocket* socket);
	UserConnection* GetUser(UserAccount::AccountId account_id);

	bool SendStatusMessage(UserAccount::AccountId account_id, int32 integer, RemoteStatus status,
		MessageStatus::InfoType info_type, str message, Packet* packet);

	//void OnCloseSocket(VSocket* socket);

	virtual MuxIoSocket* GetMuxIoSocket() const;
	virtual void AddFilterIoSocket(VIoSocket* socket, const DropFilterCallback& on_drop_callback);
	virtual void RemoveAllFilterIoSockets();
	virtual void KillIoSocket(VIoSocket* socket);

	typedef std::unordered_set<VSocket*, LEPRA_VOIDP_HASHER> SocketSet;
	typedef SocketSet PendingSocketTable;
	typedef std::unordered_map<UserAccount::AccountId, UserConnection*> LoggedInIdUserTable;
	typedef std::unordered_map<str, UserConnection*> LoggedInNameUserTable;
	typedef std::unordered_map<VSocket*, UserConnection*, LEPRA_VOIDP_HASHER> SocketUserTable;
	typedef SocketSet SocketTimeoutTable;
	typedef std::unordered_map<VSocket*, DropFilterCallback, LEPRA_VOIDP_HASHER> SocketReceiveFilterTable;
	typedef std::unordered_set<UserAccount::AccountId> AccountIdSet;

	UserConnectionFactory* user_connection_factory_;
	LoginListener* login_listener_;
	PendingSocketTable pending_login_table_;
	LoggedInIdUserTable logged_in_id_user_table_;
	LoggedInNameUserTable logged_in_name_user_table_;
	SocketUserTable socket_user_table_;
	SocketTimeoutTable socket_timeout_table_;
	SocketReceiveFilterTable socket_receive_filter_table_;
	AccountIdSet drop_user_list_;

	Timer keepalive_timer_;

	logclass();
};



}
