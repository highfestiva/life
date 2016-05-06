
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/idmanager.h"
#include "../include/useraccount.h"



namespace cure {



class UserAccountManager {
public:
	UserAccountManager();
	virtual ~UserAccountManager();

	virtual bool AddUserAccount(const LoginId& login_id) = 0;
	virtual bool DeleteUserAccount(const str& user_account_name) = 0;
	virtual UserAccount::Availability GetUserAccountStatus(const LoginId& login_id, UserAccount::AccountId& account_id) = 0;
	virtual bool SetUserAccountStatus(const str& user_account_name, UserAccount::Availability status) = 0;
	virtual bool GetUserAccountId(const str& user_name, UserAccount::AccountId& account_id) = 0;
	virtual bool AddUserAvatarId(const str& user_name, const UserAccount::AvatarId& avatar_id) = 0;
	virtual const UserAccount::AvatarIdSet* GetUserAvatarIdSet(const str& user_name) = 0;
};



class MemoryUserAccountManager: public UserAccountManager {
public:
	MemoryUserAccountManager();
	virtual ~MemoryUserAccountManager();

	bool AddUserAccount(const LoginId& login_id);
	bool DeleteUserAccount(const str& user_name);
	UserAccount::Availability GetUserAccountStatus(const LoginId& login_id, UserAccount::AccountId& account_id);
	bool SetUserAccountStatus(const str& user_account_name, UserAccount::Availability status);
	bool GetUserAccountId(const str& user_name, UserAccount::AccountId& account_id);
	bool AddUserAvatarId(const str& user_name, const UserAccount::AvatarId& avatar_id);
	const UserAccount::AvatarIdSet* GetUserAvatarIdSet(const str& user_name);

	void RemoveAllUserAccounts();

private:
	typedef IdManager<UserAccount::AccountId> LoggedInUserIdManager;
	typedef HashTable<str, UserAccount*>  UserAccountTable;

	LoggedInUserIdManager logged_in_user_id_manager_;
	UserAccountTable user_account_table_;
};



}
