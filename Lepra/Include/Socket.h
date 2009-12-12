
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <hash_set>
#include <list>
#include "../../ThirdParty/FastDelegate/FastDelegate.h"
#include "CyclicArray.h"
#include "FastAllocator.h"
#include "FdSet.h"
#include "Hasher.h"
#include "HashSet.h"
#include "HashTable.h"
#include "InputStream.h"
#include "IPAddress.h"
#include "Lepra.h"
#include "LepraTarget.h"
#include "MemberThread.h"
#include "SocketAddress.h"
#include "OutputStream.h"
#include "OrderedMap.h"
#include "SocketAddress.h"
#include "SpinLock.h"
#include "Thread.h"
#include "Timer.h"



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
		SHUTDOWN_RECV = 1,
		SHUTDOWN_SEND = 2,
		SHUTDOWN_BOTH  = (SHUTDOWN_RECV|SHUTDOWN_SEND)
	};

	static sys_socket InitSocket(sys_socket pSocket, int pSize);
	static sys_socket CreateTcpSocket();
	static sys_socket CreateUdpSocket();
	static void CloseSysSocket(sys_socket pSocket);

	SocketBase(sys_socket pSocket = INVALID_SOCKET);
	virtual ~SocketBase();
	void Close();
	void CloseKeepHandle();
	bool IsOpen() const;
	sys_socket GetSysSocket() const;

	void MakeBlocking();
	void MakeNonBlocking();

	void Shutdown(ShutdownFlag pHow);

	uint64 GetSentByteCount() const;
	uint64 GetReceivedByteCount() const;

protected:
	sys_socket mSocket;
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


// Simple data storage, used with both UDP and TCP protocol.
class Datagram
{
public:
	inline void Init()
	{
		mDataSize = 0;
	}

	uint8 mDataBuffer[SocketBase::BUFFER_SIZE];
	int mDataSize;
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

protected:
	virtual void ReleaseSocketThreads();

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
	typedef std::hash_set<BufferedIo*, std::hash<void*> > IoSet;

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

	TcpListenerSocket(const SocketAddress& pLocalAddress);
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
	typedef TcpSocket* (*SocketFactory)(sys_socket, const SocketAddress&, TcpListenerSocket*, DatagramReceiver*);
	TcpSocket* Accept(SocketFactory pSocketFactory);

private:
	static TcpSocket* CreateSocket(sys_socket pSocket, const SocketAddress& pTargetAddress,
		TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver);

	void DecNumConnections();

	int mConnectionCount;
	SocketAddress mLocalAddress;
	DatagramReceiver* mReceiver;

	LOG_CLASS_DECLARE();
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
	bool IsConnected();

	// Nagle's algorithm is used in the Tcp protocol to reduce overhead when
	// resending small amounts of data - usually single keystrokes in telnet.
	bool DisableNagleAlgo();

	const SocketAddress& GetTargetAddress() const;

	int Send(const void* pData, int pSize);
	int Receive(void* pData, int pMaxSize);
	bool Unreceive(void* pData, int pByteCount);

	void SetDatagramReceiver(DatagramReceiver* pReceiver);
	int ReceiveDatagram(void* pData, int pMaxSize);

protected:
	friend class TcpListenerSocket;

	TcpSocket(sys_socket pSocket, const SocketAddress& pTargetAddress, TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver);

private:
	DatagramReceiver* mReceiver;
	uint8 mUnreceivedArray[16];
	int mUnreceivedByteCount;
	SocketAddress mTargetAddress;
	TcpListenerSocket* mServerSocket;

	LOG_CLASS_DECLARE();
};



//
// A multiplexing TCP socket, which routes the incoming datagrams to 
// the correct virtual socket (TcpVSocket). 
//
class TcpVSocket;

class TcpMuxSocket: public MuxIo, public TcpListenerSocket
{
public:
	friend class TcpVSocket;

	TcpMuxSocket(const String& pName, const SocketAddress& pLocalAddress, bool pIsServer,
		unsigned pMaxPendingConnectionCount = 16, unsigned pMaxConnectionCount = 1024);
	virtual ~TcpMuxSocket();

	// The sockets returned by these functions should be released using
	// CloseSocket(). Do not delete the socket!
	TcpVSocket* Connect(const SocketAddress& pTargetAddress, const std::string& pConnectionId, double pTimeout);
	TcpVSocket* Accept();
	TcpVSocket* PollAccept();

	template<class _Base> void SetCloseCallback(_Base* pCallee, void (_Base::*pMethod)(TcpVSocket*))
	{
		mCloseDispatcher = SocketDispatcher(pCallee, pMethod);
	};
	void DispatchCloseSocket(TcpVSocket* pSocket);
	void CloseSocket(TcpVSocket* pSocket, bool pForceDelete = false);

	TcpVSocket* PopReceiverSocket();
	TcpVSocket* PopSenderSocket();

	uint64 GetTotalSentByteCount() const;
	uint64 GetTotalReceivedByteCount() const;
	void AddTotalSentByteCount(unsigned pByteCount);
	void AddTotalReceivedByteCount(unsigned pByteCount);

