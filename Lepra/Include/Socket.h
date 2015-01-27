
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "CyclicArray.h"
#include "Datagram.h"
#include "HashSet.h"
#include "HashTable.h"
#include "InputStream.h"
#include "OutputStream.h"
#include "SocketAddress.h"
#include "Thread.h"

#ifdef LEPRA_WINDOWS
typedef intptr_t s_socket;
#else // POSIX
typedef int s_socket;
#endif // Windows / Posix



namespace Lepra
{



class TcpSocket;


// Base class for all sockets.
class SocketBase
{
public:
	enum
	{
		BUFFER_SIZE = 1024,
	};

	enum ShutdownFlag
	{
		SHUTDOWN_NONE = 0,
		SHUTDOWN_RECV = 1,
		SHUTDOWN_SEND = 2,
		SHUTDOWN_BOTH  = (SHUTDOWN_RECV|SHUTDOWN_SEND)
	};

	static s_socket InitSocket(s_socket pSocket, int pSize, bool pReuse);
	static s_socket CreateTcpSocket();
	static s_socket CreateUdpSocket();
	static void CloseSysSocket(s_socket pSocket);

	SocketBase(s_socket pSocket = ~0);	// INVALID_SOCKET
	virtual ~SocketBase();
	void Close();
	void CloseKeepHandle();
	bool IsOpen() const;
	s_socket GetSysSocket() const;

	void MakeBlocking();
	void MakeNonBlocking();

	void Shutdown(ShutdownFlag pHow);

	uint64 GetSentByteCount() const;
	uint64 GetReceivedByteCount() const;

protected:
	s_socket mSocket;
	uint64 mSentByteCount;
	uint64 mReceivedByteCount;
};


// Base class for socket classes with an Id.
class ConnectionWithId
{
public:
	ConnectionWithId();
	virtual ~ConnectionWithId();

	void SetConnectionId(const std::string& pConnectionId);
	void ClearConnectionId();
	const std::string& GetConnectionId() const;

private:
	std::string mConnectionId;
};



class BufferedIo;

// Base class of TcpMuxSocket and UdpMuxSocket.
class MuxIo
{
public:
	MuxIo(unsigned pMaxPendingConnectionCount, unsigned pMaxConnectionCount);
	virtual ~MuxIo();

	void AddSender(BufferedIo* pSender);
	void RemoveSenderNoLock(BufferedIo* pSender);
	bool IsSender(BufferedIo* pSender) const;

	void AddReceiver(BufferedIo* pReceiver);
	void AddReceiverNoLock(BufferedIo* pReceiver);
	void RemoveReceiverNoLock(BufferedIo* pReceiver);
	bool IsReceiverNoLock(BufferedIo* pReceiver) const;

	virtual void ReleaseSocketThreads();

protected:
	BufferedIo* PopSender();
	BufferedIo* PopReceiver();

	typedef HashSet<IPAddress, IPAddress> IPSet;

	mutable Lock mIoLock;
	IPSet mBannedIPTable;
	Semaphore mAcceptSemaphore;

	const unsigned mMaxPendingConnectionCount;
	const unsigned mMaxConnectionCount;

	static const char mConnectionString[27];
	static const uint8 mAcceptionString[15];

private:
	typedef std::unordered_set<BufferedIo*, LEPRA_VOIDP_HASHER> IoSet;

	IoSet mSenderSet;
	IoSet mReceiverSet;
};



// Base class of UdpVSocket and TcpVSocket. In the output case (as a "sender") it 
// is used to fill a datagram with packets until there is no room left (max limit 
// is defined by UdpMuxSocket). When the buffer is full it will be sent by calling 
// SendBuffer(). This reduces overhead in both UDP and TCP.
class BufferedIo
{
public:
	BufferedIo();
	virtual ~BufferedIo();

	void ClearOutputData();
	Datagram& GetSendBuffer() const;
	IOError AppendSendBuffer(const void* pData, int pLength);
	bool HasSendData() const;

	// Sets/Gets mInSendBuffer. Used to keep track on whether this is added
	// to MuxIo's sender list.
	void SetInSenderList(bool pInSendBuffer);
	bool GetInSenderList() const;

	virtual int SendBuffer() = 0;

protected:
	MuxIo* mMuxIo;
	Datagram mSendBuffer;
	bool mInSendBuffer;
};



class DatagramReceiver
{
public:
	virtual int Receive(TcpSocket* pSocket, void* pBuffer, int pMaxSize) = 0;
};



//
// A socket that listens to port specified in the constructor, on the interface
// defined by the given IP-address. A "TcpServerSocket" using Java language...
//
class TcpListenerSocket: public SocketBase
{
public:
	friend class TcpSocket;

	TcpListenerSocket(const SocketAddress& pLocalAddress, bool pIsServer);
	virtual ~TcpListenerSocket();

	// Waits for connection, and returns the connected socket.
	// If this is a non-blocking socket and there are no pending connections,
	// the returned pointer will be NULL.
	TcpSocket* Accept();

	uint16 GetPort() const;
	const SocketAddress& GetLocalAddress() const;
	unsigned GetConnectionCount() const;

	void SetDatagramReceiver(DatagramReceiver* pReceiver);

protected:
	DatagramReceiver* GetDatagramReceiver() const;
	typedef TcpSocket* (*SocketFactory)(s_socket, const SocketAddress&, TcpListenerSocket*, DatagramReceiver*);
	TcpSocket* Accept(SocketFactory pSocketFactory);

private:
	static TcpSocket* CreateSocket(s_socket pSocket, const SocketAddress& pTargetAddress,
		TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver);

	void DecNumConnections();

	int mConnectionCount;
	SocketAddress mLocalAddress;
	DatagramReceiver* mReceiver;

