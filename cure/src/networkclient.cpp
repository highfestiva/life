
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/iobuffer.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/reader.h"
#include "../../lepra/include/systemmanager.h"
#include "../../lepra/include/timer.h"
#include "../include/networkclient.h"
#include "../include/packet.h"
#include "../include/runtimevariable.h"



namespace cure {



NetworkClient::NetworkClient(RuntimeVariableScope* variable_scope):
	NetworkAgent(variable_scope),
	socket_(0),
	login_account_id_(0),
	is_connecting_(false),
	is_logging_in_(false),
	is_socket_connecting_(false),
	server_host_(),
	connect_timeout_(0),
	login_token_(),
	login_timeout_(0),
	login_thread_("LoginThread"),
	safe_receive_toggle_(true) {
}

NetworkClient::~NetworkClient() {
	Disconnect(true);
}

bool NetworkClient::Open(const str& local_address) {
	ScopeLock lock(&lock_);

	SendDisconnect();
	Stop();

	bool ok = !is_socket_connecting_;
	if (!ok) {
		log_.Warning("Already connecting (from some other thread?)...");
		deb_assert(false);
	}
	SocketAddress _local_address;
	uint16 end_port = 0;
	if (ok) {
		ok = _local_address.ResolveRange(local_address, end_port);
		if (!ok) {
			log_.Warning("Unable to resolve public local address, network down?");
			ok = true;
			uint8 ipv4[] = {0,0,0,0};
			IPAddress ip_address(ipv4, sizeof(ipv4));
			_local_address.Set(ip_address, 1025);
			end_port = 65534;
		}
	}
	if (ok) {
		//ScopeLock lock(&lock_);
		for (; _local_address.GetPort() <= end_port; _local_address.SetPort(_local_address.GetPort()+1)) {
			SetMuxSocket(new MuxSocket("Client ", _local_address, false));
			if (mux_socket_->IsOpen()) {
				break;
			}
			delete (mux_socket_);
			mux_socket_ = 0;
		}
		ok = (mux_socket_ != 0);
	}
	return (ok);
}

void NetworkClient::Stop() {
	ScopeLock lock(&lock_);
	deb_assert(!is_socket_connecting_);
	if (socket_) {
		mux_socket_->CloseSocket(socket_);
		socket_ = 0;
	}
	if (mux_socket_) {
		mux_socket_->Close();
	}
	StopLoginThread();
	Parent::Stop();
}



bool NetworkClient::Connect(const str& server_address, double timeout) {
	ScopeLock lock(&lock_);

	bool ok = !is_socket_connecting_;
	if (!ok) {
		log_.Warning("Already connecting (from some other thread?)...");
		deb_assert(false);
	}

	SocketAddress target_address;
	if (ok) {
		ok = target_address.Resolve(server_address);
		if (!ok) {
			log_.Warningf("Could not resolve server address '%s'.", server_address.c_str());
		}
	}
	if (ok) {
		const std::string connection_id = SystemManager::GetRandomId();
		is_socket_connecting_ = true;
		lock.Release();
		socket_ = mux_socket_->Connect(target_address, connection_id, timeout);
		lock.Acquire();
		is_socket_connecting_ = false;
		ok = (socket_ != 0);
	}
	return (ok);
}

void NetworkClient::Disconnect(bool send_disconnect) {
	StopLoginThread();

	ScopeLock lock(&lock_);
	while (!socket_receive_filter_table_.empty()) {
		VSocket* _socket = socket_receive_filter_table_.begin()->first;
		KillIoSocket(_socket);
	}
	if (send_disconnect) {
		SendDisconnect();
	}
	SetLoginAccountId(0);
	if (socket_) {
		Thread::Sleep(0.1);	// Try to wait until data sent. SO_LINGER doesn't seem trustworthy.
	}
	Stop();
}



void NetworkClient::StartConnectLogin(const str& server_host, double connect_timeout, const cure::LoginId& login_token) {
	deb_assert(mux_socket_);
	deb_assert(!is_connecting_);
	deb_assert(!is_logging_in_);
	login_account_id_ = 0;
	is_connecting_ = true;
	is_logging_in_ = true;

	if (connect_timeout != 0) {
		server_host_ = server_host;
	}
	deb_assert(connect_timeout >= 0);
	connect_timeout_ = connect_timeout;
	login_token_ = login_token;
	v_get(login_timeout_, =, variable_scope_, kRtvarNetworkLoginTimeout, 3.0);
	deb_assert(login_timeout_ > 0);
	StopLoginThread();
	login_thread_.Start(this, &NetworkClient::LoginEntry);
}

RemoteStatus NetworkClient::WaitLogin() {
	const int check_count = 50;
	const double sleep_time = connect_timeout_/check_count;
	for (int x = 0; IsConnecting() && x < check_count; ++x) {
		Thread::Sleep(sleep_time);
	}

	RemoteStatus status = kRemoteUnknown;
	if (socket_ == 0) {
		status = kRemoteNoConnection;
	} else {
		cure::Packet* _packet = GetPacketFactory()->Allocate();
		for (int x = 0; x < 100000 && IsLoggingIn(); ++x) {
			ReceiveStatus received = ReceiveNonBlocking(_packet);
			switch (received) {
				case cure::NetworkAgent::kReceiveOk: {
					const size_t message_count = _packet->GetMessageCount();
					if (message_count >= 1) {
						cure::Message* message = _packet->GetMessageAt(0);
						if (message->GetType() == kMessageTypeStatus) {
							cure::MessageStatus* message_status = (cure::MessageStatus*)message;
							status = message_status->GetRemoteStatus();
							SetLoginAccountId(message_status->GetInteger());
						}
					}
				} break;
				case cure::NetworkAgent::kReceiveParseError: {
					log_.Error("Problem with receiving crap data during login wait!");
					status = kRemoteLoginErronousData;
					SetLoginAccountId(0);
				} break;
				case cure::NetworkAgent::kReceiveConnectionBroken: {
					log_.Error("Disconnected from server while waiting for login!");
					status = kRemoteUnknown;
					SetLoginAccountId(0);
				} break;
				case cure::NetworkAgent::kReceiveNoData: {
					// Nothing, really.
				} break;
			}

			if (IsLoggingIn()) {
				Thread::Sleep(0.05);
			}
		}
		GetPacketFactory()->Release(_packet);
	}
	StopLoginThread();

	return (status);
}

bool NetworkClient::IsActive() const {
	return (IsConnecting() || IsLoggingIn() || GetLoginAccountId() != 0);
}

bool NetworkClient::IsConnecting() const {
	return (is_connecting_);
}

bool NetworkClient::IsLoggingIn() const {
	return (is_logging_in_);
}

uint32 NetworkClient::GetLoginAccountId() const {
	return (login_account_id_);
}

void NetworkClient::SetLoginAccountId(uint32 login_account_id) {
	login_account_id_ = login_account_id;
	is_logging_in_ = false;
}



NetworkAgent::VSocket* NetworkClient::GetSocket() const {
	return (socket_);
}



bool NetworkClient::SendAll() {
	ScopeLock lock(&lock_);
	bool ok = true;
	VSocket* _socket;
	while (mux_socket_ && (_socket = mux_socket_->PopSenderSocket()) != 0) {
		ok &= (_socket->SendBuffer() > 0);
	}
	return (ok);
}

NetworkAgent::ReceiveStatus NetworkClient::ReceiveNonBlocking(Packet* packet) {
	ReceiveStatus result = kReceiveConnectionBroken;
	ScopeLock lock(&lock_);
	if (socket_) {
		packet->Release();
		safe_receive_toggle_ = !safe_receive_toggle_;
		const int data_length = socket_->Receive(safe_receive_toggle_, packet->GetWriteBuffer(), packet->GetBufferSize());
		if (data_length == 0) {
			result = kReceiveNoData;
		} else if (data_length > 0) {
			packet->SetPacketSize(data_length);
			if (packet->Parse() == Packet::kParseOk) {
				result = kReceiveOk;
			} else {
				result = kReceiveParseError;
			}
		} else {
			result = kReceiveConnectionBroken;
		}
	}
	return (result);
}

NetworkAgent::ReceiveStatus NetworkClient::ReceiveTimeout(Packet* packet, double timeout) {
	const double single_loop_time = 0.05;
	const int loop_count = (int)(timeout/single_loop_time)+1;
	ReceiveStatus status = kReceiveNoData;
	for (int x = 0; status == kReceiveNoData && x < loop_count; ++x) {
		status = ReceiveNonBlocking(packet);
		if (status == kReceiveNoData) {
			Thread::Sleep(single_loop_time);
		}
	}
	return (status);
}

NetworkAgent::ReceiveStatus NetworkClient::ReceiveMore(Packet* packet) {
	ScopeLock lock(&lock_);
	if (!socket_) {
		return kReceiveConnectionBroken;
	}

	ReceiveStatus result = kReceiveConnectionBroken;
	const int data_length = socket_->Receive(true, packet->GetWriteBuffer() + packet->GetPacketSize(),
		packet->GetBufferSize() - packet->GetPacketSize());
	if (data_length == 0) {
		result = kReceiveNoData;
	} else if (data_length > 0) {
		packet->SetPacketSize(packet->GetPacketSize() + data_length);
		result = kReceiveOk;
	}
	return result;
}



bool NetworkClient::SendLoginRequest(const LoginId& login_id) {
	bool ok = false;
	ScopeLock lock(&lock_);
	if (socket_) {
		Packet* _packet = packet_factory_->Allocate();
		_packet->Release();
		MessageLoginRequest* login_request = (MessageLoginRequest*)packet_factory_->GetMessageFactory()->Allocate(kMessageTypeLoginRequest);
		_packet->AddMessage(login_request);	// Leave ownership of message with packet.
		login_request->Store(_packet, login_id.GetName(), login_id.GetMangledPassword());
		ok = Parent::PlaceInSendBuffer(true, socket_, _packet);
		if (ok) {
			ok = SendAll();
		}
		packet_factory_->Release(_packet);
	}
	return (ok);
}

void NetworkClient::SendDisconnect() {
	if (socket_) {
		cure::Packet* _packet = GetPacketFactory()->Allocate();
		SendStatusMessage(socket_, 0, cure::kRemoteNoConnection, cure::MessageStatus::kInfoLogin, "", _packet);
		GetPacketFactory()->Release(_packet);
	}
	SendAll();
}

void NetworkClient::LoginEntry() {
	deb_assert(mux_socket_);

	bool ok = true;
	if (ok && connect_timeout_ > 0) {
		log_.Info("Connecting to " + server_host_ + ".");
		int x = 0;
		do {
			if (!ok) {
				log_.Info("Retrying connect...");
			}
			ok = Connect(server_host_, connect_timeout_);
		}
		while (++x <= v_slowget(variable_scope_, kRtvarNetworkConnectRetrycount, 1) && !ok &&
			!SystemManager::GetQuitRequest() && !login_thread_.GetStopRequest());
	}
	is_connecting_ = false;
	if (ok) {
		ok = (socket_ != 0);	// In case we skipped trying to connect; we need to be connected prior to this.
	}
	const bool connect_ok = ok;
	if (ok) {
		log_.Info("Connected.");
		ok = SendLoginRequest(login_token_);
	}
	if (ok) {
		Timer timer;
		while (is_logging_in_ && timer.GetTimeDiff() < login_timeout_ &&
			!SystemManager::GetQuitRequest() && !login_thread_.GetStopRequest()) {
			Thread::Sleep(0.05);
			timer.UpdateTimer();
		}
		ok = (login_account_id_ != 0);
	}
	is_logging_in_ = false;
	if (ok) {
		log_.Infof("Logged in on account # %u.", login_account_id_);
		// TODO: handshaking for pingtime with server.
	}
	if (!ok) {
		if (!connect_ok) {
			log_.Error("Could not connect.");
		} else {
			log_.Error("Could not login.");
		}
		Disconnect(false);
	}
}

void NetworkClient::StopLoginThread() {
	if (Thread::GetCurrentThreadId() != login_thread_.GetThreadId()) {
		login_thread_.Join(connect_timeout_+login_timeout_+0.5);
		login_thread_.Kill();
	}
}



NetworkClient::MuxIoSocket* NetworkClient::GetMuxIoSocket() const {
	return mux_socket_;
}

void NetworkClient::AddFilterIoSocket(VIoSocket* socket, const DropFilterCallback& on_drop_callback) {
	socket_receive_filter_table_.insert(SocketReceiveFilterTable::value_type(socket, on_drop_callback));
}

void NetworkClient::RemoveAllFilterIoSockets() {
	socket_receive_filter_table_.clear();
}

void NetworkClient::KillIoSocket(VIoSocket* socket) {
	if (socket) {
		socket->SendBuffer();
	}
	SocketReceiveFilterTable::iterator x = socket_receive_filter_table_.find(socket);
	if (x != socket_receive_filter_table_.end()) {
		x->second(x->first);
		socket_receive_filter_table_.erase(x);
	}
	if (socket) {
		mux_socket_->CloseSocket(socket);
		if (socket == socket_) {
			socket_ = 0;
		}
	}
}



/*void NetworkClient::OnCloseSocket(VSocket*) {
	Disconnect(false);
}*/



loginstance(kNetworkClient, NetworkClient);



}
