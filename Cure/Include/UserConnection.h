
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/Socket.h"
#include "../../Lepra/Include/String.h"
#include "NetworkAgent.h"
#include "UserAccount.h"



namespace Cure
{



class UserConnection
{
public:
	UserConnection();
	virtual ~UserConnection();

	const str& GetLoginName() const;
	void SetLoginName(const str& pName);
	UserAccount::AccountId GetAccountId() const;
	void SetAccountId(UserAccount::AccountId pId);
	NetworkAgent::VSocket* GetSocket() const;
	void SetSocket(NetworkAgent::VSocket* pSocket);

private:
	str mName;
	UserAccount::AccountId mAccountId;
	NetworkAgent::VSocket* mSocket;
};



class UserConnectionFactory
{
public:
	UserConnectionFactory();
	~UserConnectionFactory();
	UserConnection* AllocateUserConnection();
	void FreeUserConnection(UserConnection* pUserConnection);
};



}
