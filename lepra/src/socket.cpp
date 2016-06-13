
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/socket.h"
#include "../include/fdset.h"
#include "../include/socketaddressgetter.h"
#include "../include/hashutil.h"
#include "../include/hirestimer.h"
#include "../include/resourcetracker.h"
#include "../include/systemmanager.h"
#include <algorithm>



namespace lepra {



s_socket SocketBase::InitSocket(s_socket _socket, int _size, bool reuse) {
	// Set the underlying socket buffer sizes.
	int buffer_size = _size;
	::setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&buffer_size, sizeof(buffer_size));
	buffer_size = _size;
	::setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&buffer_size, sizeof(buffer_size));
	linger __linger;
	__linger.l_onoff = 0;	// Graceful shutdown.
	__linger.l_linger = 1;	// Wait this many seconds.
	::setsockopt(_socket, SOL_SOCKET, SO_LINGER, (const char*)&__linger, sizeof(__linger));
	if (reuse) {
#ifndef LEPRA_WINDOWS
		int flag = 1;
		::setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, sizeof(flag));
#endif // !Windows
	}
	return (_socket);
}

s_socket SocketBase::CreateTcpSocket() {
	LEPRA_ACQUIRE_RESOURCE(Socket);
	s_socket s = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	return (InitSocket(s, 32*1024, true));
}

s_socket SocketBase::CreateUdpSocket() {
	LEPRA_ACQUIRE_RESOURCE(Socket);
	s_socket s = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	return (InitSocket(s, 8*1024, false));
}

void SocketBase::CloseSysSocket(s_socket _socket) {
#ifdef LEPRA_WINDOWS
	::closesocket(_socket);
#else // !LEPRA_WINDOWS
	::close(_socket);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
	LEPRA_RELEASE_RESOURCE(Socket);
}



SocketBase::SocketBase(s_socket _socket):
	socket_(_socket),
	sent_byte_count_(0),
	received_byte_count_(0) {
}

SocketBase::~SocketBase() {
	Close();
}

void SocketBase::Close() {
	CloseKeepHandle();
	socket_ = INVALID_SOCKET;
}

void SocketBase::CloseKeepHandle() {
	if (socket_ != INVALID_SOCKET) {
		Shutdown(kShutdownSend);
		CloseSysSocket(socket_);
	}
}

bool SocketBase::IsOpen() const {
	return (socket_ != INVALID_SOCKET);
}

s_socket SocketBase::GetSysSocket() const {
	return (socket_);
}

void SocketBase::MakeBlocking() {
#ifdef LEPRA_WINDOWS
	u_long non_blocking = 0;
	::ioctlsocket(socket_, FIONBIO, &non_blocking);
#elif defined LEPRA_POSIX
	::fcntl(socket_, F_SETFL, 0);
#else // !LEPRA_WINDOWS
#error "Not implemented!"
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
}

void SocketBase::MakeNonBlocking() {
#ifdef LEPRA_WINDOWS
	u_long non_blocking = 1;
	::ioctlsocket(socket_, FIONBIO, &non_blocking);
#elif defined LEPRA_POSIX
	::fcntl(socket_, F_SETFL, O_NONBLOCK);
#else // !LEPRA_WINDOWS
#error "Not implemented!"
#endif // LEPRA_WINDOWS /!LEPRA_WINDOWS
}

void SocketBase::Shutdown(ShutdownFlag how) {
	int _how = 0;
	switch (how) {
#ifdef LEPRA_WINDOWS
		case kShutdownRecv:	_how = SD_RECEIVE;	break;
		case kShutdownSend:	_how = SD_SEND;		break;
		case kShutdownBoth:	_how = SD_BOTH;		break;
#else // Posix
		case kShutdownRecv:	_how = SHUT_RD;		break;
		case kShutdownSend:	_how = SHUT_WR;		break;
		case kShutdownBoth:	_how = SHUT_RDWR;	break;
#endif // Win32 / Posix
	}
	::shutdown(socket_, (int)how);
}

int SocketBase::ClearErrors() const {
	int error = 0;
	int __size = sizeof(error);
	::getsockopt(socket_, SOL_SOCKET, SO_ERROR, (char*)&error, (socklen_t*)&__size);
	return error;
}

uint64 SocketBase::GetSentByteCount() const {
	return (sent_byte_count_);
}

uint64 SocketBase::GetReceivedByteCount() const {
	return (received_byte_count_);
}



ConnectionWithId::ConnectionWithId() {
}

ConnectionWithId::~ConnectionWithId() {
	ClearConnectionId();
}

void ConnectionWithId::SetConnectionId(const std::string& connection_id) {
	connection_id_ = connection_id;
}

void ConnectionWithId::ClearConnectionId() {
	connection_id_.clear();
}

