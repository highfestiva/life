
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <hash_map>
#include <hash_set>
#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Timer.h"
#include "NetworkAgent.h"
#include "SocketIoHandler.h"
#include "UserConnection.h"
#include "UserAccountManager.h"



namespace Cure
{



class ContextObject;
class MessageLoginRequest;



class NetworkServer: public NetworkAgent, public SocketIoHandler
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
	ReceiveStatus ReceiveMore(UserAccount::AccountId pAccountId, Packet* pPacket);

private:
	void PollAccept();
	void TryLogin(VSocket* pSocket, Packet* pPacket, int pDataLength);
	RemoteStatus QueryLogin(const wstr& pLoginName, MessageLoginRequest* pLoginRequest, UserAccount::AccountId& pAccountId);
	void Login(const wstr& pUserName, UserAccount::AccountId pAccountId, VSocket* pSocket, Packet* pPacket);
	RemoteStatus ManageLogin(VSocket* pSocket, Packet* pPacket);
	void AddUser(UserConnection* pUserConnection, UserAccount::AccountId& pAccountId);
	bool RemoveUser(UserAccount::AccountId pAccountId, bool pDestroy);
	void KillDeadSockets();
	void DropSocket(VSocket* pSocket);
	UserConnection* GetUser(UserAccount::AccountId pAccountId);

	bool SendStatusMessage(UserAccount::AccountId pAccountId, int32 pInteger, RemoteStatus pStatus,
		MessageStatus::InfoType pInfoType, wstr pMessage, Packet* pPacket);

	//void OnCloseSocket(VSocket* pSocket);

	virtual MuxIoSocket* GetMuxIoSocket() const;
	virtual void AddFilterIoSocket(VIoSocket* pSocket, const DropFilterCallback& pOnDropCallback);
	virtual void RemoveAllFilterIoSockets();
	virtual void KillIoSocket(VIoSocket* pSocket);

	typedef std::hash_set<VSocket*, LEPRA_VOIDP_HASHER> SocketSet;
	typedef SocketSet PendingSocketTable;
	typedef std::hash_map<UserAccount::AccountId, UserConnection*> LoggedInIdUserTable;
	typedef std::hash_map<wstr, UserConnection*> LoggedInNameUserTable;
	typedef std::hash_map<VSocket*, UserConnection*, LEPRA_VOIDP_HASHER> SocketUserTable;
	typedef SocketSet SocketTimeoutTable;
	typedef std::hash_map<VSocket*, DropFilterCallback, LEPRA_VOIDP_HASHER> SocketReceiveFilterTable;
	typedef std::hash_set<UserAccount::AccountId> AccountIdSet;

	UserConnectionFactory* mUserConnectionFactory;
	LoginListener* mLoginListener;
	PendingSocketTable mPendingLoginTable;
	LoggedInIdUserTable mLoggedInIdUserTable;
	LoggedInNameUserTable mLoggedInNameUserTable;
	SocketUserTable mSocketUserTable;
	SocketTimeoutTable mSocketTimeoutTable;
	SocketReceiveFilterTable mSocketReceiveFilterTable;
	AccountIdSet mDropUserList;

	Timer mKeepaliveTimer;

	LOG_CLASS_DECLARE();
};



}
