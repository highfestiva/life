
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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

	virtual bool Start(const str& pHostAddress);
	virtual void Stop();

	void Disconnect(UserAccount::AccountId pAccountId, const str& pReason, bool pSendDisconnect);

	bool PlaceInSendBuffer(bool pSafe, Packet* pPacket, UserAccount::AccountId pAccountId);
	bool SendAll();
	ReceiveStatus ReceiveFirstPacket(Packet* pPacket, UserAccount::AccountId& pAccountId);

private:
	void PollAccept();
	void TryLogin(UdpVSocket* pSocket, Packet* pPacket, int pDataLength);
	RemoteStatus QueryLogin(const wstr& pLoginName, MessageLoginRequest* pLoginRequest, UserAccount::AccountId& pAccountId);
	void Login(const wstr& pUserName, UserAccount::AccountId pAccountId, UdpVSocket* pSocket, Packet* pPacket);
	RemoteStatus ManageLogin(UdpVSocket* pSocket, Packet* pPacket);
	void AddUser(UserConnection* pUserConnection, UserAccount::AccountId& pAccountId);
	bool RemoveUser(UserAccount::AccountId pAccountId, bool pDestroy);
	void KillDeadSockets();
	void DropSocket(UdpVSocket* pSocket);
	UserConnection* GetUser(UserAccount::AccountId pAccountId);

	bool SendStatusMessage(UserAccount::AccountId pAccountId, int32 pInteger, RemoteStatus pStatus,
		MessageStatus::InfoType pInfoType, wstr pMessage, Packet* pPacket);

	void OnCloseSocket(UdpVSocket* pSocket);

	typedef std::hash_set<UdpVSocket*, std::hash<void*> > SocketSet;
	typedef SocketSet PendingSocketTable;
	typedef std::hash_map<UserAccount::AccountId, UserConnection*> LoggedInIdUserTable;
	typedef std::pair<UserAccount::AccountId, UserConnection*> LoggedInIdUserPair;
	typedef std::hash_map<wstr, UserConnection*> LoggedInNameUserTable;
	typedef std::pair<wstr, UserConnection*> LoggedInNameUserPair;
	typedef std::hash_map<UdpVSocket*, UserConnection*, std::hash<void*> > SocketUserTable;
	typedef std::pair<UdpVSocket*, UserConnection*> SocketUserPair;
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

	Timer mKeepaliveTimer;

	LOG_CLASS_DECLARE();
};



}