const std::string& ConnectionWithId::GetConnectionId() const {
	return (connection_id_);
}



MuxIo::MuxIo(unsigned max_pending_connection_count, unsigned max_connection_count):
	max_pending_connection_count_(max_pending_connection_count),
	max_connection_count_(max_connection_count) {
}

MuxIo::~MuxIo() {
}

void MuxIo::AddSender(BufferedIo* sender) {
	ScopeLock lock(&io_lock_);
	sender_set_.insert(sender);
}

void MuxIo::RemoveSenderNoLock(BufferedIo* sender) {
	sender_set_.erase(sender);
}

bool MuxIo::IsSender(BufferedIo* sender) const {
	return (sender_set_.find(sender) != sender_set_.end());
}

void MuxIo::AddReceiver(BufferedIo* receiver) {
	ScopeLock lock(&io_lock_);
	receiver_set_.insert(receiver);
}

void MuxIo::AddReceiverNoLock(BufferedIo* receiver) {
	receiver_set_.insert(receiver);
}

void MuxIo::RemoveReceiverNoLock(BufferedIo* receiver) {
	receiver_set_.erase(receiver);
}

bool MuxIo::IsReceiverNoLock(BufferedIo* receiver) const {
	return (HashUtil::FindSetObject(receiver_set_, receiver) == receiver);
}

void MuxIo::ReleaseSocketThreads() {
	for (int x = 0; x < 10; ++x) {
		accept_semaphore_.Signal();
	}
}

BufferedIo* MuxIo::PopSender() {
	ScopeLock lock(&io_lock_);
	BufferedIo* _sender = 0;
	if (sender_set_.size() > 0) {
		_sender = *sender_set_.begin();
		sender_set_.erase(sender_set_.begin());
		_sender->SetInSenderList(false);
	}
	return (_sender);
}

BufferedIo* MuxIo::PopReceiver() {
	ScopeLock lock(&io_lock_);
	BufferedIo* _receiver = 0;
	if (receiver_set_.size() > 0) {
		_receiver = *receiver_set_.begin();
		receiver_set_.erase(receiver_set_.begin());
	}
	return (_receiver);
}

const char MuxIo::connection_string_[27] = "Hook me up, operator? I'm ";
const uint8 MuxIo::acception_string_[15] = "Join the club!";



BufferedIo::BufferedIo():
	mux_io_(0),
	in_send_buffer_(false) {
	ClearOutputData();
}

BufferedIo::~BufferedIo() {
}

void BufferedIo::ClearOutputData() {
	send_buffer_.Init();
}

Datagram& BufferedIo::GetSendBuffer() const {
	return ((Datagram&)send_buffer_);
}

IOError BufferedIo::AppendSendBuffer(const void* data, int _length) {
	IOError error = kIoOk;

	if (send_buffer_.data_size_ + _length < Datagram::kBufferSize) {
		if (!in_send_buffer_) {
			SetInSenderList(true);
			deb_assert(!mux_io_->IsSender(this));
			mux_io_->AddSender(this);
		} else {
			deb_assert(mux_io_->IsSender(this));
		}
		::memcpy(&send_buffer_.data_buffer_[send_buffer_.data_size_], data, _length);
		send_buffer_.data_size_ += (int)_length;
	} else {
		// It's all or nothing. If half a network packet arrives, we're fucked.
		SendBuffer();
		if (_length <= Datagram::kBufferSize) {
			error = AppendSendBuffer(data, _length);
		} else {
			error = kIoBufferOverflow;
		}
	}

	return (error);
}

bool BufferedIo::HasSendData() const {
	return (send_buffer_.data_size_ > 0);
}

void BufferedIo::SetInSenderList(bool in_send_buffer) {
	in_send_buffer_ = in_send_buffer;
}

bool BufferedIo::GetInSenderList() const {
	return (in_send_buffer_);
}



TcpListenerSocket::TcpListenerSocket(const SocketAddress& local_address, bool is_server):
	connection_count_(0),
	local_address_(local_address),
	receiver_(0) {
	log_trace("TcpListenerSocket()");

	// Initialize the socket.
	socket_ = CreateTcpSocket();

	if (socket_ != INVALID_SOCKET && is_server) {
		// Init socket address and bind it to the socket.
		if (::bind(socket_, SocketAddressGetter::GetRaw(local_address_), sizeof(RawSocketAddress)) == 0) {
			::listen(socket_, SOMAXCONN);
		} else {
			log_.Warning("Failed to bind TCP listener socket to "+local_address.GetAsString()+".");
			Close();
		}
	}
}

TcpListenerSocket::~TcpListenerSocket() {
	log_trace("~TcpListenerSocket()");
	receiver_ = 0;
}

