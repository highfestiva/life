
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/tcpmuxsocket.h"
#include "../include/hashutil.h"
#include "../include/systemmanager.h"



namespace lepra {



TcpMuxSocket::TcpMuxSocket(const str& name, const SocketAddress& local_address, bool is_server,
	unsigned max_pending_connection_count, unsigned max_connection_count):
	MuxIo(max_pending_connection_count, max_connection_count),
	TcpListenerSocket(local_address, is_server),
	accept_thread_(name+"TcpMuxAccept "+local_address.GetAsString()),
	select_thread_(name+"TcpMuxSelect "+local_address.GetAsString()),
	connect_id_timeout_(DEFAULT_CONNECT_ID_TIMEOUT),
	active_receiver_map_changed_(false),
	connected_socket_semaphore_(100),
	v_sent_byte_count_(0),
	v_received_byte_count_(0) {
	log_trace("TcpMuxSocket()");
	if (is_server) {
		accept_thread_.Start(this, &TcpMuxSocket::AcceptThreadEntry);
	}
	select_thread_.Start(this, &TcpMuxSocket::SelectThreadEntry);
}

TcpMuxSocket::~TcpMuxSocket() {
	log_trace("~TcpMuxSocket()");
	accept_thread_.RequestStop();
	select_thread_.RequestStop();
	Close();
	ReleaseSocketThreads();
	accept_thread_.Join(5.0f);
	select_thread_.Join(5.0f);
}

TcpVSocket* TcpMuxSocket::Connect(const SocketAddress& target_address, const std::string& connection_id, double timeout) {
	TcpVSocket* _socket = new TcpVSocket(*this, GetDatagramReceiver());
	bool ok = _socket->Connect(target_address, connection_id, timeout);
	if (ok) {
		std::string connect_string(connection_string_, sizeof(connection_string_));
		connect_string += connection_id;
		ok = (_socket->Send(connect_string.c_str(), (int)connect_string.length()) == (int)connect_string.length());
		if (!ok) {
			log_.Error("Could not send connect data to server!");
		}
	}
	if (ok) {
		Datagram buffer;
		buffer.data_size_ = _socket->Receive(buffer.data_buffer_, sizeof(buffer.data_buffer_), timeout, false);
		ok = (buffer.data_size_ == sizeof(acception_string_) && ::memcmp(acception_string_, buffer.data_buffer_, sizeof(acception_string_)) == 0);
		if (ok) {
			AddConnectedSocket(_socket);
			log_trace("Connect went through!");
		} else {
			if (buffer.data_size_ == 0) {
				log_debug("Remote end seems dead. Firewall?");
			} else if (buffer.data_size_ < 0) {
				log_.Error("Connect was refused. Firewall?");
			} else if (buffer.data_size_ > 0) {
				log_.Error("Connect was replied to with jibberish. Wassup?");
			}
		}
	}
	if (!ok) {
		CloseSocket(_socket, true);
		_socket = 0;
	}
	return (_socket);
}

TcpVSocket* TcpMuxSocket::Accept() {
	TcpVSocket* _socket = 0;
	while (IsOpen() && (_socket = PollAccept()) == 0) {
		const double wait_time = (pending_connect_id_map_.GetCountSafe() > 0)? 0.001 : 10.0;
		accept_semaphore_.Wait(wait_time);
	}
	return (_socket);
}

TcpVSocket* TcpMuxSocket::PollAccept() {
	HiResTimer time(false);
	TcpVSocket* _tcp_socket = 0;
	size_t pending_socket_count = pending_connect_id_map_.GetCountSafe();
	for (size_t x = 0; !_tcp_socket && x < pending_socket_count; ++x) {
		{
			ScopeLock lock(&io_lock_);
			if (pending_connect_id_map_.GetCount() > 0) {
				pending_connect_id_map_.PopFront(_tcp_socket, time);
			}
		}
		if (_tcp_socket) {
			log_trace("Popped a connected socket.");
			AcceptStatus accept_status = QueryReceiveConnectString(_tcp_socket);
			if (accept_status == kAcceptClose) {
				CloseSocket(_tcp_socket, true);
				_tcp_socket = 0;
			} else if (accept_status == kAcceptQueryWait) {
				if (time.QueryTimeDiff() >= connect_id_timeout_) {
					log_debug("Connected socket ID-timed out => dropped.");
					CloseSocket(_tcp_socket, true);
				} else {
					ScopeLock lock(&io_lock_);
					pending_connect_id_map_.PushBack(_tcp_socket, time);
				}
				_tcp_socket = 0;
			}
		}
	}
	return (_tcp_socket);
}

void TcpMuxSocket::DispatchCloseSocket(TcpVSocket* socket) {
	if (!close_dispatcher_.empty()) {
		close_dispatcher_(socket);
	}
	CloseSocket(socket, false);
}

void TcpMuxSocket::CloseSocket(TcpVSocket* socket, bool force_delete) {
	{
		ScopeLock lock(&io_lock_);
		log_trace("Closing TcpVSocket.");
		force_delete |= pending_connect_id_map_.Remove(socket);
		force_delete |= RemoveConnectedSocketNoLock(socket);
		RemoveSenderNoLock(socket);
		RemoveReceiverNoLock(socket);
	}
	if (force_delete) {
		delete (socket);
	}
}

TcpVSocket* TcpMuxSocket::PopReceiverSocket() {
	TcpVSocket* _socket = (TcpVSocket*)PopReceiver();
	if (_socket) {
		active_receiver_map_changed_ = true;
		connected_socket_semaphore_.Signal();
	}
	return (_socket);
}

TcpVSocket* TcpMuxSocket::PopSenderSocket() {
	TcpVSocket* _socket = (TcpVSocket*)PopSender();
	if (_socket) {
		log_trace("Popped TCP sender socket.");
	}
	return (_socket);
}

uint64 TcpMuxSocket::GetTotalSentByteCount() const {
	return (GetSentByteCount()+v_sent_byte_count_);
}

uint64 TcpMuxSocket::GetTotalReceivedByteCount() const {
	return (GetReceivedByteCount()+v_received_byte_count_);
}

void TcpMuxSocket::AddTotalSentByteCount(unsigned byte_count) {
	BusLock::Add(&v_sent_byte_count_, byte_count);
}

void TcpMuxSocket::AddTotalReceivedByteCount(unsigned byte_count) {
	BusLock::Add(&v_received_byte_count_, byte_count);
}

void TcpMuxSocket::SetConnectIdTimeout(double timeout) {
	connect_id_timeout_ = timeout;
}

TcpSocket* TcpMuxSocket::CreateSocket(s_socket socket, const SocketAddress& target_address,
	TcpListenerSocket* server_socket, DatagramReceiver* receiver) {
	return (new TcpVSocket(socket, target_address, *(TcpMuxSocket*)server_socket, receiver));
}

void TcpMuxSocket::AddConnectedSocket(TcpVSocket* socket) {
	ScopeLock lock(&io_lock_);
	log_volatile(log_.Tracef("AddConnectedSocket(%i)", socket->GetSysSocket()));
	connected_socket_map_.insert(SocketVMap::value_type(socket->GetSysSocket(), socket));
	active_receiver_map_changed_ = true;
	connected_socket_semaphore_.Signal();
}

bool TcpMuxSocket::RemoveConnectedSocketNoLock(TcpVSocket* socket) {
	log_volatile(log_.Tracef("RemoveConnectedSocketNoLock(%i)", socket->GetSysSocket()));
	size_t erase_count = connected_socket_map_.erase(socket->GetSysSocket());
	active_receiver_map_changed_ = true;
	connected_socket_semaphore_.Signal();
	return (erase_count > 0);
}

int TcpMuxSocket::BuildConnectedSocketSet(FdSet& socket_set) {
	int socket_count;
	if (active_receiver_map_changed_) {
		ScopeLock lock(&io_lock_);
		active_receiver_map_changed_ = false;
		//socket_count = connected_socket_map_.size();
		//log_volatile(log_.Tracef("Rebuilding active receiver map with %i possible sockets.", socket_count));
		LEPRA_FD_ZERO(&socket_set);
		for (SocketVMap::iterator x = connected_socket_map_.begin(); x != connected_socket_map_.end(); ++x) {
			TcpVSocket* _socket = x->second;
			if (!IsReceiverNoLock(_socket)) {
				s_socket __sys_socket = x->first;
#pragma warning(push)
#pragma warning(disable: 4127)	// MSVC warning: conditional expression is constant.
				LEPRA_FD_SET((sys_socket)__sys_socket, &socket_set);
#pragma warning(pop)
			}
		}
		socket_count = FdSetHelper::Copy(backup_fd_set_, socket_set);
	} else {
		socket_count = FdSetHelper::Copy(socket_set, backup_fd_set_);
	}
	deb_assert(socket_count == (int)LEPRA_FD_GET_COUNT(&socket_set));
	return (socket_count);
}

void TcpMuxSocket::PushReceiverSockets(const FdSet& socket_set) {
	ScopeLock lock(&io_lock_);
	// TODO: optimize (using platform specifics?).
	bool added = false;
	for (SocketVMap::iterator y = connected_socket_map_.begin(); y != connected_socket_map_.end(); ++y) {
		s_socket __sys_socket = y->first;
		if (FD_ISSET(__sys_socket, (fd_set*)LEPRA_FDS(&socket_set))) {
			TcpVSocket* _socket = y->second;
			log_debug("Adding receiver socket.");
			AddReceiverNoLock(_socket);
			added = true;
		} else {
			log_debug("Didn't add receiver socket.");
		}
	}
	if (added) {
		active_receiver_map_changed_ = true;
		connected_socket_semaphore_.Signal();
	}
}

TcpMuxSocket::AcceptStatus TcpMuxSocket::QueryReceiveConnectString(TcpVSocket* socket) {
	if (banned_ip_table_.Find(socket->GetTargetAddress().GetIP()) != banned_ip_table_.End()) {
		log_.Warning("Received a connect from a banned client.");
		return (kAcceptClose);	// RAII is great here.
	}

	// Check if we get the connection string (containing an ID that connects the TCP and UDP sockets).
	AcceptStatus accept_status = kAcceptQueryWait;
	Datagram buffer;
	buffer.data_size_ = socket->Receive(buffer.data_buffer_, sizeof(buffer.data_buffer_), false);
	if (buffer.data_size_ > 0) {
		log_trace("Received a connect string.");

		// Look for "VSocket connect magic".
		if (buffer.data_size_ >= (int)sizeof(connection_string_) &&
			::memcmp(buffer.data_buffer_, connection_string_, sizeof(connection_string_)) == 0) {
			std::string _connection_id((const char*)&buffer.data_buffer_[sizeof(connection_string_)], buffer.data_size_-sizeof(connection_string_));
			unsigned socket_count = GetConnectionCount();
			if (socket_count < max_connection_count_) {
				socket->SetConnectionId(_connection_id);
				if (socket->Send(acception_string_, sizeof(acception_string_)) == sizeof(acception_string_)) {
					accept_status = kAcceptOk;
					AddConnectedSocket(socket);
				} else {
					accept_status = kAcceptClose;
				}
			} else {
				log_.Warning("Too many sockets!");
				accept_status = kAcceptClose;
			}
		} else {
			log_.Warning("Invalid connection string.");
			accept_status = kAcceptClose;
		}
	} else {
		log_trace("Waited for connect magic, but none came this loop.");
	}
	return (accept_status);
}

void TcpMuxSocket::ReleaseSocketThreads() {
	MuxIo::ReleaseSocketThreads();
	for (int x = 0; x < 10; ++x) {
		connected_socket_semaphore_.Signal();
	}
}

void TcpMuxSocket::AcceptThreadEntry() {
	log_trace("Accept thread running");

	while (IsOpen() && !accept_thread_.GetStopRequest()) {
		TcpVSocket* _socket = (TcpVSocket*)TcpListenerSocket::Accept(&TcpMuxSocket::CreateSocket);
		if (_socket) {
			log_trace("Received a connect.");
			HiResTimer time(false);
			ScopeLock lock(&io_lock_);
			pending_connect_id_map_.PushBack(_socket, time);
			accept_semaphore_.Signal();
		}
	}
}

void TcpMuxSocket::SelectThreadEntry() {
	log_trace("Select thread running");

	FdSet read_set;
	LEPRA_FD_ZERO(&backup_fd_set_);
	active_receiver_map_changed_ = true;
	while (IsOpen() && !select_thread_.GetStopRequest()) {
		int socket_count = BuildConnectedSocketSet(read_set);
		if (socket_count > 0) {
			timeval _timeout;
			_timeout.tv_sec = 0;
			_timeout.tv_usec = 200000;
			deb_assert(socket_count == (int)LEPRA_FD_GET_COUNT(&read_set));
			FdSet exception_set;
			FdSetHelper::Copy(exception_set, read_set);
			int select_count = ::select(LEPRA_FD_GET_MAX_HANDLE(&read_set)+1, LEPRA_FDS(&read_set), 0, LEPRA_FDS(&exception_set), &_timeout);
			if (select_count > 0) {
				log_trace("Picked up a receive socket.");
				PushReceiverSockets(read_set);
			} else if (select_count < 0) {
				int e = SOCKET_LAST_ERROR();
				log_.Warningf("Could not ::select() properly. Error=%i, sockets=%u, exception set=%u.", e, socket_count, LEPRA_FD_GET_COUNT(&exception_set));

				for (unsigned x = 0; x < LEPRA_FD_GET_COUNT(&exception_set); ++x) {
					const s_socket __sys_socket = LEPRA_FD_GET(&exception_set, x);
					int error_code = 0;
					socklen_t error_size = sizeof(error_code);
					if (::getsockopt(__sys_socket, SOL_SOCKET, SO_ERROR, (char*)&error_code, &error_size) == 0 &&
						error_code == 0) {
						continue;
					}

					TcpVSocket* _socket = 0;
					{
						ScopeLock lock(&io_lock_);
						SocketVMap::iterator y = connected_socket_map_.find(__sys_socket);
						if (y != connected_socket_map_.end()) {
							_socket = y->second;
						}
					}
					if (_socket) {
						log_.Warning("Kicking socket due to ::select() failure.");
						DispatchCloseSocket(_socket);
					}
				}
			}
		} else {
			log_debug("Going into \"wait for socket connect\" state.");
			connected_socket_semaphore_.Wait(10.0f);
			log_debug("Leaving \"wait for socket connect\" state.");
		}
	}
}



TcpVSocket::TcpVSocket(TcpMuxSocket& mux_socket, DatagramReceiver* receiver):
	TcpSocket(receiver) {
	log_trace("TcpVSocket()");
	mux_io_ = &mux_socket;
}

TcpVSocket::TcpVSocket(s_socket socket, const SocketAddress& target_address, TcpMuxSocket& mux_socket,
	DatagramReceiver* receiver):
	TcpSocket(socket, target_address, &mux_socket, receiver) {
	log_trace("TcpVSocket()");
	mux_io_ = &mux_socket;
	MakeNonBlocking();
}

TcpVSocket::~TcpVSocket() {
	log_trace("~TcpVSocket()");
	SendBuffer();
}

bool TcpVSocket::Connect(const SocketAddress& target_address, const std::string& connection_id, double /*timeout*/) {
	// TODO: respect _timeout?
	SetConnectionId(connection_id);
	bool connected = TcpSocket::Connect(target_address);
	if (connected) {
		MakeNonBlocking();
		DisableNagleAlgo();
		// TODO: send connection ID to acceptor.
	}
	return (connected);
}

int TcpVSocket::SendBuffer() {
	int send_result = 0;
	if (send_buffer_.data_size_ > 0) {
		send_result = Send(send_buffer_.data_buffer_, send_buffer_.data_size_);
		if (send_result > 0) {
			((TcpMuxSocket*)mux_io_)->AddTotalSentByteCount(send_result);
		}
		send_buffer_.Init();
	}
	return (send_result);
}

int TcpVSocket::Receive(void* data, int max_size, bool datagram) {
	int receive_result = datagram? TcpSocket::ReceiveDatagram(data, max_size) : TcpSocket::Receive(data, max_size);
	if (receive_result > 0) {
		((TcpMuxSocket*)mux_io_)->AddTotalReceivedByteCount(receive_result);
	}
	return (receive_result);
}

int TcpVSocket::Receive(void* data, int max_size, double timeout, bool datagram) {
	HiResTimer timer(false);
	int receive_count;
	do {
		receive_count = Receive(data, max_size, datagram);
		if (receive_count == 0) {
			Thread::Sleep(0.001);
			timer.UpdateTimer();
		}
	} while (receive_count == 0 && timer.GetTimeDiff() < timeout && !SystemManager::GetQuitRequest());
	return (receive_count);
}



DualMuxSocket::DualMuxSocket(const str& name, const SocketAddress& local_address, bool is_server,
	unsigned max_pending_connection_count, unsigned max_connection_count):
	tcp_mux_socket_(new TcpMuxSocket(name, local_address, is_server, max_pending_connection_count, max_connection_count)),
	udp_mux_socket_(new UdpMuxSocket(name, local_address, is_server, max_pending_connection_count, max_connection_count)),
	connect_dual_timeout_(DEFAULT_CONNECT_DUAL_TIMEOUT),
	pop_safe_toggle_(false) {
	tcp_mux_socket_->SetCloseCallback(this, &DualMuxSocket::OnCloseTcpSocket);
	log_trace("DualMuxSocket()");
}

DualMuxSocket::~DualMuxSocket() {
	log_trace("~DualMuxSocket()");

	ScopeLock lock(&lock_);
	while (!id_socket_map_.empty()) {
		CloseSocket(id_socket_map_.begin()->second);
	}

	delete (tcp_mux_socket_);
	tcp_mux_socket_ = 0;
	delete (udp_mux_socket_);
	udp_mux_socket_ = 0;
}

bool DualMuxSocket::IsOpen() const {
	ScopeLock lock(&lock_);
	return (tcp_mux_socket_ && udp_mux_socket_ && tcp_mux_socket_->IsOpen() && udp_mux_socket_->IsOpen());
}

bool DualMuxSocket::IsOpen(DualSocket* socket) const {
	ScopeLock lock(&lock_);
	return (HashUtil::FindMapObject(id_socket_map_, socket->GetConnectionId()) == socket);
}

DualSocket* DualMuxSocket::Connect(const SocketAddress& target_address, const std::string& connection_id, double timeout) {
	// Simulatanously connect TCP and UDP.
	ScopeLock lock(&lock_);
	Semaphore connected_semaphore;
	Connector<TcpMuxSocket, TcpVSocket> tcp_connector("TCP connector", tcp_mux_socket_, target_address, connection_id, timeout, connected_semaphore);
	Connector<UdpMuxSocket, UdpVSocket> udp_connector("UDP connector", udp_mux_socket_, target_address, connection_id, timeout, connected_semaphore);
	if (tcp_connector.Start() && udp_connector.Start()) {
		// Wait for both connectors to finish.
		HiResTimer time(false);
		connected_semaphore.Wait(timeout);
		connected_semaphore.Wait(timeout - time.PopTimeDiff());
		udp_connector.Join();
		tcp_connector.Join();	// Join TCP last, in hope that it'll have made it through.
	}
	DualSocket* _socket = 0;
	if (tcp_connector.socket_ && udp_connector.socket_) {
		_socket = new DualSocket(this, connection_id);
		AddSocket(_socket, tcp_connector.socket_, udp_connector.socket_);
	} else {
		if (tcp_connector.socket_) {
			tcp_mux_socket_->CloseSocket(tcp_connector.socket_, true);
		}
		if (udp_connector.socket_) {
			udp_mux_socket_->CloseSocket(udp_connector.socket_);
		}
	}
	return (_socket);
}

void DualMuxSocket::Close() {
	ScopeLock lock(&lock_);
	if (tcp_mux_socket_) {
		tcp_mux_socket_->Close();
	}
	if (udp_mux_socket_) {
		udp_mux_socket_->Close();
	}
}

DualSocket* DualMuxSocket::PollAccept() {
	ScopeLock lock(&lock_);

	// Slaughter old single-sided-connector (ONLY TCP or ONLY UDP).
	KillNonDualConnected();

	DualSocket* _socket = 0;

	// Try to connect TCP to an existing DualSocket, or create a new one.
	TcpVSocket* _tcp_socket = tcp_mux_socket_->PollAccept();
	if (_tcp_socket) {
		IdSocketMap::iterator x = id_socket_map_.find(_tcp_socket->GetConnectionId());
		if (x != id_socket_map_.end()) {
			// UDP is already connected.
			_socket = x->second;
			AddSocket(_socket, _tcp_socket, 0);
		} else {
			// TCP is first.
			_socket = new DualSocket(this, _tcp_socket->GetConnectionId());
			AddSocket(_socket, _tcp_socket, 0);
		}
	}

	if (!_tcp_socket) {
		// Try to connect UDP to an existing DualSocket, or create a new one.
		UdpVSocket* _udp_socket = udp_mux_socket_->PollAccept();
		if (_udp_socket) {
			IdSocketMap::iterator x = id_socket_map_.find(_udp_socket->GetConnectionId());
			if (x != id_socket_map_.end()) {
				// TCP is already connected.
				_socket = x->second;
				AddSocket(_socket, 0, _udp_socket);
			} else {
				// UDP is first.
				_socket = new DualSocket(this, _udp_socket->GetConnectionId());
				AddSocket(_socket, 0, _udp_socket);
			}
		}
	}

	// If both TCP and UDP are connected, we roll.
	if (_socket && !_socket->IsOpen()) {
		_socket = 0;
	}
	return (_socket);
}

DualSocket* DualMuxSocket::PopReceiverSocket() {
	pop_safe_toggle_ = !pop_safe_toggle_;
	return PopReceiverSocket(pop_safe_toggle_);
}

DualSocket* DualMuxSocket::PopReceiverSocket(bool safe) {
	ScopeLock lock(&lock_);

	DualSocket* _socket = 0;

	if (safe) {
		TcpVSocket* _tcp_socket = tcp_mux_socket_->PopReceiverSocket();
		if (_tcp_socket) {
			_socket = HashUtil::FindMapObject(tcp_socket_map_, _tcp_socket);
		}
	} else {
		UdpVSocket* _udp_socket = udp_mux_socket_->PopReceiverSocket();
		if (_udp_socket) {
			_socket = HashUtil::FindMapObject(udp_socket_map_, _udp_socket);
		}
	}

	// If we received something, even though the socket isn't fully opened we throw it away.
	if (_socket) {
		if (!_socket->IsOpen()) {
			CloseSocket(_socket);
			_socket = 0;
		} else {
			_socket->SetSafeReceive(safe);
		}
	}
	return (_socket);
}

DualSocket* DualMuxSocket::PopSenderSocket() {
	ScopeLock lock(&lock_);

	DualSocket* _socket = 0;

	//if (!_socket)
	{
		TcpVSocket* _tcp_socket = tcp_mux_socket_->PopSenderSocket();
		if (_tcp_socket) {
			_socket = HashUtil::FindMapObject(tcp_socket_map_, _tcp_socket);
		}
	}
	if (!_socket) {
		UdpVSocket* _udp_socket = udp_mux_socket_->PopSenderSocket();
		if (_udp_socket) {
			_socket = HashUtil::FindMapObject(udp_socket_map_, _udp_socket);
		}
	}

	deb_assert(!_socket || _socket->IsOpen());
	return (_socket);
}

uint64 DualMuxSocket::GetSentByteCount() const {
	return GetSentByteCount(false) + GetSentByteCount(true);
}

uint64 DualMuxSocket::GetSentByteCount(bool safe) const {
	ScopeLock lock(&lock_);
	return safe? tcp_mux_socket_->GetTotalSentByteCount() : udp_mux_socket_->GetSentByteCount();
}

uint64 DualMuxSocket::GetReceivedByteCount() const {
	return GetReceivedByteCount(false) + GetReceivedByteCount(true);
}

uint64 DualMuxSocket::GetReceivedByteCount(bool safe) const {
	ScopeLock lock(&lock_);
	return safe? tcp_mux_socket_->GetTotalReceivedByteCount() : udp_mux_socket_->GetReceivedByteCount();
}

SocketAddress DualMuxSocket::GetLocalAddress() const {
	if (tcp_mux_socket_) {
		// RAII.
		return (tcp_mux_socket_->GetLocalAddress());
	}
	if (udp_mux_socket_) {
		// RAII.
		return (udp_mux_socket_->GetLocalAddress());
	}
	return (SocketAddress());
}

unsigned DualMuxSocket::GetConnectionCount() const {
	ScopeLock lock(&lock_);
	return ((unsigned)id_socket_map_.size());
}

void DualMuxSocket::SetSafeConnectIdTimeout(double timeout) {
	tcp_mux_socket_->SetConnectIdTimeout(timeout);
}

void DualMuxSocket::SetConnectDualTimeout(double timeout) {
	connect_dual_timeout_ = timeout;
}

void DualMuxSocket::SetDatagramReceiver(DatagramReceiver* receiver) {
	tcp_mux_socket_->SetDatagramReceiver(receiver);
}

void DualMuxSocket::CloseSocket(DualSocket* socket) {
	ScopeLock lock(&lock_);
	if (HashUtil::FindMapObject(id_socket_map_, socket->GetConnectionId()) == socket) {
		if (socket->GetTcpSocket()) {
			tcp_socket_map_.erase(socket->GetTcpSocket());
			tcp_mux_socket_->CloseSocket(socket->GetTcpSocket(), false);
		}

		if (socket->GetUdpSocket()) {
			udp_socket_map_.erase(socket->GetUdpSocket());
			udp_mux_socket_->CloseSocket(socket->GetUdpSocket());
		}

		id_socket_map_.erase(socket->GetConnectionId());
		pending_dual_connect_map_.Remove(socket);
		delete (socket);
	}
}

void DualMuxSocket::DispatchCloseSocket(DualSocket* socket) {
	if (!close_dispatcher_.empty()) {
		close_dispatcher_(socket);
	}
	CloseSocket(socket);
}

void DualMuxSocket::AddUdpReceiverSocket(UdpVSocket* udp_socket) {
	ScopeLock lock(&lock_);
	deb_assert(HashUtil::FindMapObject(udp_socket_map_, udp_socket));
	udp_mux_socket_->AddReceiver(udp_socket);
}

void DualMuxSocket::AddSocket(DualSocket* socket, TcpVSocket* tcp_socket, UdpVSocket* udp_socket) {
	if (tcp_socket && udp_socket) {
		deb_assert(!HashUtil::FindMapObject(id_socket_map_, socket->GetConnectionId()));
	}
	if (tcp_socket) {
		deb_assert(!HashUtil::FindMapObject(tcp_socket_map_, tcp_socket));
		socket->SetSocket(tcp_socket);
		tcp_socket_map_.insert(TcpSocketMap::value_type(tcp_socket, socket));
	}
	if (udp_socket) {
		deb_assert(!HashUtil::FindMapObject(udp_socket_map_, udp_socket));
		socket->SetSocket(udp_socket);
		udp_socket_map_.insert(UdpSocketMap::value_type(udp_socket, socket));
	}
	if (!HashUtil::FindMapObject(id_socket_map_, socket->GetConnectionId())) {
		log_trace("Adding DualSocket with ID "+
			strutil::ReplaceCtrlChars(socket->GetConnectionId(), '.')+
			(socket->GetTcpSocket()?" TCP set,":" no TCP,")+str()+
			(socket->GetUdpSocket()?" UDP set.":" no UDP."));
		id_socket_map_.insert(IdSocketMap::value_type(socket->GetConnectionId(), socket));
	} else {
		log_debug("Appending info to DualSocket with ID "+
			strutil::ReplaceCtrlChars(socket->GetConnectionId(), '.') +
			(socket->GetTcpSocket()?" TCP set,":" no TCP,")+str()+
			(socket->GetUdpSocket()?" UDP set.":" no UDP."));
	}
	if (!socket->GetTcpSocket() || !socket->GetUdpSocket()) {
		log_trace("Adding a not-yet-fully-connected DualSocket to 'pending dual' list.");
		pending_dual_connect_map_.PushBack(socket, HiResTimer(false));
	} else {
		log_trace("Dropping a fully connected DualSocket from 'pending dual' list.");
		pending_dual_connect_map_.Remove(socket);
	}
}

void DualMuxSocket::KillNonDualConnected() {
	for (SocketTimeMap::Iterator x = pending_dual_connect_map_.First(); x != pending_dual_connect_map_.End();) {
		HiResTimer& time = x.GetObject();
		if (time.QueryTimeDiff() >= connect_dual_timeout_) {
			log_debug("Connected socket dual-timed out => dropped.");
			DualSocket* _socket = x.GetKey();
			++x;	// Must be increased before we close the socket (implicitly removes it from the table).
			CloseSocket(_socket);
		} else {
			++x;
		}
	}
}

void DualMuxSocket::OnCloseTcpSocket(TcpVSocket* tcp_socket) {
	DualSocket* _socket;
	{
		ScopeLock lock(&lock_);
		_socket = HashUtil::FindMapObject(tcp_socket_map_, tcp_socket);
	}
	if (_socket) {
		DispatchCloseSocket(_socket);
	}
}



DualSocket::DualSocket(DualMuxSocket* mux_socket, const std::string& connection_id):
	ConnectionWithId(),
	mux_socket_(mux_socket),
	tcp_socket_(0),
	udp_socket_(0),
	default_safe_send_(false),
	default_safe_receive_(false) {
	log_trace("DualSocket()");
	SetConnectionId(connection_id);
}

bool DualSocket::SetSocket(TcpVSocket* socket) {
	bool ok = (!tcp_socket_);
	deb_assert(ok);
	if (ok) {
		tcp_socket_ = socket;
	}
	return (ok);
}

bool DualSocket::SetSocket(UdpVSocket* socket) {
	bool ok = (!udp_socket_);
	deb_assert(ok);
	if (ok) {
		udp_socket_ = socket;
	}
	return (ok);
}

bool DualSocket::IsOpen() const {
	return (tcp_socket_ && udp_socket_ && tcp_socket_->IsOpen() && mux_socket_->IsOpen());
}

void DualSocket::ClearAll() {
	tcp_socket_ = 0;
	udp_socket_ = 0;
	ClearConnectionId();
}

SocketAddress DualSocket::GetLocalAddress() const {
	if (mux_socket_) {
		// RAII.
		return (mux_socket_->GetLocalAddress());
	}
	return (SocketAddress());
}

SocketAddress DualSocket::GetTargetAddress() const {
	if (tcp_socket_) {
		// RAII.
		return (tcp_socket_->GetTargetAddress());
	}
	if (udp_socket_) {
		// RAII.
		udp_socket_->GetTargetAddress();
	}
	return (SocketAddress());
}

void DualSocket::ClearOutputData() {
	if (tcp_socket_) {
		tcp_socket_->ClearOutputData();
	}
	if (udp_socket_) {
		udp_socket_->ClearOutputData();
	}
}

void DualSocket::SetSafeSend(bool safe) {
	default_safe_send_ = safe;
}

Datagram& DualSocket::GetSendBuffer() const {
	return GetSendBuffer(default_safe_send_);
}

Datagram& DualSocket::GetSendBuffer(bool safe) const {
	if (safe) {
		return (tcp_socket_->GetSendBuffer());
	} else {
		return (udp_socket_->GetSendBuffer());
	}
}

IOError DualSocket::AppendSendBuffer(const void* data, int _length) {
	return AppendSendBuffer(default_safe_send_, data, _length);
}

IOError DualSocket::AppendSendBuffer(bool safe, const void* data, int _length) {
	IOError send_result = kIoOk;
	if (safe) {
		send_result = tcp_socket_->AppendSendBuffer(data, (int)_length);
	} else {
		send_result = udp_socket_->AppendSendBuffer(data, _length);
	}
	return (send_result);
}

int DualSocket::SendBuffer() {
	int tcp_sent_count = 0;
	if (tcp_socket_ && !tcp_socket_->GetInSenderList()) {	// If it is in the sender list, it will be served later.
		tcp_sent_count = tcp_socket_->SendBuffer();
		log_volatile(log_.Tracef("TCP send returned %i.", tcp_sent_count));
	}
	int udp_sent_count = 0;
	if (udp_socket_ && !udp_socket_->GetInSenderList()) {	// If it is in the sender list, it will be served later.
		udp_sent_count = udp_socket_->SendBuffer();
		log_volatile(log_.Tracef("UDP send returned %i.", udp_sent_count));
	}
	int sent_count = -1;
	if (tcp_sent_count >= 0 && udp_sent_count >= 0) {
		sent_count = tcp_sent_count+udp_sent_count;
	}
	return (sent_count);
}

bool DualSocket::HasSendData() const {
	return (tcp_socket_->HasSendData() || udp_socket_->HasSendData());
}

void DualSocket::SetSafeReceive(bool safe) {
	default_safe_receive_ = safe;
}

int DualSocket::Receive(void* data, int _length) {
	return Receive(default_safe_receive_, data, _length);
}

int DualSocket::Receive(bool safe, void* data, int _length) {
	int receive_count;
	if (safe) {
		receive_count = tcp_socket_->Receive(data, _length, true);
		//log_volatile(log_.Tracef("TCP recv returned %i.", receive_count));
	} else {
		receive_count = udp_socket_->Receive(data, _length);
		//log_volatile(log_.Tracef("UDP recv returned %i.", receive_count));
	}
	return (receive_count);
}

void DualSocket::TryAddReceiverSocket() {
	// TODO: add TCP receiver socket...
	if (udp_socket_->NeedInputPeek()) {
		mux_socket_->AddUdpReceiverSocket(udp_socket_);
	}
}

DualSocket::~DualSocket() {
	log_trace("~DualSocket()");
	ClearAll();
	mux_socket_ = 0;
}

TcpVSocket* DualSocket::GetTcpSocket() const {
	return (tcp_socket_);
}

UdpVSocket* DualSocket::GetUdpSocket() const {
	return (udp_socket_);
}



const double TcpMuxSocket::DEFAULT_CONNECT_ID_TIMEOUT = 2.0;
const double DualMuxSocket::DEFAULT_CONNECT_DUAL_TIMEOUT = 5.0;

loginstance(kNetwork, TcpMuxSocket);
loginstance(kNetwork, TcpVSocket);
loginstance(kNetwork, DualMuxSocket);
loginstance(kNetwork, DualSocket);



}
