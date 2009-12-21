
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Socket.h"
#include "UserAccount.h"



namespace Cure
{



class UserConnection
{
public:
	UserConnection();
	virtual ~UserConnection();

	const wstr& GetLoginName() const;
	void SetLoginName(const wstr& pName);
	UserAccount::AccountId GetAccountId() const;
	void SetAccountId(UserAccount::AccountId pId);
	GameSocket* GetSocket() const;
	void SetSocket(GameSocket* pSocket);

private:
	wstr mName;
	UserAccount::AccountId mAccountId;
	GameSocket* mSocket;
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
