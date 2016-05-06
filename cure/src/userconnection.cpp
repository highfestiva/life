
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/userconnection.h"



namespace cure {



UserConnection::UserConnection():
	name_(),
	account_id_(0xFFFFFFFF),
	socket_(0) {
}

UserConnection::~UserConnection() {
	SetSocket(0);
	account_id_ = 0xFFFFFFFF;
}



const str& UserConnection::GetLoginName() const {
	return (name_);
}

void UserConnection::SetLoginName(const str& name) {
	name_ = name;
}

UserAccount::AccountId UserConnection::GetAccountId() const {
	return (account_id_);
}

void UserConnection::SetAccountId(UserAccount::AccountId account_id) {
	account_id_ = account_id;
}

NetworkAgent::VSocket* UserConnection::GetSocket() const {
	return (socket_);
}

void UserConnection::SetSocket(NetworkAgent::VSocket* socket) {
	socket_ = socket;
}



UserConnectionFactory::UserConnectionFactory() {
}

UserConnectionFactory::~UserConnectionFactory() {
}

UserConnection* UserConnectionFactory::AllocateUserConnection() {
	return new UserConnection;
}

void UserConnectionFactory::FreeUserConnection(UserConnection* user_connection) {
	delete (user_connection);
}



}