TcpSocket* TcpListenerSocket::Accept() {
	return (Accept(&TcpListenerSocket::CreateSocket));
}

const SocketAddress& TcpListenerSocket::GetLocalAddress() const {
	return (local_address_);
}

unsigned TcpListenerSocket::GetConnectionCount() const {
	return (connection_count_);
}

void TcpListenerSocket::SetDatagramReceiver(DatagramReceiver* receiver) {
	receiver_ = receiver;
}

DatagramReceiver* TcpListenerSocket::GetDatagramReceiver() const {
	return (receiver_);
}

TcpSocket* TcpListenerSocket::Accept(SocketFactory socket_factory) {
	TcpSocket* tcp_socket = 0;
	if (socket_ != INVALID_SOCKET) {
		int accept_count = 0;
		while (IsOpen() && accept_count == 0) {
			FdSet accept_set;
			LEPRA_FD_ZERO(&accept_set);
#pragma warning(push)
#pragma warning(disable: 4127)	// MSVC warning: conditional expression is constant.
			LEPRA_FD_SET((sys_socket)socket_, &accept_set);
#pragma warning(pop)
			timeval time;
			time.tv_sec = 1;
			time.tv_usec = 0;
			accept_count = ::select((int)socket_+1, LEPRA_FDS(&accept_set), NULL, NULL, &time);
		}
		if (accept_count >= 1) {
			SocketAddress sock_address;
			socklen_t __size = (socklen_t)sizeof(RawSocketAddress);
			s_socket __socket = ::accept(socket_, SocketAddressGetter::GetRaw(sock_address), &__size);
			if (__socket != INVALID_SOCKET) {
				log_trace("::accept() received a ::connect()");
				tcp_socket = socket_factory(__socket, sock_address, this, receiver_);
				BusLock::Add(&connection_count_, 1);
			}
		}
	}
	return (tcp_socket);
}

TcpSocket* TcpListenerSocket::CreateSocket(s_socket _socket, const SocketAddress& target_address,
	TcpListenerSocket* server_socket, DatagramReceiver* receiver) {
	return (new TcpSocket(_socket, target_address, server_socket, receiver));
}

void TcpListenerSocket::DecNumConnections() {
	BusLock::Add(&connection_count_, -1);
}

loginstance(kNetwork, TcpListenerSocket);



TcpSocket::TcpSocket(DatagramReceiver* receiver):
	receiver_(receiver),
	unreceived_byte_count_(0),
	target_address_(),
	server_socket_(0) {
	socket_ = CreateTcpSocket();
}

TcpSocket::TcpSocket(const SocketAddress& local_address):
	receiver_(0),
	unreceived_byte_count_(0),
	target_address_(),
	server_socket_(0) {
	socket_ = CreateTcpSocket();

	if (socket_ != INVALID_SOCKET) {
		if (::bind(socket_, SocketAddressGetter::GetRaw(local_address), sizeof(RawSocketAddress)) != 0) {
			log_.Warningf("TCP socket binding failed! Error=%i", SOCKET_LAST_ERROR());
			Close();
		}
	}
}

TcpSocket::TcpSocket(s_socket _socket, const SocketAddress& target_address,
	TcpListenerSocket* server_socket, DatagramReceiver* receiver):
	SocketBase(_socket),
	receiver_(receiver),
	unreceived_byte_count_(0),
	target_address_(target_address),
	server_socket_(server_socket) {
	log_trace("TcpSocket()");
	InitSocket(socket_, 32*1024, false);
}

TcpSocket::~TcpSocket() {
	log_trace("~TcpSocket()");
	Disconnect();
	receiver_ = 0;
}

bool TcpSocket::Connect(const SocketAddress& target_address) {
	target_address_ = target_address;
	bool ok = (::connect(socket_, SocketAddressGetter::GetRaw(target_address_), sizeof(RawSocketAddress)) != SOCKET_ERROR);
	if (!ok) {
		int e = SOCKET_LAST_ERROR();
		log_.Infof("TCP connect failed! Error=%i.", e);
		Close();
	}
	return (ok);
}

void TcpSocket::Disconnect() {
	if (server_socket_ != 0) {
		server_socket_->DecNumConnections();
		server_socket_ = 0;
	}

	Close();
}

bool TcpSocket::DisableNagleAlgo() {
	// Disable the Nagle algorithm.
	int flag = 1;
	int result = setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
	return (result == 0);
}