	void SetConnectIdTimeout(double pTimeout);

private:
	enum AcceptStatus
	{
		ACCEPT_OK,
		ACCEPT_CLOSE,
		ACCEPT_QUERY_WAIT,
	};

	static TcpSocket* CreateSocket(sys_socket pSocket, const SocketAddress& pTargetAddress,
		TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver);
	void AddConnectedSocket(TcpVSocket* pSocket);
	bool RemoveConnectedSocketNoLock(TcpVSocket* pSocket);
	int BuildConnectedSocketSet(FdSet& pSocketSet);
	void PushReceiverSockets(const FdSet& pSocketSet);
	AcceptStatus QueryReceiveConnectString(TcpVSocket* pSocket);
	void ReleaseSocketThreads();
	void AcceptThreadEntry();
	void SelectThreadEntry();

	typedef Lepra::OrderedMap<TcpVSocket*, Timer, std::hash<void*> > SocketTimeMap;
	typedef std::hash_map<sys_socket, TcpVSocket*> SocketVMap;

	MemberThread<TcpMuxSocket> mAcceptThread;
	MemberThread<TcpMuxSocket> mSelectThread;
	static const double DEFAULT_CONNECT_ID_TIMEOUT;
	double mConnectIdTimeout;
	SocketTimeMap mPendingConnectIdMap;
	SocketVMap mConnectedSocketMap;
	bool mActiveReceiverMapChanged;
	FdSet mBackupFdSet;
	Semaphore mConnectedSocketSemaphore;
	int64 mVSentByteCount;
	int64 mVReceivedByteCount;
	typedef fastdelegate::FastDelegate1<TcpVSocket*, void> SocketDispatcher;
	SocketDispatcher mCloseDispatcher;

	LOG_CLASS_DECLARE();
};

class TcpVSocket: public BufferedIo, public TcpSocket, public ConnectionWithId
{
public:
	TcpVSocket(TcpMuxSocket& pMuxSocket, DatagramReceiver* pReceiver);
	TcpVSocket(sys_socket pSocket, const SocketAddress& pTargetAddress, TcpMuxSocket& pMuxSocket, DatagramReceiver* pReceiver);
	virtual ~TcpVSocket();

	bool Connect(const SocketAddress& pTargetAddress, const std::string& pConnectionId, double pTimeout);

	int SendBuffer();	// ::send() return value.
	int Receive(void* pData, int pMaxSize, bool pDatagram = true);
	int Receive(void* pData, int pMaxSize, double pTimeout, bool pDatagram);

private:
	LOG_CLASS_DECLARE();
};



//
// Represents a regular UDP socket, which is inconvenient to use compared to
// the next class below...
//
class UdpSocket: public SocketBase
{
public:
	UdpSocket(const SocketAddress& pLocalAddress);
	UdpSocket(const UdpSocket& pSocket);
	virtual ~UdpSocket();

	const SocketAddress& GetLocalAddress() const;

	virtual int SendTo(const uint8* pData, unsigned pSize, const SocketAddress& pTargetAddress);
	virtual int ReceiveFrom(uint8* pData, unsigned pMaxSize, SocketAddress& pSourceAddress);

private:
	SocketAddress mLocalAddress;

	LOG_CLASS_DECLARE();
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

	UdpMuxSocket(const String& pName, const SocketAddress& pLocalAddress,
		unsigned pMaxPendingConnectionCount = 16, unsigned pMaxConnectionCount = 1024);
	virtual ~UdpMuxSocket();

	// The sockets returned by these functions should be released using
	// CloseSocket(). Do not delete the socket!
	UdpVSocket* Connect(const SocketAddress& pTargetAddress, const std::string& pConnectionId, double pTimeout);
	UdpVSocket* Accept();
	UdpVSocket* PollAccept();
	void CloseSocket(UdpVSocket* pSocket);
	unsigned GetConnectionCount() const;

	void AddBannedIP(const IPAddress& pIP);
	void RemoveBannedIP(const IPAddress& pIP);

	UdpVSocket* PopReceiverSocket();
	UdpVSocket* PopSenderSocket();

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

	LOG_CLASS_DECLARE();
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

	int Receive(void* pData, int pLength);	// ::recv() return value.
	int SendBuffer();	// ::send() return value.
	int DirectSend(const void* pData, int pLength);	// Writes buffer, flushes.

	const SocketAddress& GetTargetAddress() const;

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

private:
	static const int MAX_INPUT_BUFFERS = 4;

	typedef CyclicArray<Datagram*, MAX_INPUT_BUFFERS> BufferList;

	SocketAddress mTargetAddress;

	mutable Lock mLock;

	BufferList mReceiveBufferList;
	unsigned mRawReadBufferIndex;
	bool mReceiverFollowupActive;

	LOG_CLASS_DECLARE();
};



class GameSocket;

