
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <string>
#include "../../lepra/include/string.h"
#include "cure.h"
//#include "../include/resourcemanager.h"



namespace cure {



class MangledPassword {
public:
	MangledPassword();
	MangledPassword(str& password);
	MangledPassword(const MangledPassword& password);
	~MangledPassword();
	void operator=(const MangledPassword& password);
	void Clear();

	const std::string& Get() const;
	void MangleAndSet(str& password);
	void SetUnmodified(const std::string& password);
	bool operator==(const MangledPassword& password) const;

private:
	std::string mangled_password_;
};



class LoginId {
public:
	LoginId();
	LoginId(const str& user_account_name, const MangledPassword& password);
	virtual ~LoginId();

	const str& GetName() const;
	const MangledPassword& GetMangledPassword() const;

	bool operator==(const LoginId& user_account) const;

private:
	str user_name_;
	MangledPassword mangled_password_;
};



class UserAccount: public LoginId {
public:
	typedef uint32 AccountId;
	typedef str AvatarId;
	typedef std::unordered_set<AvatarId> AvatarIdSet;
	enum Availability {
		kStatusOk		= 1,
		kStatusNotPresent	= 2,
		kStatusTemporaryBanned	= 3,
		kStatusBanned		= 4,
	};

	UserAccount(const LoginId& login_id, AccountId id);
	virtual ~UserAccount();

	AccountId GetId() const;
	Availability GetStatus() const;
	void SetStatus(Availability status);
	bool AddAvatarId(const AvatarId& avatar_id);
	const AvatarIdSet& GetAvatarIdSet() const;

private:
	AccountId id_;
	Availability status_;
	AvatarIdSet avatar_id_set_;
};



}