	logclass();
};


// A regular Tcp socket.
class TcpSocket: public SocketBase
{
public:
	TcpSocket(DatagramReceiver* pReceiver);
	TcpSocket(const SocketAddress& pLocalAddress);
	virtual ~TcpSocket();

	bool Connect(const SocketAddress& pTargetAddress);
	void Disconnect();

	// Nagle's algorithm is used in the Tcp protocol to reduce overhead when
	// resending small amounts of data - usually single keystrokes in telnet.
	bool DisableNagleAlgo();

	const SocketAddress& GetTargetAddress() const;

	int Send(const void* pData, int pSize);
	int Receive(void* pData, int pMaxSize);
	int Receive(void* pData, int pMaxSize, double pTimeout);
	bool Unreceive(void* pData, int pByteCount);

	void SetDatagramReceiver(DatagramReceiver* pReceiver);
	int ReceiveDatagram(void* pData, int pMaxSize);

protected:
	friend class TcpListenerSocket;

	TcpSocket(s_socket pSocket, const SocketAddress& pTargetAddress, TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver);

private:
	DatagramReceiver* mReceiver;
	uint8 mUnreceivedArray[16];
	int mUnreceivedByteCount;
	SocketAddress mTargetAddress;
	TcpListenerSocket* mServerSocket;

	logclass();
};



//
// Represents a regular UDP socket, which is inconvenient to use compared to
// the next class below...
//
class UdpSocket: public SocketBase
{
public:
	UdpSocket(const SocketAddress& pLocalAddress, bool pIsServer);
	UdpSocket(const UdpSocket& pSocket);
	virtual ~UdpSocket();

	const SocketAddress& GetLocalAddress() const;

	virtual int SendTo(const uint8* pData, unsigned pSize, const SocketAddress& pTargetAddress);
	virtual int ReceiveFrom(uint8* pData, unsigned pMaxSize, SocketAddress& pSourceAddress);
	virtual int ReceiveFrom(uint8* pData, unsigned pMaxSize, SocketAddress& pSourceAddress, double pTimeout);

private:
	SocketAddress mLocalAddress;

	logclass();
};



//
// A multiplexing UDP socket, which routes the incoming datagrams to 
// the correct virtual socket (UdpVSocket). 
//

class UdpVSocket;

class UdpMuxSocket: public MuxIo, protected Thread, public UdpSocket
{
public:
	friend class UdpVSocket;

	UdpMuxSocket(const str& pName, const SocketAddress& pLocalAddress, bool pIsServer,
		unsigned pMaxPendingConnectionCount = 16, unsigned pMaxConnectionCount = 1024);
	virtual ~UdpMuxSocket();

	// The sockets returned by these functions should be released using
	// CloseSocket(). Do not delete the socket!
	UdpVSocket* Connect(const SocketAddress& pTargetAddress, const std::string& pConnectionId, double pTimeout);
	UdpVSocket* Accept();
	UdpVSocket* PollAccept();
	void CloseSocket(UdpVSocket* pSocket);
	unsigned GetConnectionCount() const;

	UdpVSocket* PopReceiverSocket();
	UdpVSocket* PopSenderSocket();
	UdpVSocket* GetVSocket(const SocketAddress& pTargetAddress);

	bool SendOpenFirewallData(const SocketAddress& pTargetAddress);

protected:
	void Run();

	void RecycleBuffer(Datagram* pBuffer);

private:
	typedef HashTable<SocketAddress, UdpVSocket*, SocketAddress> SocketTable;
	typedef std::list<UdpVSocket*> SocketList;

	typedef FastAllocator<UdpVSocket> SocketAllocator;
	typedef FastAllocator<Datagram> BufferAllocator;

	SocketAllocator mSocketAllocator;
	BufferAllocator mBufferAllocator;

	SocketTable mSocketTable;
	SocketTable mAcceptTable;
	SocketList mAcceptList;

	static const uint8 mOpenFirewallString[27];

	logclass();
};



// A virtual socket; data is delegated here (upon receive) from the MUX
// socket, and passed through the MUX socket (on send).
class UdpVSocket: public BufferedIo, public InputStream, public OutputStream, public ConnectionWithId
{
public:
	UdpVSocket();
	virtual ~UdpVSocket();
	void Init(UdpMuxSocket& pSocket, const SocketAddress& pTargetAddress, const std::string& pConnectionId);

	void ClearAll();

	int Receive(bool pSafe, void* pData, int pLength);	// ::recv() return value.
	int Receive(void* pData, int pLength);	// ::recv() return value.
	int SendBuffer();	// ::send() return value.
	int DirectSend(const void* pData, int pLength);	// Writes buffer, flushes.

	const SocketAddress& GetLocalAddress() const;
	const SocketAddress& GetTargetAddress() const;

	void TryAddReceiverSocket();
	void AddInputBuffer(Datagram* pBuffer);
	bool NeedInputPeek() const;
	void SetReceiverFollowupActive(bool pActive);

	bool WaitAvailable(double pTime);

	// Stream interface.
	void Close();
	int64 GetAvailable() const;
	IOError ReadRaw(void* pData, size_t pLength);
	IOError Skip(size_t pLength);
	IOError WriteRaw(const void* pData, size_t pLength);
	void Flush();

	void SetSafeSend(bool);

private:
	static const int MAX_INPUT_BUFFERS = 32;

	typedef CyclicArray<Datagram*, MAX_INPUT_BUFFERS> BufferList;

	SocketAddress mTargetAddress;

	mutable Lock mLock;

	BufferList mReceiveBufferList;
	unsigned mRawReadBufferIndex;
	bool mReceiverFollowupActive;

	logclass();
};



}
