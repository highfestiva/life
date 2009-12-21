
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games
 


#include "../Include/UserConnection.h"



namespace Cure
{



UserConnection::UserConnection():
	mName(),
	mAccountId(0xFFFFFFFF),
	mSocket(0)
{
}

UserConnection::~UserConnection()
{
	SetSocket(0);
	mAccountId = 0xFFFFFFFF;
}



const wstr& UserConnection::GetLoginName() const
{
	return (mName);
}

void UserConnection::SetLoginName(const wstr& pName)
{
	mName = pName;
}

UserAccount::AccountId UserConnection::GetAccountId() const
{
	return (mAccountId);
}

void UserConnection::SetAccountId(UserAccount::AccountId pAccountId)
{
	mAccountId = pAccountId;
}

GameSocket* UserConnection::GetSocket() const
{
	return (mSocket);
}

void UserConnection::SetSocket(GameSocket* pSocket)
{
	mSocket = pSocket;
}



UserConnectionFactory::UserConnectionFactory()
{
}

UserConnectionFactory::~UserConnectionFactory()
{
}

UserConnection* UserConnectionFactory::AllocateUserConnection()
{
	return new UserConnection;
}

void UserConnectionFactory::FreeUserConnection(UserConnection* pUserConnection)
{
	delete (pUserConnection);
}



}
