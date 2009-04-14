
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



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
	virtual bool DeleteUserAccount(const Lepra::UnicodeString& pUserAccountName) = 0;
	virtual UserAccount::Availability GetUserAccountStatus(const LoginId& pLoginId, UserAccount::AccountId& pAccountId) = 0;
	virtual bool SetUserAccountStatus(const Lepra::UnicodeString& pUserAccountName, UserAccount::Availability pStatus) = 0;
	virtual bool GetUserAccountId(const Lepra::UnicodeString& pUserName, UserAccount::AccountId& pAccountId) = 0;
	virtual bool AddUserAvatarId(const Lepra::UnicodeString& pUserName, const UserAccount::AvatarId& pAvatarId) = 0;
	virtual const UserAccount::AvatarIdSet* GetUserAvatarIdSet(const Lepra::UnicodeString& pUserName) = 0;
};



class MemoryUserAccountManager: public UserAccountManager
{
public:
	MemoryUserAccountManager();
	virtual ~MemoryUserAccountManager();

	bool AddUserAccount(const LoginId& pLoginId);
	bool DeleteUserAccount(const Lepra::UnicodeString& pUserName);
	UserAccount::Availability GetUserAccountStatus(const LoginId& pLoginId, UserAccount::AccountId& pAccountId);
	bool SetUserAccountStatus(const Lepra::UnicodeString& pUserAccountName, UserAccount::Availability pStatus);
	bool GetUserAccountId(const Lepra::UnicodeString& pUserName, UserAccount::AccountId& pAccountId);
	bool AddUserAvatarId(const Lepra::UnicodeString& pUserName, const UserAccount::AvatarId& pAvatarId);
	const UserAccount::AvatarIdSet* GetUserAvatarIdSet(const Lepra::UnicodeString& pUserName);

	void RemoveAllUserAccounts();

private:
	typedef Lepra::IdManager<UserAccount::AccountId> LoggedInUserIdManager;
	typedef Lepra::HashTable<Lepra::UnicodeString, UserAccount*>  UserAccountTable;

	LoggedInUserIdManager mLoggedInUserIdManager;
	UserAccountTable mUserAccountTable;
};



}
