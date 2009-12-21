
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games
 


#include "../Include/UserAccount.h"
#include "../../Lepra/Include/Packer.h"
#include "../../Lepra/Include/SHA1.h"



namespace Cure
{



MangledPassword::MangledPassword()
{
}

MangledPassword::MangledPassword(wstr& pMangledPassword)
{
	MangleAndSet(pMangledPassword);
}

MangledPassword::MangledPassword(const MangledPassword& pMangledPassword)
{
	*this = pMangledPassword;
}

MangledPassword::~MangledPassword()
{
	Clear();
}

void MangledPassword::operator=(const MangledPassword& pMangledPassword)
{
	mMangledPassword = pMangledPassword.mMangledPassword;
}

void MangledPassword::Clear()
{
	size_t lOriginalLength = mMangledPassword.length();
	for (size_t x = 0; x < lOriginalLength; ++x)
	{
		mMangledPassword[x] = 0;
	}
}

const std::string& MangledPassword::Get() const
{
	return (mMangledPassword);
}

void MangledPassword::MangleAndSet(wstr& pPassword)
{
	Clear();
	mMangledPassword.resize(20);

	const size_t lStringLength = pPassword.length();
	std::vector<uint8> lData(lStringLength*4 + 32);
	uint8* lRawData = &lData[0];
	const int lRawDataSize = PackerUnicodeString::Pack(lRawData, pPassword);
	uint8 lHashData[20];
	SHA1::Hash(lRawData, lRawDataSize, lHashData);
	for (size_t x = 0; x < 20; ++x)
	{
		mMangledPassword[x] = lHashData[x];
	}

	// Clear original password.
	for (size_t y = 0; y < lStringLength; ++y)
	{
		pPassword[y] = _T('\0');
	}
	::memset(lRawData, 0, lRawDataSize);
}

void MangledPassword::SetUnmodified(const std::string& pMangledPassword)
{
	Clear();
	mMangledPassword = pMangledPassword;
}

bool MangledPassword::operator==(const MangledPassword& pMangledPassword) const
{
	return (mMangledPassword == pMangledPassword.mMangledPassword);
}



LoginId::LoginId()
{
}

LoginId::LoginId(const wstr& pUserName, const MangledPassword& pMangledPassword):
	mUserName(pUserName),
	mMangledPassword(pMangledPassword)
{
}

LoginId::~LoginId()
{
}

const wstr& LoginId::GetName() const
{
	return (mUserName);
}

const MangledPassword& LoginId::GetMangledPassword() const
{
	return (mMangledPassword);
}

bool LoginId::operator==(const LoginId& pUser) const
{
	return (mUserName == pUser.GetName() &&
		mMangledPassword == pUser.mMangledPassword);
}



UserAccount::UserAccount(const LoginId& pLoginId, AccountId pId):
	LoginId(pLoginId),
	mId(pId),
	mStatus(STATUS_OK)
{
}

/*UserAccount::UserAccount(const wstr& pUserName, const MangledPassword& pMangledPassword):
	LoginId(pUserName, pMangledPassword),
	mStatus(STATUS_OK)
{
}*/

UserAccount::~UserAccount()
{
}

UserAccount::AccountId UserAccount::GetId() const
{
	return (mId);
}

UserAccount::Availability UserAccount::GetStatus() const
{
	return (mStatus);
}

void UserAccount::SetStatus(Availability pStatus)
{
	mStatus = pStatus;
}

bool UserAccount::AddAvatarId(const AvatarId& pAvatarId)
{
	return (mAvatarIdSet.insert(pAvatarId).second);
}

const UserAccount::AvatarIdSet& UserAccount::GetAvatarIdSet() const
{
	return (mAvatarIdSet);
}



}
