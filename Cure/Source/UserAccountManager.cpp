
// Author: Jonas Bystr�m
// Copyright (c) 2002-2008, Righteous Games
 


#include "../Include/UserAccountManager.h"



namespace Cure
{



UserAccountManager::UserAccountManager()
{
}

UserAccountManager::~UserAccountManager()
{
}



MemoryUserAccountManager::MemoryUserAccountManager():
	mLoggedInUserIdManager(1000, 0x7FFFFFFF-1, 0xFFFFFFFF)
{
}

MemoryUserAccountManager::~MemoryUserAccountManager()
{
}

bool MemoryUserAccountManager::AddUserAccount(const LoginId& pLoginId)
{
	bool lOk = (mUserAccountTable.FindObject(pLoginId.GetName()) == 0);
	if (lOk)
	{
		UserAccount::AccountId lId = mLoggedInUserIdManager.GetFreeId();
		UserAccount* lUserAccount = new UserAccount(pLoginId, lId);
		mUserAccountTable.Insert(pLoginId.GetName(), lUserAccount);
	}
	return (lOk);
}

bool MemoryUserAccountManager::DeleteUserAccount(const Lepra::UnicodeString& pUserName)
{
	UserAccount* lUserAccount = mUserAccountTable.FindObject(pUserName);
	bool lOk = (lUserAccount != 0);
	if (lOk)
	{
		mUserAccountTable.Remove(pUserName);
		mLoggedInUserIdManager.RecycleId(lUserAccount->GetId());
		delete (lUserAccount);
	}
	return (lOk);
}

UserAccount::Availability MemoryUserAccountManager::GetUserAccountStatus(const LoginId& pLoginId, UserAccount::AccountId& pAccountId)
{
	UserAccount::Availability lStatus = UserAccount::STATUS_NOT_PRESENT;
	UserAccount* lUserAccount = mUserAccountTable.FindObject(pLoginId.GetName());
	if (lUserAccount)
	{
		if (*lUserAccount == pLoginId)
		{
			lStatus = lUserAccount->GetStatus();
			pAccountId = lUserAccount->GetId();
		}
	}
	return (lStatus);
}

bool MemoryUserAccountManager::SetUserAccountStatus(const Lepra::UnicodeString& pUserAccountName, UserAccount::Availability pStatus)
{
	UserAccount* lUserAccount = mUserAccountTable.FindObject(pUserAccountName);
	bool lOk = (lUserAccount != 0);
	if (lOk)
	{
		lUserAccount->SetStatus(pStatus);
	}
	return (lOk);
}

bool MemoryUserAccountManager::GetUserAccountId(const Lepra::UnicodeString& pUserName, UserAccount::AccountId& pAccountId)
{
	UserAccount* lUserAccount = mUserAccountTable.FindObject(pUserName);
	bool lOk = (lUserAccount != 0);
	if (lOk)
	{
		pAccountId = lUserAccount->GetId();
	}
	return (lOk);
}

bool MemoryUserAccountManager::AddUserAvatarId(const Lepra::UnicodeString& pUserName, const UserAccount::AvatarId& pAvatarId)
{
	UserAccount* lUserAccount = mUserAccountTable.FindObject(pUserName);
	bool lOk = (lUserAccount != 0);
	if (lOk)
	{
		lOk = lUserAccount->AddAvatarId(pAvatarId);
	}
	return (lOk);
}

const UserAccount::AvatarIdSet* MemoryUserAccountManager::GetUserAvatarIdSet(const Lepra::UnicodeString& pUserName)
{
	const UserAccount::AvatarIdSet* lAvatarIdSet = 0;
	UserAccount* lUserAccount = mUserAccountTable.FindObject(pUserName);
	if (lUserAccount)
	{
		lAvatarIdSet = &lUserAccount->GetAvatarIdSet();
	}
	return (lAvatarIdSet);
}

void MemoryUserAccountManager::RemoveAllUserAccounts()
{
	UserAccountTable::Iterator x = mUserAccountTable.First();
	for (; x != mUserAccountTable.End(); ++x)
	{
		UserAccount* lUserAccount = x.GetObject();
		delete (lUserAccount);
	}
	mUserAccountTable.RemoveAll();
}



}
