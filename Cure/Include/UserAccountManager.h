
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/IdManager.h"
#include "../Include/UserAccount.h"



namespace Cure
{



class UserAccountManager
{
public:
	UserAccountManager();
	virtual ~UserAccountManager();

	virtual bool AddUserAccount(const LoginId& pLoginId) = 0;
	virtual bool DeleteUserAccount(const str& pUserAccountName) = 0;
	virtual UserAccount::Availability GetUserAccountStatus(const LoginId& pLoginId, UserAccount::AccountId& pAccountId) = 0;
	virtual bool SetUserAccountStatus(const str& pUserAccountName, UserAccount::Availability pStatus) = 0;
	virtual bool GetUserAccountId(const str& pUserName, UserAccount::AccountId& pAccountId) = 0;
	virtual bool AddUserAvatarId(const str& pUserName, const UserAccount::AvatarId& pAvatarId) = 0;
	virtual const UserAccount::AvatarIdSet* GetUserAvatarIdSet(const str& pUserName) = 0;
};



class MemoryUserAccountManager: public UserAccountManager
{
public:
	MemoryUserAccountManager();
	virtual ~MemoryUserAccountManager();

	bool AddUserAccount(const LoginId& pLoginId);
	bool DeleteUserAccount(const str& pUserName);
	UserAccount::Availability GetUserAccountStatus(const LoginId& pLoginId, UserAccount::AccountId& pAccountId);
	bool SetUserAccountStatus(const str& pUserAccountName, UserAccount::Availability pStatus);
	bool GetUserAccountId(const str& pUserName, UserAccount::AccountId& pAccountId);
	bool AddUserAvatarId(const str& pUserName, const UserAccount::AvatarId& pAvatarId);
	const UserAccount::AvatarIdSet* GetUserAvatarIdSet(const str& pUserName);

	void RemoveAllUserAccounts();

private:
	typedef IdManager<UserAccount::AccountId> LoggedInUserIdManager;
	typedef HashTable<str, UserAccount*>  UserAccountTable;

	LoggedInUserIdManager mLoggedInUserIdManager;
	UserAccountTable mUserAccountTable;
};



}
