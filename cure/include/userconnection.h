
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/socket.h"
#include "../../lepra/include/string.h"
#include "networkagent.h"
#include "useraccount.h"



namespace cure {



class UserConnection {
public:
	UserConnection();
	virtual ~UserConnection();

	const str& GetLoginName() const;
	void SetLoginName(const str& name);
	UserAccount::AccountId GetAccountId() const;
	void SetAccountId(UserAccount::AccountId id);
	NetworkAgent::VSocket* GetSocket() const;
	void SetSocket(NetworkAgent::VSocket* socket);

private:
	str name_;
	UserAccount::AccountId account_id_;
	NetworkAgent::VSocket* socket_;
};



class UserConnectionFactory {
public:
	UserConnectionFactory();
	~UserConnectionFactory();
	UserConnection* AllocateUserConnection();
	void FreeUserConnection(UserConnection* user_connection);
};



}