int TcpSocket::Send(const void* data, int _size) {
	int sent_byte_count = 0;
	if (socket_ != INVALID_SOCKET && data != 0 && _size > 0) {
		sent_byte_count = ::send(socket_, (const char*)data, _size, 0);
		// Did we get disconnected?
		if (sent_byte_count <= 0) {
			int e = SOCKET_LAST_ERROR();
			log_.Errorf("TCP send error. Error=%i, socket=%u.", e, socket_);
			CloseKeepHandle();
		} else {
			log_volatile(str _local_address);
			log_volatile(if (server_socket_) _local_address = server_socket_->GetLocalAddress().GetAsString());
			log_volatile(str _data = strutil::DumpData((uint8*)data, std::min((int)_size, 50)));
			log_volatile(str __s = strutil::ReplaceCtrlChars((const char*)data, '.'));
			log_volatile(__s.resize(15));
			log_volatile(log_.Tracef("TCP -> %u bytes (%s -> %s): %s %s.", _size,
				_local_address.c_str(), target_address_.GetAsString().c_str(), _data.c_str(), __s.c_str()));

			sent_byte_count_ += sent_byte_count;
		}
	}
	return (sent_byte_count);
}

int TcpSocket::Receive(void* data, int max_size) {
	int __size = -1;
	if (socket_ != INVALID_SOCKET) {
		if (max_size == unreceived_byte_count_) {
			::memcpy(data, unreceived_array_, max_size);
			unreceived_byte_count_ = 0;
			__size = (int)max_size;
		} else if (unreceived_byte_count_ == 0) {
			__size = ::recv(socket_, (char*)data, (int)max_size, 0);
		} else {
			log_.Error("Tried to ::recv() data, but unreceived data size don't match!");
			__size = 0;
		}
		if (__size == 0) {
			// Disconnected.
			CloseKeepHandle();
			__size = -1;
		} else if(__size == SOCKET_ERROR) {
			// There's no data...
			__size = 0;
		} else {
			log_volatile(str _local_address);
			log_volatile(if (server_socket_) _local_address = server_socket_->GetLocalAddress().GetAsString());
			log_volatile(str _data = strutil::DumpData((uint8*)data, std::min(__size, 50)));
			log_volatile(str __s = strutil::ReplaceCtrlChars((const char*)data, '.'));
			log_volatile(__s.resize(15));
			log_volatile(log_.Tracef("TCP <- %u bytes (%s <- %s): %s %s.", __size,
				_local_address.c_str(), target_address_.GetAsString().c_str(), _data.c_str(), __s.c_str()));

			received_byte_count_ += __size;
		}
	}
	return (__size);
}

int TcpSocket::Receive(void* data, int max_size, double timeout) {
	if (timeout < 0) {
		timeout = 0;
	}
	FdSet accept_set;
	LEPRA_FD_ZERO(&accept_set);
#pragma warning(push)
#pragma warning(disable: 4127)	// MSVC warning: conditional expression is constant.
	LEPRA_FD_SET((sys_socket)socket_, &accept_set);
#pragma warning(pop)
	timeval time;
	time.tv_sec = (long)timeout;
	time.tv_usec = (long)((timeout-time.tv_sec) * 1000000);
	int read_count = ::select((int)socket_+1, LEPRA_FDS(&accept_set), NULL, NULL, &time);
	if (read_count == 1) {
		return Receive(data, max_size);
	}
	return 0;
}

bool TcpSocket::Unreceive(void* data, int byte_count) {
	deb_assert(unreceived_byte_count_ == 0);
	deb_assert(byte_count <= (int)sizeof(unreceived_array_));
	bool ok = (unreceived_byte_count_ == 0 && byte_count <= (int)sizeof(unreceived_array_));
	if (ok) {
		unreceived_byte_count_ = byte_count;
		::memcpy(unreceived_array_, data, byte_count);
	}
	if (!ok) {
		log_.Errorf("Unable to unreceive %u bytes.", byte_count);
	}
	return (ok);
}

void TcpSocket::SetDatagramReceiver(DatagramReceiver* receiver) {
	receiver_ = receiver;
}

int TcpSocket::ReceiveDatagram(void* data, int max_size) {
	int received_byte_count;
	if (receiver_) {
		received_byte_count = receiver_->Receive(this, data, max_size);
	} else {
		received_byte_count = Receive(data, max_size);
	}
	return (received_byte_count);
}

const SocketAddress& TcpSocket::GetTargetAddress() const {
	return (target_address_);
}

loginstance(kNetwork, TcpSocket);



UdpSocket::UdpSocket(const SocketAddress& local_address, bool is_server):
	local_address_(local_address) {
	// Initialize UDP socket.
	socket_ = CreateUdpSocket();

	if (socket_ != INVALID_SOCKET && is_server) {
		if (::bind(socket_, SocketAddressGetter::GetRaw(local_address_), sizeof(RawSocketAddress)) != 0) {
			log_.Warningf("Failed to bind UDP socket to %s: %i.", local_address.GetAsString().c_str(), SOCKET_LAST_ERROR());
			Close();
			local_address_.Set(IPAddress(), 0);
		}
	}
}