//
// The dream of all game programmers! =)
// A multiplexing socket that works over both UDP and TCP at the same time, over only
// one socket address!
class GameMuxSocket
{
public:
	GameMuxSocket(const String& pName, const SocketAddress& pLocalAddress, bool pIsServer,
		unsigned pMaxPendingConnectionCount = 16, unsigned pMaxConnectionCount = 256);
	virtual ~GameMuxSocket();

	bool IsOpen() const;

	GameSocket* Connect(const SocketAddress& pTargetAddress, double pTimeout);

	void Close();
	void CloseSocket(GameSocket* pSocket);

	template<class _Base> void SetCloseCallback(_Base* pCallee, void (_Base::*pMethod)(GameSocket*))
	{
		mCloseDispatcher = SocketDispatcher(pCallee, pMethod);
	};
	void DispatchCloseSocket(GameSocket* pSocket);

	GameSocket* PollAccept();
	GameSocket* PopReceiverSocket(bool pSafe);
	GameSocket* PopSenderSocket();

	uint64 GetSentByteCount(bool pSafe) const;
	uint64 GetReceivedByteCount(bool pSafe) const;

	SocketAddress GetLocalAddress() const;
	unsigned GetConnectionCount() const;

	void SetSafeConnectIdTimeout(double pTimeout);
	void SetConnectDualTimeout(double pTimeout);
	void SetDatagramReceiver(DatagramReceiver* pReceiver);

protected:
	friend class GameSocket;

	void AddUdpReceiverSocket(UdpVSocket* pSocket);

private:
	void AddSocket(GameSocket* pSocket, TcpVSocket* pTcpSocket, UdpVSocket* pUdpSocket);
	void KillNonDualConnected();

	void OnCloseTcpSocket(TcpVSocket* pTcpSocket);

	typedef std::hash_map<TcpVSocket*, GameSocket*, std::hash<void*> > TcpSocketMap;
	typedef std::hash_map<UdpVSocket*, GameSocket*, std::hash<void*> > UdpSocketMap;
	typedef std::hash_map<std::string, GameSocket*> IdSocketMap;
	typedef Lepra::OrderedMap<GameSocket*, Timer, std::hash<void*> > SocketTimeMap;

	mutable Lock mLock;
	TcpMuxSocket* mTcpMuxSocket;
	UdpMuxSocket* mUdpMuxSocket;
	TcpSocketMap mTcpSocketMap;
	UdpSocketMap mUdpSocketMap;
	IdSocketMap mIdSocketMap;
	static const double DEFAULT_CONNECT_DUAL_TIMEOUT;
	double mConnectDualTimeout;
	SocketTimeMap mPendingDualConnectMap;

	typedef fastdelegate::FastDelegate1<GameSocket*, void> SocketDispatcher;
	SocketDispatcher mCloseDispatcher;

	template<class _MuxSocket, class _VSocket> class Connector: public Thread
	{
	public:
		Connector(const String& pName, _MuxSocket* pMuxSocket, const SocketAddress& pTargetAddress,
			const std::string& pConnectionId, double pTimeout, Semaphore& pSemaphore):
			Thread(pName),
			mMuxSocket(pMuxSocket),
			mSocket(0),
			mTargetAddress(pTargetAddress),
			mConnectionId(pConnectionId),
			mTimeout(pTimeout),
			mSemaphore(pSemaphore)
		{
		}
		void Run()
		{
			mSocket = mMuxSocket->Connect(mTargetAddress, mConnectionId, mTimeout);
			mSemaphore.Signal();
		}
		_MuxSocket* mMuxSocket;
		_VSocket* mSocket;
		SocketAddress mTargetAddress;
		std::string mConnectionId;
		double mTimeout;
		Semaphore& mSemaphore;
	private:
		void operator=(const Connector&);
	};

	LOG_CLASS_DECLARE();
};



// Sends all "safe" data through a TCP connection (guarantees ordered arrivals,
// also of large data chunks), everything else (positional info) over UDP.
class GameSocket: public ConnectionWithId
{
public:
	GameSocket(GameMuxSocket* pMuxSocket, const std::string& lConnectionId);

	bool SetSocket(TcpVSocket* pSocket);
	bool SetSocket(UdpVSocket* pSocket);
	bool IsOpen() const;
	void ClearAll();

	SocketAddress GetLocalAddress() const;
	SocketAddress GetTargetAddress() const;

	void ClearOutputData();
	Datagram& GetSendBuffer(bool pSafe) const;
	IOError AppendSendBuffer(bool pSafe, const void* pData, int pLength);
	int SendBuffer();	// ::send() and ::sendto() return values.
	bool HasSendData() const;

	int Receive(bool pSafe, void* pData, int pLength);

	void TryAddReceiverSocket();

protected:
	friend class GameMuxSocket;

	virtual ~GameSocket();

	TcpVSocket* GetTcpSocket() const;
	UdpVSocket* GetUdpSocket() const;


private:
	GameMuxSocket* mMuxSocket;
	TcpVSocket* mTcpSocket;
	UdpVSocket* mUdpSocket;

	LOG_CLASS_DECLARE();
};



}
