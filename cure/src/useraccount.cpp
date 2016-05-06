
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/useraccount.h"
#include "../../lepra/include/packer.h"
#include "../../lepra/include/sha1.h"



namespace cure {



MangledPassword::MangledPassword() {
}

MangledPassword::MangledPassword(str& mangled_password) {
	MangleAndSet(mangled_password);
}

MangledPassword::MangledPassword(const MangledPassword& mangled_password) {
	*this = mangled_password;
}

MangledPassword::~MangledPassword() {
	Clear();
}

void MangledPassword::operator=(const MangledPassword& mangled_password) {
	mangled_password_ = mangled_password.mangled_password_;
}

void MangledPassword::Clear() {
	size_t original_length = mangled_password_.length();
	for (size_t x = 0; x < original_length; ++x) {
		mangled_password_[x] = 0;
	}
}

const std::string& MangledPassword::Get() const {
	return (mangled_password_);
}

void MangledPassword::MangleAndSet(str& password) {
	Clear();
	mangled_password_.resize(20);

	const size_t string_length = password.length();
	std::vector<uint8> data(string_length*4 + 32);
	uint8* raw_data = &data[0];
	const int raw_data_size = PackerUnicodeString::Pack(raw_data, password);
	uint8 hash_data[20];
	SHA1::Hash(raw_data, raw_data_size, hash_data);
	for (size_t x = 0; x < 20; ++x) {
		mangled_password_[x] = hash_data[x];
	}

	// Clear original password.
	for (size_t y = 0; y < string_length; ++y) {
		password[y] = '\0';
	}
	::memset(raw_data, 0, raw_data_size);
}

void MangledPassword::SetUnmodified(const std::string& mangled_password) {
	Clear();
	mangled_password_ = mangled_password;
}

bool MangledPassword::operator==(const MangledPassword& mangled_password) const {
	return (mangled_password_ == mangled_password.mangled_password_);
}



LoginId::LoginId() {
}

LoginId::LoginId(const str& user_name, const MangledPassword& mangled_password):
	user_name_(user_name),
	mangled_password_(mangled_password) {
}

LoginId::~LoginId() {
}

const str& LoginId::GetName() const {
	return (user_name_);
}

const MangledPassword& LoginId::GetMangledPassword() const {
	return (mangled_password_);
}

bool LoginId::operator==(const LoginId& user) const {
	return (user_name_ == user.GetName() &&
		mangled_password_ == user.mangled_password_);
}



UserAccount::UserAccount(const LoginId& login_id, AccountId id):
	LoginId(login_id),
	id_(id),
	status_(kStatusOk) {
}

/*UserAccount::UserAccount(const str& user_name, const MangledPassword& mangled_password):
	LoginId(user_name, mangled_password),
	status_(kStatusOk) {
}*/

UserAccount::~UserAccount() {
}

UserAccount::AccountId UserAccount::GetId() const {
	return (id_);
}

UserAccount::Availability UserAccount::GetStatus() const {
	return (status_);
}

void UserAccount::SetStatus(Availability status) {
	status_ = status;
}

bool UserAccount::AddAvatarId(const AvatarId& avatar_id) {
	return (avatar_id_set_.insert(avatar_id).second);
}

const UserAccount::AvatarIdSet& UserAccount::GetAvatarIdSet() const {
	return (avatar_id_set_);
}



}
