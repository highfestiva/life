
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "cyclicarray.h"
#include "datagram.h"
#include "hashset.h"
#include "hashtable.h"
#include "inputstream.h"
#include "outputstream.h"
#include "socketaddress.h"
#include "thread.h"

#ifdef LEPRA_WINDOWS
typedef intptr_t s_socket;
#else // POSIX
typedef int s_socket;
#endif // Windows / Posix



namespace lepra {



class TcpSocket;


// Base class for all sockets.
class SocketBase {
public:
	enum {
		kBufferSize = 1024,
	};

	enum ShutdownFlag {
		kShutdownNone = 0,
		kShutdownRecv = 1,
		kShutdownSend = 2,
		kShutdownBoth  = (kShutdownRecv|kShutdownSend)
	};

	static s_socket InitSocket(s_socket socket, int size, bool reuse);
	static s_socket CreateTcpSocket();
	static s_socket CreateUdpSocket();
	static void CloseSysSocket(s_socket socket);

	SocketBase(s_socket socket = ~0);	// INVALID_SOCKET
	virtual ~SocketBase();
	void Close();
	void CloseKeepHandle();
	bool IsOpen() const;
	s_socket GetSysSocket() const;

	void MakeBlocking();
	void MakeNonBlocking();

	void Shutdown(ShutdownFlag how);

	int ClearErrors() const;

	uint64 GetSentByteCount() const;
	uint64 GetReceivedByteCount() const;

protected:
	s_socket socket_;
	uint64 sent_byte_count_;
	uint64 received_byte_count_;
};


// Base class for socket classes with an Id.
class ConnectionWithId {
public:
	ConnectionWithId();
	virtual ~ConnectionWithId();

	void SetConnectionId(const std::string& connection_id);
	void ClearConnectionId();
	const std::string& GetConnectionId() const;

private:
	std::string connection_id_;
};



class BufferedIo;

// Base class of TcpMuxSocket and UdpMuxSocket.
class MuxIo {
public:
	MuxIo(unsigned max_pending_connection_count, unsigned max_connection_count);
	virtual ~MuxIo();

	void AddSender(BufferedIo* sender);
	void RemoveSenderNoLock(BufferedIo* sender);
	bool IsSender(BufferedIo* sender) const;

	void AddReceiver(BufferedIo* receiver);
	void AddReceiverNoLock(BufferedIo* receiver);
	void RemoveReceiverNoLock(BufferedIo* receiver);
	bool IsReceiverNoLock(BufferedIo* receiver) const;

	virtual void ReleaseSocketThreads();

protected:
	BufferedIo* PopSender();
	BufferedIo* PopReceiver();

	typedef HashSet<IPAddress, IPAddress> IPSet;

	mutable Lock io_lock_;
	IPSet banned_ip_table_;
	Semaphore accept_semaphore_;

	const unsigned max_pending_connection_count_;
	const unsigned max_connection_count_;

	static const char connection_string_[27];
	static const uint8 acception_string_[15];

private:
	typedef std::unordered_set<BufferedIo*, LEPRA_VOIDP_HASHER> IoSet;

	IoSet sender_set_;
	IoSet receiver_set_;
};



// Base class of UdpVSocket and TcpVSocket. In the output case (as a "sender") it
// is used to fill a datagram with packets until there is no room left (max limit
// is defined by UdpMuxSocket). When the buffer is full it will be sent by calling
// SendBuffer(). This reduces overhead in both UDP and TCP.
class BufferedIo {
public:
	BufferedIo();
	virtual ~BufferedIo();

	void ClearOutputData();
	Datagram& GetSendBuffer() const;
	IOError AppendSendBuffer(const void* data, int length);
	bool HasSendData() const;

	// Sets/Gets in_send_buffer_. Used to keep track on whether this is added
	// to MuxIo's sender list.
	void SetInSenderList(bool in_send_buffer);
	bool GetInSenderList() const;

	virtual int SendBuffer() = 0;

protected:
	MuxIo* mux_io_;
	Datagram send_buffer_;
	bool in_send_buffer_;
};



class DatagramReceiver {
public:
	virtual int Receive(TcpSocket* socket, void* buffer, int max_size) = 0;
};



//
// A socket that listens to port specified in the constructor, on the interface
// defined by the given IP-address. A "TcpServerSocket" using Java language...
//
class TcpListenerSocket: public SocketBase {
public:
	friend class TcpSocket;

	TcpListenerSocket(const SocketAddress& local_address, bool is_server);
	virtual ~TcpListenerSocket();

	// Waits for connection, and returns the connected socket.
	// If this is a non-blocking socket and there are no pending connections,
	// the returned pointer will be NULL.
	TcpSocket* Accept();

	uint16 GetPort() const;
	const SocketAddress& GetLocalAddress() const;
	unsigned GetConnectionCount() const;

	void SetDatagramReceiver(DatagramReceiver* receiver);

protected:
	DatagramReceiver* GetDatagramReceiver() const;
	typedef TcpSocket* (*SocketFactory)(s_socket, const SocketAddress&, TcpListenerSocket*, DatagramReceiver*);
	TcpSocket* Accept(SocketFactory socket_factory);

private:
	static TcpSocket* CreateSocket(s_socket socket, const SocketAddress& target_address,
		TcpListenerSocket* server_socket, DatagramReceiver* receiver);

	void DecNumConnections();

