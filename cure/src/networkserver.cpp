
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/networkserver.h"
#include "../../lepra/include/endian.h"
#include "../../lepra/include/hashutil.h"
#include "../../lepra/include/log.h"
#include "../include/contextobject.h"
#include "../include/packet.h"
#include "../include/runtimevariable.h"
#include "../include/userconnection.h"



namespace cure {



NetworkServer::LoginListener::LoginListener() {
}

NetworkServer::LoginListener::~LoginListener() {
}



NetworkServer::NetworkServer(RuntimeVariableScope* variable_scope, LoginListener* login_listener):
	NetworkAgent(variable_scope),
	user_connection_factory_(new UserConnectionFactory()),
	login_listener_(login_listener) {
}

NetworkServer::~NetworkServer() {
	Stop();

	login_listener_ = 0;	// TRICKY: caller owns login listener.
	delete (user_connection_factory_);
	user_connection_factory_ = 0;
}

bool NetworkServer::Start(const str& host_address) {
	bool ok = true;
	SocketAddress address;
	if (ok) {
		ok = address.Resolve(host_address);
	}
	if (ok) {
		log_.Info("Server listening to address " + address.GetAsString() + ".");
		ScopeLock lock(&lock_);
		SetMuxSocket(new MuxSocket("Srv ", address, true, 100, 1000));
		ok = mux_socket_->IsOpen();
	}
	return (ok);
}

void NetworkServer::Stop() {
	ScopeLock lock(&lock_);
	while (!logged_in_id_user_table_.empty()) {
		LoggedInIdUserTable::iterator x = logged_in_id_user_table_.begin();
		UserConnection* _user = x->second;
		RemoveUser(_user->GetAccountId(), true);
	}
	while (!socket_receive_filter_table_.empty()) {
		VSocket* _socket = socket_receive_filter_table_.begin()->first;
		DropSocket(_socket);
	}
	Parent::Stop();
}



void NetworkServer::Disconnect(UserAccount::AccountId account_id, const str& reason, bool send_disconnect) {
	UserConnection* _user = GetUser(account_id);
	if (_user) {
		if (send_disconnect) {
			cure::Packet* _packet = GetPacketFactory()->Allocate();
			Parent::SendStatusMessage(_user->GetSocket(), 0, cure::kRemoteNoConnection,
				cure::MessageStatus::kInfoLogin, reason, _packet);
			GetPacketFactory()->Release(_packet);
		} else if (_user->GetSocket()) {
			_user->GetSocket()->ClearOutputData();
		}
		RemoveUser(account_id, true);
	}
}



bool NetworkServer::PlaceInSendBuffer(bool safe, Packet* packet, UserAccount::AccountId account_id) {
	bool ok = false;
	bool remove_user = false;
	UserConnection* _user = GetUser(account_id);
	if (_user) {
		ok = Parent::PlaceInSendBuffer(safe, _user->GetSocket(), packet);
		remove_user = !ok;
	}
	if (remove_user) {
		RemoveUser(account_id, true);
	}
	return (ok);
}

bool NetworkServer::SendAll() {
	bool all_sends_ok = true;
	VSocket* _socket;
	while ((_socket = mux_socket_->PopSenderSocket()) != 0) {
		int send_count = _socket->SendBuffer();
		bool ok = (send_count > 0);
		if (!ok) {
			all_sends_ok = false;
			if (send_count == 0) {
				log_.Warning("Disconnecting socket since no data available for sending, though listed as such.");
			} else {
				log_.Warning("Disconnecting socket since send failed.");
			}

			// Socket disconnected: drop user or socket.
			UserConnection* _user;
			{
				_user = HashUtil::FindMapObject(socket_user_table_, _socket);
			}
			if (_user) {
				RemoveUser(_user->GetAccountId(), true);
			} else {
				// A pending login or similar.
				DropSocket(_socket);
			}
		}
	}
	return (all_sends_ok);
}

NetworkServer::ReceiveStatus NetworkServer::ReceiveFirstPacket(Packet* packet, UserAccount::AccountId& account_id) {
	if (!mux_socket_) {
		return kReceiveConnectionBroken;
	}

	PollAccept();
	KillDeadSockets();

	ReceiveStatus _status = kReceiveNoData;
	VSocket* _socket;
	while ((_socket = mux_socket_->PopReceiverSocket()) != 0) {
		if (socket_receive_filter_table_.find(_socket) != socket_receive_filter_table_.end()) {
			continue;
		}
		int _data_length = _socket->Receive(packet->GetWriteBuffer(), packet->GetBufferSize());
		UserConnection* _user;
		{
			_user = HashUtil::FindMapObject(socket_user_table_, _socket);
			if (_user) {
				account_id = _user->GetAccountId();
			}
		}
		if (_data_length == 0) {
			log_volatile(log_.Debug("Got zero-length packet from "+
				_socket->GetTargetAddress().GetAsString()));
			_status = kReceiveNoData;
			// TRICKY: no break here, but in sibling scopes.
		} else if (_data_length > 0) {
			packet->SetPacketSize(_data_length);
			if (packet->Parse() == Packet::kParseOk) {
				_status = kReceiveOk;

				// Good boy, you will not be kicked this run!
				socket_timeout_table_.erase(_socket);
			} else {
				_status = kReceiveParseError;
				log_.Warningf("Got bad data from %s on %s.",
					_user? ("logged in user "+_user->GetLoginName()).c_str() :
						"not logged in user",
					_socket->GetTargetAddress().GetAsString().c_str());
				// TODO: take action on bad network data?
			}
			_socket->TryAddReceiverSocket();
			// TRICKY: break here, but not in all sibling scopes.
			if (_user) {
				break;	// We have parsed towards a logged-in user. Done.
			}
		} else {
			_status = kReceiveConnectionBroken;
			log_volatile(log_.Debug("Got broken pipe from "+
				_socket->GetTargetAddress().GetAsString()));
			// TRICKY: break here, but not in all sibling scopes.
			if (_user) {
				break;	// We have broken pipe towards a logged-in user. Done.
			}
		}
		if (!_user) {
			TryLogin(_socket, packet, _data_length);
			packet->Release();
			_status = kReceiveNoData;
		}
	}
	if (_status == kReceiveConnectionBroken) {
		RemoveUser(account_id, true);
	}
	return (_status);
}

NetworkServer::ReceiveStatus NetworkServer::ReceiveMore(UserAccount::AccountId account_id, Packet* packet) {
	if (!mux_socket_) {
		return kReceiveConnectionBroken;
	}

	ScopeLock lock(&lock_);
	UserConnection* _user = HashUtil::FindMapObject(logged_in_id_user_table_, account_id);
	if (!_user) {
		return kReceiveConnectionBroken;
	}
	VSocket* _socket = _user->GetSocket();
	if (!_socket) {
		return kReceiveConnectionBroken;
	}

	ReceiveStatus result = kReceiveConnectionBroken;
	const int _data_length = _socket->Receive(true, packet->GetWriteBuffer() + packet->GetPacketSize(),
		packet->GetBufferSize() - packet->GetPacketSize());
	if (_data_length == 0) {
		result = kReceiveNoData;
	} else if (_data_length > 0) {
		packet->SetPacketSize(packet->GetPacketSize() + _data_length);
		result = kReceiveOk;
	}
	return result;
}



void NetworkServer::PollAccept() {
	if (pending_login_table_.size() < 1000) {
		VSocket* _socket = mux_socket_->PollAccept();
		// TODO: add banning techniques to avoid DoS attacks.
		if (_socket) {
			pending_login_table_.insert(_socket);
		}
	}
}

void NetworkServer::TryLogin(VSocket* socket, Packet* packet, int data_length) {
	{
		PendingSocketTable::iterator x = pending_login_table_.find(socket);
		if (x == pending_login_table_.end()) {
			// If user hasn't connected properly, we just cut the line.
			DropSocket(socket);
			return;	// TRICKY: returning here simplifies.
		}
		pending_login_table_.erase(x);
	}

	if (data_length >= 1+4) {
		if (packet->GetMessageCount() == 1) {
			if (packet->GetMessageAt(0)->GetType() == kMessageTypeLoginRequest) {
				ManageLogin(socket, packet);
			} else {
				// This login packet was shit. Ignore it.
				log_.Warning("Yucky hack packet from "+
					socket->GetTargetAddress().GetAsString()+".");
				DropSocket(socket);
			}
		} else {
			// Too many login packets. Throo avaj.
			log_.Warning("Too many (login?) packets or crappy data from "+
				socket->GetTargetAddress().GetAsString()+".");
			DropSocket(socket);
		}
	} else if (data_length != 0) {
		log_.Error("Login connection broken to "+
			socket->GetTargetAddress().GetAsString()+".");
		DropSocket(socket);
	}
	// TODO: add timeout for sockets in pending state.
}

RemoteStatus NetworkServer::ManageLogin(VSocket* socket, Packet* packet) {
	Message* _message = packet->GetMessageAt(0);
	MessageLoginRequest* login_message = (MessageLoginRequest*)_message;
	str _login_name;
	login_message->GetLoginName(_login_name);
	UserAccount::AccountId _account_id;
	RemoteStatus _status = QueryLogin(_login_name, login_message, _account_id);
	switch (_status) {
		case kRemoteOk: {
			log_.Info("Logging in user "+_login_name+".");
			Login(_login_name, _account_id, socket, packet);
		} break;
		case kRemoteLoginAlready: {
			log_.Warning("User "+_login_name+" already logged in.");
			Parent::SendStatusMessage(socket, 0, _status, cure::MessageStatus::kInfoLogin,
				"You have already been logged in.", packet);
			DropSocket(socket);
		} break;
		case kRemoteLoginErronousData: {
			log_.Warning("User "+_login_name+" attempted with wrong username or password.");
			Parent::SendStatusMessage(socket, 0, _status, cure::MessageStatus::kInfoLogin,
				"Wrong username or password. Try again.", packet);
			DropSocket(socket);
		} break;
		case kRemoteLoginBan: {
			log_.Warning("User "+_login_name+" tried logging in, but was banned.");
			Parent::SendStatusMessage(socket, 0, _status, cure::MessageStatus::kInfoLogin,
				"Sorry, you are banned. Try again later.", packet);
			DropSocket(socket);
		} break;
		case kRemoteNoConnection:	// TODO: check me out!
		case kRemoteUnknown: {
			log_.Error("An unknown error occurred when user "+_login_name+" tried logging in.");
			Parent::SendStatusMessage(socket, 0, _status, cure::MessageStatus::kInfoLogin,
				"Unknown login error, please contact support.", packet);
			DropSocket(socket);
		} break;
	}
	return (_status);
}

RemoteStatus NetworkServer::QueryLogin(const str& login_name, MessageLoginRequest* login_request, UserAccount::AccountId& account_id) {
	RemoteStatus login_result = kRemoteUnknown;

	UserConnection* account = HashUtil::FindMapObject(logged_in_name_user_table_, login_name);
	if (account == 0) {
		MangledPassword mangled_password = login_request->GetPassword();
		LoginId login_id(login_name, mangled_password);
		UserAccount::Availability _status = login_listener_->QueryLogin(login_id, account_id);
		switch (_status) {
			case UserAccount::kStatusOk: {
				login_result = kRemoteOk;
			} break;
			case UserAccount::kStatusNotPresent: {
				// Wrong username or password.
				login_result = kRemoteLoginErronousData;
			} break;
			case UserAccount::kStatusBanned:
			case UserAccount::kStatusTemporaryBanned: {
				// Banned for now or forever.
				login_result = kRemoteLoginBan;
			} break;
		}
	} else {
		// User already logged in.
		login_result = kRemoteLoginAlready;
	}

	return (login_result);
}

void NetworkServer::Login(const str& login_name, UserAccount::AccountId account_id, VSocket* socket, Packet* packet) {
	UserConnection* _user = user_connection_factory_->AllocateUserConnection();
	_user->SetLoginName(login_name);
	_user->SetAccountId(account_id);
	_user->SetSocket(socket);

	AddUser(_user, account_id);

	log_.Infof("Sending login OK with account ID %i", account_id);
	SendStatusMessage(account_id, account_id, kRemoteOk, cure::MessageStatus::kInfoLogin, "Welcome.", packet);

	login_listener_->OnLogin(_user);
}

void NetworkServer::AddUser(UserConnection* user, UserAccount::AccountId& account_id) {
	ScopeLock lock(&lock_);
	logged_in_id_user_table_.insert(LoggedInIdUserTable::value_type(account_id, user));
	logged_in_name_user_table_.insert(LoggedInNameUserTable::value_type(user->GetLoginName(), user));
	socket_user_table_.insert(SocketUserTable::value_type(user->GetSocket(), user));
}

bool NetworkServer::RemoveUser(UserAccount::AccountId account_id, bool destroy) {
	UserConnection* _user = 0;
	{
		ScopeLock lock(&lock_);
		LoggedInIdUserTable::iterator x = logged_in_id_user_table_.find(account_id);
		if (x != logged_in_id_user_table_.end()) {
			_user = x->second;
			if (destroy) {
				logged_in_id_user_table_.erase(x);
				logged_in_name_user_table_.erase(_user->GetLoginName());
			}
			socket_user_table_.erase(_user->GetSocket());
			socket_timeout_table_.erase(_user->GetSocket());
		}
	}
	if (_user) {
		VSocket* _socket = _user->GetSocket();
		if (_socket) {
			_user->SetSocket(0);
			DropSocket(_socket);
		}
		if (destroy) {
			login_listener_->OnLogout(_user);
			user_connection_factory_->FreeUserConnection(_user);
		}
	}
	return (_user != 0);
}

void NetworkServer::KillDeadSockets() {
	{
		ScopeLock lock(&lock_);
		AccountIdSet::iterator x = drop_user_list_.begin();
		for (; x != drop_user_list_.end(); ++x) {
			UserAccount::AccountId user_account_id = *x;
			log_.Infof("Kicking user ID %i in drop zone silently.", user_account_id);
			Disconnect(user_account_id, EmptyString, false);
		}
		drop_user_list_.clear();
	}

	double kill_interval;
	v_get(kill_interval, =, variable_scope_, kRtvarNetworkKeepaliveKillinterval, 20.0);
	keepalive_timer_.UpdateTimer();
	if (keepalive_timer_.GetTimeDiff() >= kill_interval) {
		// Reset the keepalive timer.
		keepalive_timer_.ClearTimeDiff();

		// Kill all old and dead connections.
		while (!socket_timeout_table_.empty()) {
			VSocket* _socket = *socket_timeout_table_.begin();
			SocketUserTable::iterator y = socket_user_table_.find(_socket);
			if (y != socket_user_table_.end()) {
				UserConnection* _user = y->second;
				UserAccount::AccountId user_account_id = _user->GetAccountId();
				log_.Infof("Dropping user %s (ID %i) due to network keepalive timeout.",
					_user->GetLoginName().c_str(), user_account_id);
				Disconnect(user_account_id, "Network timeout", true);
			} else {
				// A not-logged-in pending socket. Probably just a spoofer or similar. Close silently.
				log_.Infof("Dropping connected, but not logged in socket on %s due to network keepalive timeout.",
					_socket->GetTargetAddress().GetAsString().c_str());
				DropSocket(_socket);
			}
		}

		// Put all current connections in the "old and dead" bin. If they won't talk within the given
		// keepalive timeout, we kill them off.
		// First copy all pending connections.
		socket_timeout_table_.clear();
		socket_timeout_table_.insert(pending_login_table_.begin(), pending_login_table_.end());
		// Then extend with established user connections.
		SocketUserTable::iterator y = socket_user_table_.begin();
		for (; y != socket_user_table_.end(); ++y) {
			socket_timeout_table_.insert(y->second->GetSocket());
		}
	}
}

void NetworkServer::DropSocket(VSocket* socket) {
	ScopeLock lock(&lock_);
	if (socket) {
		socket->SendBuffer();
		socket_timeout_table_.erase(socket);
		pending_login_table_.erase(socket);
	}
	SocketReceiveFilterTable::iterator x = socket_receive_filter_table_.find(socket);
	if (x != socket_receive_filter_table_.end()) {
		x->second(x->first);
		socket_receive_filter_table_.erase(x);
	}
	if (socket) {
		mux_socket_->CloseSocket(socket);
	}
}

UserConnection* NetworkServer::GetUser(UserAccount::AccountId account_id) {
	ScopeLock lock(&lock_);
	UserConnection* _user = HashUtil::FindMapObject(logged_in_id_user_table_, account_id);
	return (_user);
}



bool NetworkServer::SendStatusMessage(UserAccount::AccountId account_id, int32 integer, RemoteStatus status,
	MessageStatus::InfoType info_type, str message, Packet* packet) {
	packet->Release();
	MessageStatus* _status = (MessageStatus*)packet_factory_->GetMessageFactory()->Allocate(kMessageTypeStatus);
	packet->AddMessage(_status);
	_status->Store(packet, status, info_type, integer, message);
	bool ok = PlaceInSendBuffer(true, packet, account_id);
	return (ok);
}



/*void NetworkServer::OnCloseSocket(VSocket* socket) {
	ScopeLock lock(&lock_);
	SocketUserTable::iterator x = socket_user_table_.find(socket);
	if (x != socket_user_table_.end()) {
		UserConnection* _user = x->second;
		UserAccount::AccountId user_account_id = _user->GetAccountId();
		log_.Infof("Placing user %s (ID %i in drop zone (due to OOB? kill). Will drop next frame."),
			_user->GetLoginName().c_str(), user_account_id);
		RemoveUser(user_account_id, false);
		drop_user_list_.insert(user_account_id);
	} else {
		log_.Infof("Network drop (OOB? of socket on %s."),
			socket->GetTargetAddress().GetAsString().c_str());
		DropSocket(socket);
	}
}*/



NetworkServer::MuxIoSocket* NetworkServer::GetMuxIoSocket() const {
	return mux_socket_;
}

void NetworkServer::AddFilterIoSocket(VIoSocket* socket, const DropFilterCallback& on_drop_callback) {
	socket_receive_filter_table_.insert(SocketReceiveFilterTable::value_type(socket, on_drop_callback));
}

void NetworkServer::RemoveAllFilterIoSockets() {
	socket_receive_filter_table_.clear();
}

void NetworkServer::KillIoSocket(VIoSocket* socket) {
	DropSocket(socket);
}



loginstance(kNetworkServer, NetworkServer);



}