UdpSocket::UdpSocket(const UdpSocket& _socket):
	local_address_(_socket.local_address_) {
	log_trace("UdpSocket()");
}

UdpSocket::~UdpSocket() {
	log_trace("~UdpSocket()");
}

const SocketAddress& UdpSocket::GetLocalAddress() const {
	return (local_address_);
}

int UdpSocket::SendTo(const uint8* data, unsigned _size, const SocketAddress& target_address) {
	int sent_byte_count = 0;
	if (socket_ != INVALID_SOCKET && data != 0 && _size > 0) {
		sent_byte_count = ::sendto(socket_, (const char*)data, _size, 0, SocketAddressGetter::GetRaw(target_address), sizeof(RawSocketAddress));
		if (sent_byte_count != (int)_size) {
			CloseKeepHandle();
		} else {
			/*log_volatile(str _data = strutil::DumpData((uint8*)data, std::min(_size, (unsigned)50)));
			log_volatile(log_.Tracef("UDP -> %u bytes (%s -> %s: %s."), _size,
				local_address_.GetAsString().c_str(), target_address.GetAsString().c_str(),
				_data.c_str()));*/
			sent_byte_count_ += sent_byte_count;
		}
	}

	return (sent_byte_count);
}

int UdpSocket::ReceiveFrom(uint8* data, unsigned max_size, SocketAddress& source_address) {
	int __size = 0;
	if (socket_ != INVALID_SOCKET && data != 0 && max_size > 0) {
		socklen_t addr_size = (socklen_t)sizeof(RawSocketAddress);
		__size = ::recvfrom(socket_, (char*)data, max_size, 0, SocketAddressGetter::GetRaw(source_address), &addr_size);
		if (__size <= 0) {
			// Disconnected.
			__size = -1;
		} else {
			/*log_volatile(str _data = strutil::DumpData((uint8*)data, std::min(__size, 50)));
			log_volatile(log_.Tracef("UDP <- %u bytes (%s <- %s: %s."), __size,
				local_address_.GetAsString().c_str(), source_address.GetAsString().c_str(),
				_data.c_str()));*/
			received_byte_count_ += __size;
		}
	}
	return (__size);
}

int UdpSocket::ReceiveFrom(uint8* data, unsigned max_size, SocketAddress& source_address, double timeout) {
	if (timeout < 0) {
		timeout = 0;
	}
	FdSet read_set;
	LEPRA_FD_ZERO(&read_set);
#pragma warning(push)
#pragma warning(disable: 4127)	// MSVC warning: conditional expression is constant.
	LEPRA_FD_SET((sys_socket)socket_, &read_set);
#pragma warning(pop)
	timeval time;
	time.tv_sec = (long)timeout;
	time.tv_usec = (long)((timeout-time.tv_sec) * 1000000);
	int read_count = ::select((int)socket_+1, LEPRA_FDS(&read_set), NULL, NULL, &time);
	if (read_count == 1) {
		return ReceiveFrom(data, max_size, source_address);
	}
	return 0;
}

loginstance(kNetwork, UdpSocket);



UdpMuxSocket::UdpMuxSocket(const str& name, const SocketAddress& local_address, bool is_server,
	unsigned max_pending_connection_count, unsigned max_connection_count):
	MuxIo(max_pending_connection_count, max_connection_count),
	Thread(name+"UdpMuxRecv "+local_address.GetAsString()),
	UdpSocket(local_address, is_server) {
	log_trace("UdpMuxSocket()");

	if (IsOpen()) {
		Start();
	}
}

UdpMuxSocket::~UdpMuxSocket() {
	log_trace("~UdpMuxSocket()");

	RequestStop();

	const SocketAddress& address = GetLocalAddress();
	if (address.GetPort()) {
		s_socket killer = CreateUdpSocket();
		const int release_byte_count = 8;
		::sendto(killer, "Release!", release_byte_count, 0, SocketAddressGetter::GetRaw(address), sizeof(RawSocketAddress));
		//Thread::Sleep(0.01);
		//CloseSysSocket(killer);
	}

	Close();
	ReleaseSocketThreads();
	Signal(0);
	Join(5.0f);
}

