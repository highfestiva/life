
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../ThirdParty/FastDelegate/FastDelegate.h"
#include "FdSet.h"
#include "HiResTimer.h"
#include "MemberThread.h"
#include "OrderedMap.h"
#include "Socket.h"



namespace Lepra
{



class TcpVSocket;



class TcpMuxSocket: public MuxIo, public TcpListenerSocket
{
public:
	friend class TcpVSocket;

	TcpMuxSocket(const str& pName, const SocketAddress& pLocalAddress, bool pIsServer,
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

	static TcpSocket* CreateSocket(s_socket pSocket, const SocketAddress& pTargetAddress,
		TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver);
	void AddConnectedSocket(TcpVSocket* pSocket);
	bool RemoveConnectedSocketNoLock(TcpVSocket* pSocket);
	int BuildConnectedSocketSet(FdSet& pSocketSet);
	void PushReceiverSockets(const FdSet& pSocketSet);
	AcceptStatus QueryReceiveConnectString(TcpVSocket* pSocket);
	void ReleaseSocketThreads();
	void AcceptThreadEntry();
	void SelectThreadEntry();

	typedef OrderedMap<TcpVSocket*, HiResTimer, LEPRA_VOIDP_HASHER> SocketTimeMap;
	typedef std::unordered_map<s_socket, TcpVSocket*> SocketVMap;

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

	logclass();
};

class TcpVSocket: public BufferedIo, public TcpSocket, public ConnectionWithId
{
public:
	TcpVSocket(TcpMuxSocket& pMuxSocket, DatagramReceiver* pReceiver);
	TcpVSocket(s_socket pSocket, const SocketAddress& pTargetAddress, TcpMuxSocket& pMuxSocket, DatagramReceiver* pReceiver);
	virtual ~TcpVSocket();

	bool Connect(const SocketAddress& pTargetAddress, const std::string& pConnectionId, double pTimeout);

	int SendBuffer();	// ::send() return value.
	int Receive(void* pData, int pMaxSize, bool pDatagram = true);
	int Receive(void* pData, int pMaxSize, double pTimeout, bool pDatagram);

private:
	logclass();
};



class DualSocket;

//
// The dream of all game programmers! =)
// A multiplexing socket that works over both UDP and TCP at the same time, over only
// one socket address!
class DualMuxSocket
{
public:
	DualMuxSocket(const str& pName, const SocketAddress& pLocalAddress, bool pIsServer,
		unsigned pMaxPendingConnectionCount = 16, unsigned pMaxConnectionCount = 256);
	virtual ~DualMuxSocket();

	bool IsOpen() const;
	bool IsOpen(DualSocket* pSocket) const;

	DualSocket* Connect(const SocketAddress& pTargetAddress, const std::string& pConnectionId, double pTimeout);

	void Close();
	void CloseSocket(DualSocket* pSocket);

	template<class _Base> void SetCloseCallback(_Base* pCallee, void (_Base::*pMethod)(DualSocket*))
	{
		mCloseDispatcher = SocketDispatcher(pCallee, pMethod);
	};
	void DispatchCloseSocket(DualSocket* pSocket);

	DualSocket* PollAccept();
	DualSocket* PopReceiverSocket();
	DualSocket* PopReceiverSocket(bool pSafe);
	DualSocket* PopSenderSocket();

	uint64 GetSentByteCount() const;
	uint64 GetSentByteCount(bool pSafe) const;
	uint64 GetReceivedByteCount() const;
	uint64 GetReceivedByteCount(bool pSafe) const;

	SocketAddress GetLocalAddress() const;
	unsigned GetConnectionCount() const;

	void SetSafeConnectIdTimeout(double pTimeout);
	void SetConnectDualTimeout(double pTimeout);
	void SetDatagramReceiver(DatagramReceiver* pReceiver);

protected:
	friend class DualSocket;

	void AddUdpReceiverSocket(UdpVSocket* pSocket);

private:
	void AddSocket(DualSocket* pSocket, TcpVSocket* pTcpSocket, UdpVSocket* pUdpSocket);
	void KillNonDualConnected();

	void OnCloseTcpSocket(TcpVSocket* pTcpSocket);

	typedef std::unordered_map<TcpVSocket*, DualSocket*, LEPRA_VOIDP_HASHER> TcpSocketMap;
	typedef std::unordered_map<UdpVSocket*, DualSocket*, LEPRA_VOIDP_HASHER> UdpSocketMap;
	typedef std::unordered_map<std::string, DualSocket*> IdSocketMap;
	typedef OrderedMap<DualSocket*, HiResTimer, LEPRA_VOIDP_HASHER> SocketTimeMap;

	mutable Lock mLock;
	TcpMuxSocket* mTcpMuxSocket;
	UdpMuxSocket* mUdpMuxSocket;
	TcpSocketMap mTcpSocketMap;
	UdpSocketMap mUdpSocketMap;
	IdSocketMap mIdSocketMap;
	static const double DEFAULT_CONNECT_DUAL_TIMEOUT;
	double mConnectDualTimeout;
	SocketTimeMap mPendingDualConnectMap;
	bool mPopSafeToggle;

	typedef fastdelegate::FastDelegate1<DualSocket*, void> SocketDispatcher;
	SocketDispatcher mCloseDispatcher;

	template<class _MuxSocket, class _VSocket> class Connector: public Thread
	{
	public:
		Connector(const str& pName, _MuxSocket* pMuxSocket, const SocketAddress& pTargetAddress,
			const std::string& pConnectionId, double pTimeout, Semaphore& pSemaphore):
			Thread(astrutil::Encode(pName)),
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

	logclass();
};



// Sends all "safe" data through a TCP connection (guarantees ordered arrivals,
// also of large data chunks), everything else (positional info) over UDP.
class DualSocket: public ConnectionWithId
{
public:
	DualSocket(DualMuxSocket* pMuxSocket, const std::string& lConnectionId);

	bool SetSocket(TcpVSocket* pSocket);
	bool SetSocket(UdpVSocket* pSocket);
	bool IsOpen() const;
	void ClearAll();

	SocketAddress GetLocalAddress() const;
	SocketAddress GetTargetAddress() const;

	void ClearOutputData();
	void SetSafeSend(bool pSafe);
	Datagram& GetSendBuffer() const;
	Datagram& GetSendBuffer(bool pSafe) const;
	IOError AppendSendBuffer(const void* pData, int pLength);
	IOError AppendSendBuffer(bool pSafe, const void* pData, int pLength);
	int SendBuffer();	// ::send() and ::sendto() return values.
	bool HasSendData() const;

	void SetSafeReceive(bool pSafe);
	int Receive(void* pData, int pLength);
	int Receive(bool pSafe, void* pData, int pLength);

	void TryAddReceiverSocket();

protected:
	friend class DualMuxSocket;

	virtual ~DualSocket();

	TcpVSocket* GetTcpSocket() const;
	UdpVSocket* GetUdpSocket() const;


private:
	DualMuxSocket* mMuxSocket;
	TcpVSocket* mTcpSocket;
	UdpVSocket* mUdpSocket;
	bool mDefaultSafeSend;
	bool mDefaultSafeReceive;

	logclass();
};



}
