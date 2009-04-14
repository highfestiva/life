
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include <hash_map>
#include <hash_set>
#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Timer.h"
#include "UserConnection.h"
#include "NetworkAgent.h"
#include "UserAccountManager.h"



namespace Cure
{



class ContextObject;
class MessageLoginRequest;



class NetworkServer: public NetworkAgent
{
public:
	typedef NetworkAgent Parent;

	class LoginListener
	{
	public:
		LoginListener();
		virtual ~LoginListener();
		virtual UserAccount::Availability QueryLogin(const LoginId& pLoginId, UserAccount::AccountId& pAccountId) = 0;
		virtual void OnLogin(UserConnection* pUserConnection) = 0;
		virtual void OnLogout(UserConnection* pUserConnection) = 0;
	};

	NetworkServer(RuntimeVariableScope* pVariableScope, LoginListener* pLoginListener);
	virtual ~NetworkServer();

	virtual bool Start(const Lepra::String& pHostAddress);
	virtual void Stop();

	void Disconnect(UserAccount::AccountId pAccountId, const Lepra::String& pReason, bool pSendDisconnect);

	bool PlaceInSendBuffer(bool pSafe, Packet* pPacket, UserAccount::AccountId pAccountId);
	bool SendAll();
	ReceiveStatus ReceiveFirstPacket(Packet* pPacket, UserAccount::AccountId& pAccountId);

private:
	void PollAccept();
	void TryLogin(Lepra::GameSocket* pSocket, Packet* pPacket, int pDataLength);
	RemoteStatus QueryLogin(const Lepra::UnicodeString& pLoginName, MessageLoginRequest* pLoginRequest, UserAccount::AccountId& pAccountId);
	void Login(const Lepra::UnicodeString& pUserName, UserAccount::AccountId pAccountId, Lepra::GameSocket* pSocket, Packet* pPacket);
	RemoteStatus ManageLogin(Lepra::GameSocket* pSocket, Packet* pPacket);
	void AddUser(UserConnection* pUserConnection, UserAccount::AccountId& pAccountId);
	bool RemoveUser(UserAccount::AccountId pAccountId, bool pDestroy);
	void KillDeadSockets();
	void DropSocket(Lepra::GameSocket* pSocket);
	UserConnection* GetUser(UserAccount::AccountId pAccountId);

	bool SendStatusMessage(UserAccount::AccountId pAccountId, Lepra::int32 pInteger, RemoteStatus pStatus, Lepra::UnicodeString pMessage, Packet* pPacket);

	void OnCloseSocket(Lepra::GameSocket* pSocket);

	typedef std::hash_set<Lepra::GameSocket*, std::hash<void*> > SocketSet;
	typedef SocketSet PendingSocketTable;
	typedef std::hash_map<UserAccount::AccountId, UserConnection*> LoggedInIdUserTable;
	typedef std::pair<UserAccount::AccountId, UserConnection*> LoggedInIdUserPair;
	typedef std::hash_map<Lepra::UnicodeString, UserConnection*> LoggedInNameUserTable;
	typedef std::pair<Lepra::UnicodeString, UserConnection*> LoggedInNameUserPair;
	typedef std::hash_map<Lepra::GameSocket*, UserConnection*, std::hash<void*> > SocketUserTable;
	typedef std::pair<Lepra::GameSocket*, UserConnection*> SocketUserPair;
	typedef SocketSet SocketTimeoutTable;
	typedef std::hash_set<UserAccount::AccountId> AccountIdSet;

	UserConnectionFactory* mUserConnectionFactory;
	LoginListener* mLoginListener;
	PendingSocketTable mPendingLoginTable;
	LoggedInIdUserTable mLoggedInIdUserTable;
	LoggedInNameUserTable mLoggedInNameUserTable;
	SocketUserTable mSocketUserTable;
	SocketTimeoutTable mSocketTimeoutTable;
	AccountIdSet mDropUserList;

	Lepra::Timer mKeepaliveTimer;

	LOG_CLASS_DECLARE();
};



}