UdpVSocket* UdpMuxSocket::Connect(const SocketAddress& target_address, const std::string& connection_id, double timeout) {
	UdpVSocket* __socket = 0;
	{
		ScopeLock lock(&io_lock_);
		SocketTable::Iterator iter;
		iter = socket_table_.Find(target_address);
		if (iter != socket_table_.End()) {
			__socket = *iter;
			if (__socket->GetTargetAddress() != target_address) {
				deb_assert(false);
				__socket = 0;
			}
		} else {
			__socket = socket_allocator_.Alloc();
			__socket->Init(*this, target_address, connection_id);
			socket_table_.Insert(target_address, __socket);
			std::string connect_string(connection_string_, sizeof(connection_string_));
			connect_string += connection_id;
			if (__socket->DirectSend(connect_string.c_str(), (int)connect_string.length()) != (int)connect_string.length()) {
				log_.Error("Send to server (as connect) failed.");
				CloseSocket(__socket);
				__socket = 0;
			}
		}
	}
	if (__socket) {
		__socket->WaitAvailable(timeout);
		Datagram _buffer;
		_buffer.data_size_ = __socket->Receive(_buffer.data_buffer_, sizeof(_buffer.data_buffer_));
		if (_buffer.data_size_ != sizeof(acception_string_) || ::memcmp(acception_string_, _buffer.data_buffer_, sizeof(acception_string_)) != 0) {
			if (_buffer.data_size_ == 0) {
				log_debug("Remote end seems dead. Firewall?");
			} else if (_buffer.data_size_ < 0) {
				log_.Error("Connect was refused. Firewall?");
			} else if (_buffer.data_size_ > 0) {
				log_.Error("Connect was replied to with jibberish. Wassup?");
			}
			CloseSocket(__socket);
			__socket = 0;
		} else {
			log_trace("Connect went through!");
			__socket->SetReceiverFollowupActive(true);
		}
	}
	return (__socket);
}

UdpVSocket* UdpMuxSocket::Accept() {
	// Leave this function unlocked, since we are not accessing any critical data.
	UdpVSocket* __socket = 0;
	while (IsOpen() && (__socket = PollAccept()) == 0) {
		accept_semaphore_.Wait();
	}
	return (__socket);
}

UdpVSocket* UdpMuxSocket::PollAccept() {
	UdpVSocket* __socket = 0;
	ScopeLock lock(&io_lock_);
	if (accept_list_.empty() == false) {
		// Move the socket from the accept list & table to the socket table.
		__socket = accept_list_.front();
		accept_list_.pop_front();
		__socket->SetReceiverFollowupActive(true);
		accept_table_.Remove(__socket->GetTargetAddress());
		if (__socket->DirectSend(acception_string_, sizeof(acception_string_)) == sizeof(acception_string_)) {
			log_trace("Replied to connect with an ACK.");
			socket_table_.Insert(__socket->GetTargetAddress(), __socket);
		} else {
			log_.Error("Could not reply to connect with an ACK.");
			// TODO: blacklist after x number of incorrect queries?
			CloseSocket(__socket);
			__socket = 0;
		}
	}

	return (__socket);
}

void UdpMuxSocket::CloseSocket(UdpVSocket* _socket) {
	log_volatile(log_.Debugf("Dropping UDP MUX socket %s.", _socket->GetTargetAddress().GetAsString().c_str()));

	ScopeLock lock(&io_lock_);
	socket_table_.Remove(_socket->GetTargetAddress());
	RemoveSenderNoLock(_socket);
	RemoveReceiverNoLock(_socket);

	_socket->ClearAll();

	socket_allocator_.Free(_socket);
}

unsigned UdpMuxSocket::GetConnectionCount() const {
	return (socket_table_.GetCount());
}

UdpVSocket* UdpMuxSocket::PopReceiverSocket() {
	UdpVSocket* __socket = (UdpVSocket*)PopReceiver();
	return (__socket);
}

UdpVSocket* UdpMuxSocket::PopSenderSocket() {
	UdpVSocket* __socket = (UdpVSocket*)PopSender();
	if (__socket) {
		log_trace("Popped UDP sender socket.");
	}
	return (__socket);
}

UdpVSocket* UdpMuxSocket::GetVSocket(const SocketAddress& target_address) {
	ScopeLock lock(&io_lock_);
	return socket_table_.FindObject(target_address);
}

bool UdpMuxSocket::SendOpenFirewallData(const SocketAddress& target_address) {
	return SendTo(open_firewall_string_, sizeof(open_firewall_string_), target_address) == sizeof(open_firewall_string_);
}