	int connection_count_;
	SocketAddress local_address_;
	DatagramReceiver* receiver_;

	logclass();
};


// A regular Tcp socket.
class TcpSocket: public SocketBase {
public:
	TcpSocket(DatagramReceiver* receiver);
	TcpSocket(const SocketAddress& local_address);
	virtual ~TcpSocket();

	bool Connect(const SocketAddress& target_address);
	void Disconnect();

	// Nagle's algorithm is used in the Tcp protocol to reduce overhead when
	// resending small amounts of data - usually single keystrokes in telnet.
	bool DisableNagleAlgo();

	const SocketAddress& GetTargetAddress() const;

	int Send(const void* data, int size);
	int Receive(void* data, int max_size);
	int Receive(void* data, int max_size, double timeout);
	bool Unreceive(void* data, int byte_count);

	void SetDatagramReceiver(DatagramReceiver* receiver);
	int ReceiveDatagram(void* data, int max_size);

protected:
	friend class TcpListenerSocket;

	TcpSocket(s_socket socket, const SocketAddress& target_address, TcpListenerSocket* server_socket, DatagramReceiver* receiver);

private:
	DatagramReceiver* receiver_;
	uint8 unreceived_array_[16];
	int unreceived_byte_count_;
	SocketAddress target_address_;
	TcpListenerSocket* server_socket_;

	logclass();
};



//
// Represents a regular UDP socket, which is inconvenient to use compared to
// the next class below...
//
class UdpSocket: public SocketBase {
public:
	UdpSocket(const SocketAddress& local_address, bool is_server);
	UdpSocket(const UdpSocket& socket);
	virtual ~UdpSocket();

	const SocketAddress& GetLocalAddress() const;

	virtual int SendTo(const uint8* data, unsigned size, const SocketAddress& target_address);
	virtual int ReceiveFrom(uint8* data, unsigned max_size, SocketAddress& source_address);
	virtual int ReceiveFrom(uint8* data, unsigned max_size, SocketAddress& source_address, double timeout);

private:
	SocketAddress local_address_;

	logclass();
};



//
// A multiplexing UDP socket, which routes the incoming datagrams to
// the correct virtual socket (UdpVSocket).
//

class UdpVSocket;

class UdpMuxSocket: public MuxIo, protected Thread, public UdpSocket {
public:
	friend class UdpVSocket;

	UdpMuxSocket(const str& name, const SocketAddress& local_address, bool is_server,
		unsigned max_pending_connection_count = 16, unsigned max_connection_count = 1024);
	virtual ~UdpMuxSocket();

	// The sockets returned by these functions should be released using
	// CloseSocket(). Do not delete the socket!
	UdpVSocket* Connect(const SocketAddress& target_address, const std::string& connection_id, double timeout);
	UdpVSocket* Accept();
	UdpVSocket* PollAccept();
	void CloseSocket(UdpVSocket* socket);
	unsigned GetConnectionCount() const;

	UdpVSocket* PopReceiverSocket();
	UdpVSocket* PopSenderSocket();
	UdpVSocket* GetVSocket(const SocketAddress& target_address);

	bool SendOpenFirewallData(const SocketAddress& target_address);

protected:
	void Run();

	void RecycleBuffer(Datagram* buffer);

private:
	typedef HashTable<SocketAddress, UdpVSocket*, SocketAddress> SocketTable;
	typedef std::list<UdpVSocket*> SocketList;

	typedef FastAllocator<UdpVSocket> SocketAllocator;
	typedef FastAllocator<Datagram> BufferAllocator;

	SocketAllocator socket_allocator_;
	BufferAllocator buffer_allocator_;

	SocketTable socket_table_;
	SocketTable accept_table_;
	SocketList accept_list_;

	static const uint8 open_firewall_string_[27];

	logclass();
};



// A virtual socket; data is delegated here (upon receive) from the MUX
// socket, and passed through the MUX socket (on send).
class UdpVSocket: public BufferedIo, public InputStream, public OutputStream, public ConnectionWithId {
public:
	UdpVSocket();
	virtual ~UdpVSocket();
	void Init(UdpMuxSocket& socket, const SocketAddress& target_address, const std::string& connection_id);

	void ClearAll();

	int Receive(bool safe, void* data, int length);	// ::recv() return value.
	int Receive(void* data, int length);	// ::recv() return value.
	int SendBuffer();	// ::send() return value.
	int DirectSend(const void* data, int length);	// Writes buffer, flushes.

	const SocketAddress& GetLocalAddress() const;
	const SocketAddress& GetTargetAddress() const;

	void TryAddReceiverSocket();
	void AddInputBuffer(Datagram* buffer);
	bool NeedInputPeek() const;
	void SetReceiverFollowupActive(bool active);

	bool WaitAvailable(double time);

	// Stream interface.
	void Close();
	int64 GetAvailable() const;
	IOError ReadRaw(void* data, size_t length);
	IOError Skip(size_t length);
	IOError WriteRaw(const void* data, size_t length);
	void Flush();

	void SetSafeSend(bool);

private:
	static const int MAX_INPUT_BUFFERS = 32;

	typedef CyclicArray<Datagram*, MAX_INPUT_BUFFERS> BufferList;

	SocketAddress target_address_;

	mutable Lock lock_;

	BufferList receive_buffer_list_;
	unsigned raw_read_buffer_index_;
	bool receiver_followup_active_;

	logclass();
};



}
