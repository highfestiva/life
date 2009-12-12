
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <string>
#include "../../Lepra/Include/String.h"
#include "../Include/ResourceManager.h"



namespace Cure
{



class MangledPassword
{
public:
	MangledPassword();
	MangledPassword(Lepra::UnicodeString& pPassword);
	MangledPassword(const MangledPassword& pPassword);
	~MangledPassword();
	void operator=(const MangledPassword& pPassword);
	void Clear();

	const std::string& Get() const;
	void MangleAndSet(Lepra::UnicodeString& pPassword);
	void SetUnmodified(const std::string& pPassword);
	bool operator==(const MangledPassword& pPassword) const;

private:
	std::string mMangledPassword;
};



class LoginId
{
public:
	LoginId();
	LoginId(const Lepra::UnicodeString& pUserAccountName, const MangledPassword& pPassword);
	virtual ~LoginId();

	const Lepra::UnicodeString& GetName() const;
	const MangledPassword& GetMangledPassword() const;

	bool operator==(const LoginId& pUserAccount) const;

private:
	Lepra::UnicodeString mUserName;
	MangledPassword mMangledPassword;
};



class UserAccount: public LoginId
{
public:
	typedef Lepra::uint32 AccountId;
	typedef Lepra::String AvatarId;
	typedef std::set<AvatarId> AvatarIdSet;
	enum Availability
	{
		STATUS_OK		= 1,
		STATUS_NOT_PRESENT	= 2,
		STATUS_TEMPORARY_BANNED	= 3,
		STATUS_BANNED		= 4,
	};

	UserAccount(const LoginId& pLoginId, AccountId pId);
	virtual ~UserAccount();

	AccountId GetId() const;
	Availability GetStatus() const;
	void SetStatus(Availability pStatus);
	bool AddAvatarId(const AvatarId& pAvatarId);
	const AvatarIdSet& GetAvatarIdSet() const;

private:
	AccountId mId;
	Availability mStatus;
	AvatarIdSet mAvatarIdSet;
};



}