void UdpMuxSocket::Run() {
	log_trace("Receive thread running");

	SocketAddress _source_address;
	Datagram* _buffer = 0;

	while (IsOpen() && !GetStopRequest()) {
		if (!_buffer) {
			// Allocate a new buffer in which we store the received data.
			ScopeLock lock(&io_lock_);
			_buffer = buffer_allocator_.Alloc();
		}
		_buffer->Init();

		// Wait for data. This will block the thread forever if no data arrives.
		_buffer->data_size_ = ReceiveFrom(_buffer->data_buffer_, kBufferSize, _source_address);
		if (_buffer->data_size_ > 0) {
			ScopeLock lock(&io_lock_);

			// Check if the IP is already 'connected'.
			SocketTable::Iterator iter = socket_table_.Find(_source_address);
			if (iter != socket_table_.End()) {
				// Already connected.
				UdpVSocket* __socket = *iter;
				__socket->AddInputBuffer(_buffer);
				_buffer = 0;
			} else if(banned_ip_table_.Find(_source_address.GetIP()) != banned_ip_table_.End()) {
				log_.Warning("Banned socket sent us something. Playing ignorant.");
			} else if(accept_table_.Find(_source_address) == accept_table_.End()) {
				// Look for "VSocket connect magic".
				if (_buffer->data_size_ >= (int)sizeof(connection_string_) &&
					::memcmp(_buffer->data_buffer_, connection_string_, sizeof(connection_string_)-1) == 0) {
					std::string _connection_id((const char*)&_buffer->data_buffer_[sizeof(connection_string_)-1], _buffer->data_size_+1-sizeof(connection_string_));
					unsigned accept_count = (unsigned)accept_list_.size();
					unsigned socket_count = (unsigned)socket_table_.GetCount();
					// TODO: drop old acceptance socket instead of new?
					if (accept_count < max_pending_connection_count_ &&
						(accept_count + socket_count) < max_connection_count_) {
						UdpVSocket* __socket = socket_allocator_.Alloc();
						__socket->Init(*this, _source_address, _connection_id);

						accept_list_.push_back(__socket);
						accept_table_.Insert(_source_address, __socket);

						accept_semaphore_.Signal();
					} else {
						log_.Warning("Too many sockets - didn't accept connect.");
					}
				} else if (_buffer->data_size_ == sizeof(open_firewall_string_) &&
					::memcmp(_buffer->data_buffer_, open_firewall_string_, sizeof(open_firewall_string_)) == 0) {
					log_trace("Received an \"open firewall\" datagram.");
				} else {
					log_.Warning("Non-connected socket sent us junk.");
					log_volatile(const str _data = strutil::DumpData(_buffer->data_buffer_, std::min(_buffer->data_size_, 50)));
					log_volatile(log_.Debugf("UDP <- %i bytes (%s): %s.", _buffer->data_size_,
						_source_address.GetAsString().c_str(), _data.c_str()));
				}
			} else {
				log_.Warning("Non-connecting send us something. (Could mean internal error...)");
			}
		} else if (_buffer->data_size_ < 0) {
			// Socket died.
		} else {
			log_.Warning("Could not receive any data on the socket.");
		}
	}

	if (_buffer) {
		RecycleBuffer(_buffer);
	}
}

void UdpMuxSocket::RecycleBuffer(Datagram* buffer) {
	ScopeLock lock(&io_lock_);
	buffer_allocator_.Free(buffer);
}

const uint8 UdpMuxSocket::open_firewall_string_[27] = "Aaaarglebargle glop-glyph!";

loginstance(kNetwork, UdpMuxSocket);



UdpVSocket::UdpVSocket() {
	//log_trace("UdpVSocket()");
	ClearAll();
}

UdpVSocket::~UdpVSocket() {
	//log_trace("~UdpVSocket()");
}

void UdpVSocket::ClearAll() {
	if (mux_io_) {
		SendBuffer();
	}

	target_address_ = SocketAddress();
	while (receive_buffer_list_.GetCount() != 0) {
		Receive(0, 0);
	}
	mux_io_ = 0;
	receive_buffer_list_.Clear();
	raw_read_buffer_index_ = 0;
	ClearOutputData();
	SetInSenderList(false);
	receiver_followup_active_ = false;
}

void UdpVSocket::Init(UdpMuxSocket& _socket, const SocketAddress& target_address, const std::string& connection_id) {
	ClearAll();

	mux_io_ = &_socket;
	target_address_ = target_address;
	SetConnectionId(connection_id);
}

int UdpVSocket::Receive(bool, void* data, int _length) {
	return Receive(data, _length);
}

int UdpVSocket::Receive(void* data, int _length) {
	int read_size = 0;
	if (receive_buffer_list_.GetCount() > 0) {
		// Optimizing, deadlock avoiding code.
		Datagram* receive_buffer = 0;
		{
			ScopeLock lock(&lock_);
			if (receive_buffer_list_.GetCount() > 0) {
				receive_buffer = receive_buffer_list_[0];
				receive_buffer_list_.PopFront();
			}
		}

		if (receive_buffer) {
			read_size = receive_buffer->data_size_;
			if (receive_buffer->data_size_ > 0) {
				read_size = (int)std::min(receive_buffer->data_size_, (int)_length);
				::memcpy(data, receive_buffer->data_buffer_, read_size);
			}
			((UdpMuxSocket*)mux_io_)->RecycleBuffer(receive_buffer);
		}
	}
	return (read_size);
}

