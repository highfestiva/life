
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/useraccountmanager.h"



namespace cure {



UserAccountManager::UserAccountManager() {
}

UserAccountManager::~UserAccountManager() {
}



MemoryUserAccountManager::MemoryUserAccountManager():
	logged_in_user_id_manager_(1000, 0x7FFFFFFF-1, 0xFFFFFFFF) {
}

MemoryUserAccountManager::~MemoryUserAccountManager() {
	RemoveAllUserAccounts();
}

bool MemoryUserAccountManager::AddUserAccount(const LoginId& login_id) {
	bool ok = (user_account_table_.FindObject(login_id.GetName()) == 0);
	if (ok) {
		UserAccount::AccountId id = logged_in_user_id_manager_.GetFreeId();
		UserAccount* user_account = new UserAccount(login_id, id);
		user_account_table_.Insert(login_id.GetName(), user_account);
	}
	return (ok);
}

bool MemoryUserAccountManager::DeleteUserAccount(const str& user_name) {
	UserAccount* user_account = user_account_table_.FindObject(user_name);
	bool ok = (user_account != 0);
	if (ok) {
		user_account_table_.Remove(user_name);
		logged_in_user_id_manager_.RecycleId(user_account->GetId());
		delete (user_account);
	}
	return (ok);
}

UserAccount::Availability MemoryUserAccountManager::GetUserAccountStatus(const LoginId& login_id, UserAccount::AccountId& account_id) {
	UserAccount::Availability _status = UserAccount::kStatusNotPresent;
	UserAccount* user_account = user_account_table_.FindObject(login_id.GetName());
	if (user_account) {
		if (*user_account == login_id) {
			_status = user_account->GetStatus();
			account_id = user_account->GetId();
		}
	}
	return (_status);
}

bool MemoryUserAccountManager::SetUserAccountStatus(const str& user_account_name, UserAccount::Availability status) {
	UserAccount* user_account = user_account_table_.FindObject(user_account_name);
	bool ok = (user_account != 0);
	if (ok) {
		user_account->SetStatus(status);
	}
	return (ok);
}

bool MemoryUserAccountManager::GetUserAccountId(const str& user_name, UserAccount::AccountId& account_id) {
	UserAccount* user_account = user_account_table_.FindObject(user_name);
	bool ok = (user_account != 0);
	if (ok) {
		account_id = user_account->GetId();
	}
	return (ok);
}

bool MemoryUserAccountManager::AddUserAvatarId(const str& user_name, const UserAccount::AvatarId& avatar_id) {
	UserAccount* user_account = user_account_table_.FindObject(user_name);
	bool ok = (user_account != 0);
	if (ok) {
		ok = user_account->AddAvatarId(avatar_id);
	}
	return (ok);
}

const UserAccount::AvatarIdSet* MemoryUserAccountManager::GetUserAvatarIdSet(const str& user_name) {
	const UserAccount::AvatarIdSet* avatar_id_set = 0;
	UserAccount* user_account = user_account_table_.FindObject(user_name);
	if (user_account) {
		avatar_id_set = &user_account->GetAvatarIdSet();
	}
	return (avatar_id_set);
}

void MemoryUserAccountManager::RemoveAllUserAccounts() {
	UserAccountTable::Iterator x = user_account_table_.First();
	for (; x != user_account_table_.End(); ++x) {
		UserAccount* user_account = x.GetObject();
		delete (user_account);
	}
	user_account_table_.RemoveAll();
}



}
