
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../thirdparty/FastDelegate/FastDelegate.h"
#include "fdset.h"
#include "hirestimer.h"
#include "memberthread.h"
#include "orderedmap.h"
#include "socket.h"



namespace lepra {



class TcpVSocket;



class TcpMuxSocket: public MuxIo, public TcpListenerSocket {
public:
	friend class TcpVSocket;

	TcpMuxSocket(const str& name, const SocketAddress& local_address, bool is_server,
		unsigned max_pending_connection_count = 16, unsigned max_connection_count = 1024);
	virtual ~TcpMuxSocket();

	// The sockets returned by these functions should be released using
	// CloseSocket(). Do not delete the socket!
	TcpVSocket* Connect(const SocketAddress& target_address, const std::string& connection_id, double timeout);
	TcpVSocket* Accept();
	TcpVSocket* PollAccept();

	template<class _Base> void SetCloseCallback(_Base* callee, void (_Base::*method)(TcpVSocket*)) {
		close_dispatcher_ = SocketDispatcher(callee, method);
	};
	void DispatchCloseSocket(TcpVSocket* socket);
	void CloseSocket(TcpVSocket* socket, bool force_delete = false);

	TcpVSocket* PopReceiverSocket();
	TcpVSocket* PopSenderSocket();

	uint64 GetTotalSentByteCount() const;
	uint64 GetTotalReceivedByteCount() const;
	void AddTotalSentByteCount(unsigned byte_count);
	void AddTotalReceivedByteCount(unsigned byte_count);

	void SetConnectIdTimeout(double timeout);

private:
	enum AcceptStatus {
		kAcceptOk,
		kAcceptClose,
		kAcceptQueryWait,
	};

	static TcpSocket* CreateSocket(s_socket socket, const SocketAddress& target_address,
		TcpListenerSocket* server_socket, DatagramReceiver* receiver);
	void AddConnectedSocket(TcpVSocket* socket);
	bool RemoveConnectedSocketNoLock(TcpVSocket* socket);
	int BuildConnectedSocketSet(FdSet& socket_set);
	void PushReceiverSockets(const FdSet& socket_set);
	AcceptStatus QueryReceiveConnectString(TcpVSocket* socket);
	void ReleaseSocketThreads();
	void AcceptThreadEntry();
	void SelectThreadEntry();

	typedef OrderedMap<TcpVSocket*, HiResTimer, LEPRA_VOIDP_HASHER> SocketTimeMap;
	typedef std::unordered_map<s_socket, TcpVSocket*> SocketVMap;

	MemberThread<TcpMuxSocket> accept_thread_;
	MemberThread<TcpMuxSocket> select_thread_;
	static const double DEFAULT_CONNECT_ID_TIMEOUT;
	double connect_id_timeout_;
	SocketTimeMap pending_connect_id_map_;
	SocketVMap connected_socket_map_;
	bool active_receiver_map_changed_;
	FdSet backup_fd_set_;
	Semaphore connected_socket_semaphore_;
	int64 v_sent_byte_count_;
	int64 v_received_byte_count_;
	typedef fastdelegate::FastDelegate1<TcpVSocket*, void> SocketDispatcher;
	SocketDispatcher close_dispatcher_;

	logclass();
};

class TcpVSocket: public BufferedIo, public TcpSocket, public ConnectionWithId {
public:
	TcpVSocket(TcpMuxSocket& mux_socket, DatagramReceiver* receiver);
	TcpVSocket(s_socket socket, const SocketAddress& target_address, TcpMuxSocket& mux_socket, DatagramReceiver* receiver);
	virtual ~TcpVSocket();

	bool Connect(const SocketAddress& target_address, const std::string& connection_id, double timeout);

	int SendBuffer();	// ::send() return value.
	int Receive(void* data, int max_size, bool datagram = true);
	int Receive(void* data, int max_size, double timeout, bool datagram);

private:
	logclass();
};



class DualSocket;

//
// The dream of all game programmers! =)
// A multiplexing socket that works over both UDP and TCP at the same time, over only
// one socket address!
class DualMuxSocket {
public:
	DualMuxSocket(const str& name, const SocketAddress& local_address, bool is_server,
		unsigned max_pending_connection_count = 16, unsigned max_connection_count = 256);
	virtual ~DualMuxSocket();

	bool IsOpen() const;
	bool IsOpen(DualSocket* socket) const;

	DualSocket* Connect(const SocketAddress& target_address, const std::string& connection_id, double timeout);

	void Close();
	void CloseSocket(DualSocket* socket);