int UdpVSocket::SendBuffer() {
	int send_result = 0;
	if (send_buffer_.data_size_ > 0) {
		send_result = ((UdpMuxSocket*)mux_io_)->SendTo(send_buffer_.data_buffer_, send_buffer_.data_size_, target_address_);
		send_buffer_.Init();
	}
	return (send_result);
}

int UdpVSocket::DirectSend(const void* data, int _length) {
	return (((UdpMuxSocket*)mux_io_)->SendTo((const uint8*)data, _length, target_address_));
}

const SocketAddress& UdpVSocket::GetLocalAddress() const {
	return (((UdpMuxSocket*)mux_io_)->GetLocalAddress());
}

const SocketAddress& UdpVSocket::GetTargetAddress() const {
	return (target_address_);
}

void UdpVSocket::TryAddReceiverSocket() {
	if (NeedInputPeek()) {
		((UdpMuxSocket*)mux_io_)->AddReceiver(this);
	}
}

void UdpVSocket::AddInputBuffer(Datagram* buffer) {
	int pre_insert_count;
	{
		ScopeLock lock(&lock_);
		pre_insert_count = (int)receive_buffer_list_.GetCount();
		if (pre_insert_count < MAX_INPUT_BUFFERS) {
			receive_buffer_list_.PushBack(buffer);
		} else {
			// The array is full and we have to throw the data away. This should be ok
			// since UDP is unsafe per definition, and the user thread can't keep
			// up processing the data anyway.

			// Throw old data away.
			log_.Error("Throwing away network data, since receive buffer is full!");
			Datagram* receive_buffer = receive_buffer_list_[0];
			receive_buffer_list_.PopFront();
			lock_.Release();
			((UdpMuxSocket*)mux_io_)->RecycleBuffer(receive_buffer);
			lock_.Acquire();
			receive_buffer_list_.PushBack(buffer);
			raw_read_buffer_index_ = 0;
		}
	}

	// If this is our first input packet, we add us to the list of sockets.
	if (pre_insert_count == 0 && receiver_followup_active_) {
		((UdpMuxSocket*)mux_io_)->AddReceiver(this);
	}
}

bool UdpVSocket::NeedInputPeek() const {
	return (receive_buffer_list_.GetCount() != 0);
}

void UdpVSocket::SetReceiverFollowupActive(bool active) {
	receiver_followup_active_ = active;
}

bool UdpVSocket::WaitAvailable(double timeout) {
	const Thread* current_thread = Thread::GetCurrentThread();
	HiResTimer timer(false);
	while (GetAvailable() == 0 && timer.QueryTimeDiff() < timeout &&
		!current_thread->GetStopRequest() && !SystemManager::GetQuitRequest()) {
		Thread::Sleep(Math::Clamp(timeout*0.1, 0.001, 60*60.0));
	}
	return (GetAvailable() != 0);
}

// Stream interface-inherited dummy method.
void UdpVSocket::Close() {
}

int64 UdpVSocket::GetAvailable() const {
	int read_size = 0;
	ScopeLock lock(&lock_);
	if (receive_buffer_list_.GetCount() > 0) {
		Datagram* receive_buffer = receive_buffer_list_[0];
		read_size = receive_buffer->data_size_;
	}
	return (read_size);
}

IOError UdpVSocket::ReadRaw(void* data, size_t _length) {
	IOError result = kIoNoDataAvailable;
	ScopeLock lock(&lock_);
	while (receive_buffer_list_.GetCount() > 0) {
		Datagram* receive_buffer = receive_buffer_list_[0];
		if ((int)(raw_read_buffer_index_+_length) <= receive_buffer->data_size_) {
			::memcpy(data, &receive_buffer->data_buffer_[raw_read_buffer_index_], _length);
			if ((int)(raw_read_buffer_index_+_length) == receive_buffer->data_size_) {
				receive_buffer_list_.PopFront();
				raw_read_buffer_index_ = 0;
			} else {
				raw_read_buffer_index_ += (int)_length;
			}
			result = kIoOk;
			break;
		}
	}
	return (result);
}

IOError UdpVSocket::Skip(size_t /*_length*/) {
	IOError result = kIoNoDataAvailable;
	ScopeLock lock(&lock_);
	if (receive_buffer_list_.GetCount() > 0) {
		receive_buffer_list_.PopFront();
		result = kIoOk;
	}
	return (result);
}

IOError UdpVSocket::WriteRaw(const void* data, size_t _length) {
	return (AppendSendBuffer(data, (int)_length));
}

void UdpVSocket::Flush() {
	SendBuffer();
}

void UdpVSocket::SetSafeSend(bool) {
}

loginstance(kNetwork, UdpVSocket);



}
