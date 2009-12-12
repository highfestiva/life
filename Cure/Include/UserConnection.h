
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

	const Lepra::UnicodeString& GetLoginName() const;
	void SetLoginName(const Lepra::UnicodeString& pName);
	UserAccount::AccountId GetAccountId() const;
	void SetAccountId(UserAccount::AccountId pId);
	Lepra::GameSocket* GetSocket() const;
	void SetSocket(Lepra::GameSocket* pSocket);

private:
	Lepra::UnicodeString mName;
	UserAccount::AccountId mAccountId;
	Lepra::GameSocket* mSocket;
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