	template<class _Base> void SetCloseCallback(_Base* callee, void (_Base::*method)(DualSocket*)) {
		close_dispatcher_ = SocketDispatcher(callee, method);
	};
	void DispatchCloseSocket(DualSocket* socket);

	DualSocket* PollAccept();
	DualSocket* PopReceiverSocket();
	DualSocket* PopReceiverSocket(bool safe);
	DualSocket* PopSenderSocket();

	uint64 GetSentByteCount() const;
	uint64 GetSentByteCount(bool safe) const;
	uint64 GetReceivedByteCount() const;
	uint64 GetReceivedByteCount(bool safe) const;

	SocketAddress GetLocalAddress() const;
	unsigned GetConnectionCount() const;

	void SetSafeConnectIdTimeout(double timeout);
	void SetConnectDualTimeout(double timeout);
	void SetDatagramReceiver(DatagramReceiver* receiver);

protected:
	friend class DualSocket;

	void AddUdpReceiverSocket(UdpVSocket* socket);

private:
	void AddSocket(DualSocket* socket, TcpVSocket* tcp_socket, UdpVSocket* udp_socket);
	void KillNonDualConnected();

	void OnCloseTcpSocket(TcpVSocket* tcp_socket);

	typedef std::unordered_map<TcpVSocket*, DualSocket*, LEPRA_VOIDP_HASHER> TcpSocketMap;
	typedef std::unordered_map<UdpVSocket*, DualSocket*, LEPRA_VOIDP_HASHER> UdpSocketMap;
	typedef std::unordered_map<std::string, DualSocket*> IdSocketMap;
	typedef OrderedMap<DualSocket*, HiResTimer, LEPRA_VOIDP_HASHER> SocketTimeMap;

	mutable Lock lock_;
	TcpMuxSocket* tcp_mux_socket_;
	UdpMuxSocket* udp_mux_socket_;
	TcpSocketMap tcp_socket_map_;
	UdpSocketMap udp_socket_map_;
	IdSocketMap id_socket_map_;
	static const double DEFAULT_CONNECT_DUAL_TIMEOUT;
	double connect_dual_timeout_;
	SocketTimeMap pending_dual_connect_map_;
	bool pop_safe_toggle_;

	typedef fastdelegate::FastDelegate1<DualSocket*, void> SocketDispatcher;
	SocketDispatcher close_dispatcher_;

	template<class _MuxSocket, class _VSocket> class Connector: public Thread {
	public:
		Connector(const str& name, _MuxSocket* mux_socket, const SocketAddress& target_address,
			const std::string& connection_id, double timeout, Semaphore& semaphore):
			Thread(name),
			mux_socket_(mux_socket),
			socket_(0),
			target_address_(target_address),
			connection_id_(connection_id),
			timeout_(timeout),
			semaphore_(semaphore) {
		}
		void Run() {
			socket_ = mux_socket_->Connect(target_address_, connection_id_, timeout_);
			semaphore_.Signal();
		}
		_MuxSocket* mux_socket_;
		_VSocket* socket_;
		SocketAddress target_address_;
		std::string connection_id_;
		double timeout_;
		Semaphore& semaphore_;
	private:
		void operator=(const Connector&);
	};

	logclass();
};



// Sends all "safe" data through a TCP connection (guarantees ordered arrivals,
// also of large data chunks), everything else (positional info) over UDP.
class DualSocket: public ConnectionWithId {
public:
	DualSocket(DualMuxSocket* mux_socket, const std::string& _connection_id);

	bool SetSocket(TcpVSocket* socket);
	bool SetSocket(UdpVSocket* socket);
	bool IsOpen() const;
	void ClearAll();

	SocketAddress GetLocalAddress() const;
	SocketAddress GetTargetAddress() const;

	void ClearOutputData();
	void SetSafeSend(bool safe);
	Datagram& GetSendBuffer() const;
	Datagram& GetSendBuffer(bool safe) const;
	IOError AppendSendBuffer(const void* data, int length);
	IOError AppendSendBuffer(bool safe, const void* data, int length);
	int SendBuffer();	// ::send() and ::sendto() return values.
	bool HasSendData() const;

	void SetSafeReceive(bool safe);
	int Receive(void* data, int length);
	int Receive(bool safe, void* data, int length);

	void TryAddReceiverSocket();

protected:
	friend class DualMuxSocket;

	virtual ~DualSocket();

	TcpVSocket* GetTcpSocket() const;
	UdpVSocket* GetUdpSocket() const;


private:
	DualMuxSocket* mux_socket_;
	TcpVSocket* tcp_socket_;
	UdpVSocket* udp_socket_;
	bool default_safe_send_;
	bool default_safe_receive_;

	logclass();
};



}
