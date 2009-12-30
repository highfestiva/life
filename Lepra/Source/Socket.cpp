
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



//#define FD_SETSIZE	256	// Used for desciding how many sockets that can be listened to using ::select().
#include <assert.h>
#include <algorithm>
#include <fcntl.h>
#include "../Include/Lepra.h"
#include "../Include/HashUtil.h"
#include "../Include/HiResTimer.h"
#include "../Include/Log.h"
#include "../Include/Network.h"
#include "../Include/Socket.h"
#include "../Include/SocketAddress.h"
#include "../Include/SystemManager.h"
#include "../Include/Thread.h"



namespace Lepra
{



sys_socket SocketBase::InitSocket(sys_socket pSocket, int pSize)
{
	// Set the underlying socket buffer sizes.
	int lBufferSize = pSize;
	::setsockopt(pSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&lBufferSize, sizeof(lBufferSize));
	lBufferSize = pSize;
	::setsockopt(pSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&lBufferSize, sizeof(lBufferSize));
	linger lLinger;
	lLinger.l_onoff = 0;	// Graceful shutdown.
	lLinger.l_linger = 1;	// Wait this many seconds.
	::setsockopt(pSocket, SOL_SOCKET, SO_LINGER, (const char*)&lLinger, sizeof(lLinger));
#ifndef LEPRA_WINDOWS
	int lFlag = 1;
	::setsockopt(pSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&lFlag, sizeof(lFlag));
#endif // !Windows
	return (pSocket);
}

sys_socket SocketBase::CreateTcpSocket()
{
	sys_socket s = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	return (InitSocket(s, 32*1024));
}

sys_socket SocketBase::CreateUdpSocket()
{
	sys_socket s = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	return (InitSocket(s, 8*1024));
}

void SocketBase::CloseSysSocket(sys_socket pSocket)
{
#ifdef LEPRA_WINDOWS
	::closesocket(pSocket);
#else // !LEPRA_WINDOWS
	::close(pSocket);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
}



SocketBase::SocketBase(sys_socket pSocket):
	mSocket(pSocket),
	mSentByteCount(0),
	mReceivedByteCount(0)
{
}

SocketBase::~SocketBase()
{
	Close();
}

void SocketBase::Close()
{
	CloseKeepHandle();
	mSocket = INVALID_SOCKET;
}

void SocketBase::CloseKeepHandle()
{
	if (mSocket != INVALID_SOCKET)
	{
		Shutdown(SHUTDOWN_SEND);
		CloseSysSocket(mSocket);
	}
}

bool SocketBase::IsOpen() const
{
	return (mSocket != INVALID_SOCKET);
}

sys_socket SocketBase::GetSysSocket() const
{
	return (mSocket);
}

void SocketBase::MakeBlocking()
{
#ifdef LEPRA_WINDOWS
	u_long lNonBlocking = 0;
	::ioctlsocket(mSocket, FIONBIO, &lNonBlocking);
#elif defined LEPRA_POSIX
	::fcntl(mSocket, F_SETFL, 0);
#else // !LEPRA_WINDOWS
#error "Not implemented!"
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
}

void SocketBase::MakeNonBlocking()
{
#ifdef LEPRA_WINDOWS
	u_long lNonBlocking = 1;
	::ioctlsocket(mSocket, FIONBIO, &lNonBlocking);
#elif defined LEPRA_POSIX
	::fcntl(mSocket, F_SETFL, O_NONBLOCK);
#else // !LEPRA_WINDOWS
#error "Not implemented!"
#endif // LEPRA_WINDOWS /!LEPRA_WINDOWS
}

void SocketBase::Shutdown(ShutdownFlag pHow)
{
	int lHow = 0;
	switch (pHow)
	{
#ifdef LEPRA_WINDOWS
		case SHUTDOWN_RECV:	lHow = SD_RECEIVE;	break;
		case SHUTDOWN_SEND:	lHow = SD_SEND;		break;
		case SHUTDOWN_BOTH:	lHow = SD_BOTH;		break;
#else // Posix
		case SHUTDOWN_RECV:	lHow = SHUT_RD;		break;
		case SHUTDOWN_SEND:	lHow = SHUT_WR;		break;
		case SHUTDOWN_BOTH:	lHow = SHUT_RDWR;	break;
#endif // Win32 / Posix
	}
	::shutdown(mSocket, (int)pHow);
}

uint64 SocketBase::GetSentByteCount() const
{
	return (mSentByteCount);
}

uint64 SocketBase::GetReceivedByteCount() const
{
	return (mReceivedByteCount);
}



ConnectionWithId::ConnectionWithId()
{
}

ConnectionWithId::~ConnectionWithId()
{
	ClearConnectionId();
}

void ConnectionWithId::SetConnectionId(const std::string& pConnectionId)
{
	mConnectionId = pConnectionId;
}

void ConnectionWithId::ClearConnectionId()
{
	mConnectionId.clear();
}

const std::string& ConnectionWithId::GetConnectionId() const
{
	return (mConnectionId);
}



MuxIo::MuxIo(unsigned pMaxPendingConnectionCount, unsigned pMaxConnectionCount):
	mMaxPendingConnectionCount(pMaxPendingConnectionCount),
	mMaxConnectionCount(pMaxConnectionCount)
{
}

MuxIo::~MuxIo()
{
}

void MuxIo::AddSender(BufferedIo* pSender)
{
	ScopeLock lLock(&mIoLock);
	mSenderSet.insert(pSender);
}

void MuxIo::RemoveSenderNoLock(BufferedIo* pSender)
{
	mSenderSet.erase(pSender);
}

bool MuxIo::IsSender(BufferedIo* pSender) const
{
	return (mSenderSet.find(pSender) != mSenderSet.end());
}

void MuxIo::AddReceiver(BufferedIo* pReceiver)
{
	ScopeLock lLock(&mIoLock);
	mReceiverSet.insert(pReceiver);
}

void MuxIo::AddReceiverNoLock(BufferedIo* pReceiver)
{
	mReceiverSet.insert(pReceiver);
}

void MuxIo::RemoveReceiverNoLock(BufferedIo* pReceiver)
{
	mReceiverSet.erase(pReceiver);
}

bool MuxIo::IsReceiverNoLock(BufferedIo* pReceiver) const
{
	return (HashUtil::FindSetObject(mReceiverSet, pReceiver) == pReceiver);
}

void MuxIo::ReleaseSocketThreads()
{
	for (int x = 0; x < 10; ++x)
	{
		mAcceptSemaphore.Signal();
	}
}

BufferedIo* MuxIo::PopSender()
{
	ScopeLock lLock(&mIoLock);
	BufferedIo* lSender = 0;
	if (mSenderSet.size() > 0)
	{
		lSender = *mSenderSet.begin();
		mSenderSet.erase(mSenderSet.begin());
		lSender->SetInSenderList(false);
	}
	return (lSender);
}

BufferedIo* MuxIo::PopReceiver()
{
	ScopeLock lLock(&mIoLock);
	BufferedIo* lReceiver = 0;
	if (mReceiverSet.size() > 0)
	{
		lReceiver = *mReceiverSet.begin();
		mReceiverSet.erase(mReceiverSet.begin());
	}
	return (lReceiver);
}

const char MuxIo::mConnectionString[27] = "Hook me up, operator? I'm ";
const uint8 MuxIo::mAcceptionString[15] = "Join the club!";



BufferedIo::BufferedIo():
	mMuxIo(0),
	mInSendBuffer(false)
{
	ClearOutputData();
}

BufferedIo::~BufferedIo()
{
}

void BufferedIo::ClearOutputData()
{
	mSendBuffer.Init();
}

Datagram& BufferedIo::GetSendBuffer() const
{
	return ((Datagram&)mSendBuffer);
}

IOError BufferedIo::AppendSendBuffer(const void* pData, int pLength)
{
	IOError lError = IO_OK;

	if (mSendBuffer.mDataSize + pLength <= SocketBase::BUFFER_SIZE)
	{
		if (!mInSendBuffer)
		{
			SetInSenderList(true);
			assert(!mMuxIo->IsSender(this));
			mMuxIo->AddSender(this);
		}
		else
		{
			assert(mMuxIo->IsSender(this));
		}
		::memcpy(&mSendBuffer.mDataBuffer[mSendBuffer.mDataSize], pData, pLength);
		mSendBuffer.mDataSize += (int)pLength;
	}
	else
	{
		// It's all or nothing. If half a network packet arrives, we're fucked.
		SendBuffer();
		if (pLength <= SocketBase::BUFFER_SIZE)
		{
			lError = AppendSendBuffer(pData, pLength);
		}
		else
		{
			lError = IO_BUFFER_OVERFLOW;
		}
	}

	return (lError);
}

bool BufferedIo::HasSendData() const
{
	return (mSendBuffer.mDataSize > 0);
}

void BufferedIo::SetInSenderList(bool pInSendBuffer)
{
	mInSendBuffer = pInSendBuffer;
}

bool BufferedIo::GetInSenderList() const
{
	return (mInSendBuffer);
}



TcpListenerSocket::TcpListenerSocket(const SocketAddress& pLocalAddress):
	mConnectionCount(0),
	mLocalAddress(pLocalAddress),
	mReceiver(0)
{
	log_atrace("TcpListenerSocket()");

	// Initialize the socket.
	mSocket = CreateTcpSocket();

	if (mSocket != INVALID_SOCKET)
	{
		// Init socket address and bind it to the socket.
		if (::bind(mSocket, (const sockaddr*)&mLocalAddress.GetAddr(), sizeof(mLocalAddress.GetAddr())) == 0)
		{
			::listen(mSocket, SOMAXCONN);
		}
		else
		{
			mLog.Warning(_T("Failed to bind TCP listener socket to ")+pLocalAddress.GetAsString()+_T("."));
			Close();
		}
	}
}

TcpListenerSocket::~TcpListenerSocket()
{
	log_atrace("~TcpListenerSocket()");
	mReceiver = 0;
}

TcpSocket* TcpListenerSocket::Accept()
{
	return (Accept(&TcpListenerSocket::CreateSocket));
}

const SocketAddress& TcpListenerSocket::GetLocalAddress() const
{
	return (mLocalAddress);
}

unsigned TcpListenerSocket::GetConnectionCount() const
{
	return (mConnectionCount);
}

void TcpListenerSocket::SetDatagramReceiver(DatagramReceiver* pReceiver)
{
	mReceiver = pReceiver;
}

DatagramReceiver* TcpListenerSocket::GetDatagramReceiver() const
{
	return (mReceiver);
}

TcpSocket* TcpListenerSocket::Accept(SocketFactory pSocketFactory)
{
	TcpSocket* lTcpSocket = 0;
	if (mSocket != INVALID_SOCKET)
	{
		int lAcceptCount = 0;
		while (IsOpen() && lAcceptCount == 0)
		{
			fd_set lAcceptSet;
			FD_ZERO(&lAcceptSet);
#pragma warning(push)
#pragma warning(disable: 4127)	// MSVC warning: conditional expression is constant.
			FD_SET(mSocket, &lAcceptSet);
#pragma warning(pop)
			timeval lTime;
			lTime.tv_sec = 1;
			lTime.tv_usec = 0;
			lAcceptCount = ::select((int)mSocket+1, &lAcceptSet, NULL, NULL, &lTime);
		}
		if (lAcceptCount >= 1)
		{
			SocketAddress lSockAddress;
			socklen_t lSize = (socklen_t)sizeof(lSockAddress.GetAddr());
			sys_socket lSocket = ::accept(mSocket, (sockaddr*)&lSockAddress.GetAddr(), &lSize);
			if (lSocket != INVALID_SOCKET)
			{
				log_atrace("::accept() received a ::connect()");
				lTcpSocket = pSocketFactory(lSocket, lSockAddress, this, mReceiver);
				BusLock::Add(&mConnectionCount, 1);
			}
		}
	}
	return (lTcpSocket);
}

TcpSocket* TcpListenerSocket::CreateSocket(sys_socket pSocket, const SocketAddress& pTargetAddress,
	TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver)
{
	return (new TcpSocket(pSocket, pTargetAddress, pServerSocket, pReceiver));
}

void TcpListenerSocket::DecNumConnections()
{
	BusLock::Add(&mConnectionCount, -1);
}

LOG_CLASS_DEFINE(NETWORK, TcpListenerSocket);



TcpSocket::TcpSocket(DatagramReceiver* pReceiver):
	mReceiver(pReceiver),
	mUnreceivedByteCount(0),
	mTargetAddress(),
	mServerSocket(0)
{
	mSocket = CreateTcpSocket();
}

TcpSocket::TcpSocket(const SocketAddress& pLocalAddress):
	mReceiver(0),
	mUnreceivedByteCount(0),
	mTargetAddress(),
	mServerSocket(0)
{
	mSocket = CreateTcpSocket();

	if (mSocket != INVALID_SOCKET)
	{
		if (::bind(mSocket, (const sockaddr*)&pLocalAddress.GetAddr(), sizeof(pLocalAddress.GetAddr())) != 0)
		{
			mLog.Warningf(_T("TCP socket binding failed! Error=%i"), SOCKET_LAST_ERROR());
			Close();
		}
	}
}

TcpSocket::TcpSocket(sys_socket pSocket, const SocketAddress& pTargetAddress,
	TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver):
	SocketBase(pSocket),
	mReceiver(pReceiver),
	mUnreceivedByteCount(0),
	mTargetAddress(pTargetAddress),
	mServerSocket(pServerSocket)
{
	log_atrace("TcpSocket()");
}

TcpSocket::~TcpSocket()
{
	log_atrace("~TcpSocket()");
	Disconnect();
	mReceiver = 0;
}

bool TcpSocket::Connect(const SocketAddress& pTargetAddress)
{
	mTargetAddress = pTargetAddress;
	bool lOk = (::connect(mSocket, (const sockaddr*)&mTargetAddress.GetAddr(), sizeof(mTargetAddress.GetAddr())) != SOCKET_ERROR);
	if (!lOk)
	{
		int e = SOCKET_LAST_ERROR();
		mLog.Infof(_T("TCP connect failed! Error=%i."), e);
		Close();
	}
	return (lOk);
}

void TcpSocket::Disconnect()
{
	if (mServerSocket != 0)
	{
		mServerSocket->DecNumConnections();
	}

	Close();
}

bool TcpSocket::DisableNagleAlgo()
{
	// Disable the Nagle algorithm.
	int lFlag = 1;
	int lResult = setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&lFlag, sizeof(lFlag));
	return (lResult == 0);
}

bool TcpSocket::IsConnected()
{
	return (mSocket != INVALID_SOCKET);
}

int TcpSocket::Send(const void* pData, int pSize)
{
	int lSentByteCount = 0;
	if (mSocket != INVALID_SOCKET && pData != 0 && pSize > 0)
	{
		lSentByteCount = ::send(mSocket, (const char*)pData, pSize, 0);
		// Did we get disconnected?
		if (lSentByteCount <= 0)
		{
			int e = SOCKET_LAST_ERROR();
			mLog.Errorf(_T("TCP send error. Error=%i, socket=%u."), e, mSocket);
			CloseKeepHandle();
		}
		else
		{
			log_volatile(str lLocalAddress);
			log_volatile(if (mServerSocket) lLocalAddress = mServerSocket->GetLocalAddress().GetAsString());
			log_volatile(str lData = strutil::DumpData((uint8*)pData, std::min((int)pSize, 20)));
			log_volatile(mLog.Tracef(_T("TCP -> %u bytes (%s -> %s): %s."), pSize,
				lLocalAddress.c_str(), mTargetAddress.GetAsString().c_str(), lData.c_str()));

			mSentByteCount += lSentByteCount;
		}
	}
	return (lSentByteCount);
}

int TcpSocket::Receive(void* pData, int pMaxSize)
{
	int lSize = -1;
	if (mSocket != INVALID_SOCKET)
	{
		if (pMaxSize == mUnreceivedByteCount)
		{
			::memcpy(pData, mUnreceivedArray, pMaxSize);
			mUnreceivedByteCount = 0;
			lSize = (int)pMaxSize;
		}
		else if (mUnreceivedByteCount == 0)
		{
			lSize = ::recv(mSocket, (char*)pData, (int)pMaxSize, 0);
		}
		else
		{
			mLog.AError("Tried to ::recv() data, but unreceived data size don't match!");
			lSize = 0;
		}
		if (lSize == 0)
		{
			// Disconnected.
			CloseKeepHandle();
			lSize = -1;
		}
		else if(lSize == SOCKET_ERROR)
		{
			// There's no data...
			lSize = 0;
		}
		else
		{
			log_volatile(str lLocalAddress);
			log_volatile(if (mServerSocket) lLocalAddress = mServerSocket->GetLocalAddress().GetAsString());
			log_volatile(str lData = strutil::DumpData((uint8*)pData, std::min(lSize, 20)));
			log_volatile(mLog.Tracef(_T("TCP <- %u bytes (%s <- %s): %s."), lSize,
				lLocalAddress.c_str(), mTargetAddress.GetAsString().c_str(), lData.c_str()));

			mReceivedByteCount += lSize;
		}
	}
	return (lSize);
}

bool TcpSocket::Unreceive(void* pData, int pByteCount)
{
	assert(mUnreceivedByteCount == 0);
	assert(pByteCount <= (int)sizeof(mUnreceivedArray));
	bool lOk = (mUnreceivedByteCount == 0 && pByteCount <= (int)sizeof(mUnreceivedArray));
	if (lOk)
	{
		mUnreceivedByteCount = pByteCount;
		::memcpy(mUnreceivedArray, pData, pByteCount);
	}
	if (!lOk)
	{
		mLog.Errorf(_T("Unable to unreceive %u bytes."), pByteCount);
	}
	return (lOk);
}

void TcpSocket::SetDatagramReceiver(DatagramReceiver* pReceiver)
{
	mReceiver = pReceiver;
}

int TcpSocket::ReceiveDatagram(void* pData, int pMaxSize)
{
	int lReceivedByteCount;
	if (mReceiver)
	{
		lReceivedByteCount = mReceiver->Receive(this, pData, pMaxSize);
	}
	else
	{
		lReceivedByteCount = Receive(pData, pMaxSize);
	}
	return (lReceivedByteCount);
}

const SocketAddress& TcpSocket::GetTargetAddress() const
{
	return (mTargetAddress);
}

LOG_CLASS_DEFINE(NETWORK, TcpSocket);



TcpMuxSocket::TcpMuxSocket(const str& pName, const SocketAddress& pLocalAddress, bool pIsServer,
	unsigned pMaxPendingConnectionCount, unsigned pMaxConnectionCount):
	MuxIo(pMaxPendingConnectionCount, pMaxConnectionCount),
	TcpListenerSocket(pLocalAddress),
	mAcceptThread(pName+_T("TcpMuxAccept ")+pLocalAddress.GetAsString()),
	mSelectThread(pName+_T("TcpMuxSelect ")+pLocalAddress.GetAsString()),
	mConnectIdTimeout(DEFAULT_CONNECT_ID_TIMEOUT),
	mActiveReceiverMapChanged(false),
	mConnectedSocketSemaphore(100),
	mVSentByteCount(0),
	mVReceivedByteCount(0)
{
	log_atrace("TcpMuxSocket()");
	if (pIsServer)
	{
		mAcceptThread.Start(this, &TcpMuxSocket::AcceptThreadEntry);
	}
	mSelectThread.Start(this, &TcpMuxSocket::SelectThreadEntry);
}

TcpMuxSocket::~TcpMuxSocket()
{
	log_atrace("~TcpMuxSocket()");
	mAcceptThread.RequestStop();
	mSelectThread.RequestStop();
	Close();
	ReleaseSocketThreads();
	mAcceptThread.Join(5.0f);
	mSelectThread.Join(5.0f);
}

TcpVSocket* TcpMuxSocket::Connect(const SocketAddress& pTargetAddress, const std::string& pConnectionId, double pTimeout)
{
	TcpVSocket* lSocket = new TcpVSocket(*this, GetDatagramReceiver());
	bool lOk = lSocket->Connect(pTargetAddress, pConnectionId, pTimeout);
	if (lOk)
	{
		std::string lConnectString(mConnectionString, sizeof(mConnectionString));
		lConnectString += pConnectionId;
		lOk = (lSocket->Send(lConnectString.c_str(), (int)lConnectString.length()) == (int)lConnectString.length());
		if (!lOk)
		{
			mLog.AError("Could not send connect data to server!");
		}
	}
	if (lOk)
	{
		Datagram lBuffer;
		lBuffer.mDataSize = lSocket->Receive(lBuffer.mDataBuffer, sizeof(lBuffer.mDataBuffer), pTimeout, false);
		lOk = (lBuffer.mDataSize == sizeof(mAcceptionString) && ::memcmp(mAcceptionString, lBuffer.mDataBuffer, sizeof(mAcceptionString)) == 0);
		if (lOk)
		{
			AddConnectedSocket(lSocket);
			log_atrace("Connect went through!");
		}
		else
		{
			if (lBuffer.mDataSize == 0)
			{
				mLog.AInfo("Remote end seems dead. Firewall?");
			}
			else if (lBuffer.mDataSize < 0)
			{
				mLog.AError("Connect was refused. Firewall?");
			}
			else if (lBuffer.mDataSize > 0)
			{
				mLog.AError("Connect was replied to with jibberish. Wassup?");
			}
		}
	}
	if (!lOk)
	{
		CloseSocket(lSocket, true);
		lSocket = 0;
	}
	return (lSocket);
}

TcpVSocket* TcpMuxSocket::Accept()
{
	TcpVSocket* lSocket = 0;
	while (IsOpen() && (lSocket = PollAccept()) == 0)
	{
		const double lWaitTime = (mPendingConnectIdMap.GetCountSafe() > 0)? 0.001 : 10.0;
		mAcceptSemaphore.Wait(lWaitTime);
	}
	return (lSocket);
}

TcpVSocket* TcpMuxSocket::PollAccept()
{
	Timer lTime;
	TcpVSocket* lTcpSocket = 0;
	size_t lPendingSocketCount = mPendingConnectIdMap.GetCountSafe();
	for (size_t x = 0; !lTcpSocket && x < lPendingSocketCount; ++x)
	{
		{
			ScopeLock lLock(&mIoLock);
			if (mPendingConnectIdMap.GetCount() > 0)
			{
				mPendingConnectIdMap.PopFront(lTcpSocket, lTime);
			}
		}
		if (lTcpSocket)
		{
			log_atrace("Popped a connected socket.");
			AcceptStatus lAcceptStatus = QueryReceiveConnectString(lTcpSocket);
			if (lAcceptStatus == ACCEPT_CLOSE)
			{
				CloseSocket(lTcpSocket, true);
				lTcpSocket = 0;
			}
			else if (lAcceptStatus == ACCEPT_QUERY_WAIT)
			{
				lTime.UpdateTimer();
				if (lTime.GetTimeDiffF() >= mConnectIdTimeout)
				{
					log_adebug("Connected socket ID-timed out => dropped.");
					CloseSocket(lTcpSocket, true);
				}
				else
				{
					ScopeLock lLock(&mIoLock);
					mPendingConnectIdMap.PushBack(lTcpSocket, lTime);
				}
				lTcpSocket = 0;
			}
		}
	}
	return (lTcpSocket);
}

void TcpMuxSocket::DispatchCloseSocket(TcpVSocket* pSocket)
{
	if (!mCloseDispatcher.empty())
	{
		mCloseDispatcher(pSocket);
	}
	CloseSocket(pSocket, false);
}

void TcpMuxSocket::CloseSocket(TcpVSocket* pSocket, bool pForceDelete)
{
	{
		ScopeLock lLock(&mIoLock);
		log_atrace("Closing TcpVSocket.");
		pForceDelete |= mPendingConnectIdMap.Remove(pSocket);
		pForceDelete |= RemoveConnectedSocketNoLock(pSocket);
		RemoveSenderNoLock(pSocket);
		RemoveReceiverNoLock(pSocket);
	}
	if (pForceDelete)
	{
		delete (pSocket);
	}
}

TcpVSocket* TcpMuxSocket::PopReceiverSocket()
{
	TcpVSocket* lSocket = (TcpVSocket*)PopReceiver();
	if (lSocket)
	{
		mActiveReceiverMapChanged = true;
		mConnectedSocketSemaphore.Signal();
	}
	return (lSocket);
}

TcpVSocket* TcpMuxSocket::PopSenderSocket()
{
	TcpVSocket* lSocket = (TcpVSocket*)PopSender();
	if (lSocket)
	{
		log_atrace("Popped TCP sender socket.");
	}
	return (lSocket);
}

uint64 TcpMuxSocket::GetTotalSentByteCount() const
{
	return (GetSentByteCount()+mVSentByteCount);
}

uint64 TcpMuxSocket::GetTotalReceivedByteCount() const
{
	return (GetReceivedByteCount()+mVReceivedByteCount);
}

void TcpMuxSocket::AddTotalSentByteCount(unsigned pByteCount)
{
	BusLock::Add(&mVSentByteCount, pByteCount);
}

void TcpMuxSocket::AddTotalReceivedByteCount(unsigned pByteCount)
{
	BusLock::Add(&mVReceivedByteCount, pByteCount);
}

void TcpMuxSocket::SetConnectIdTimeout(double pTimeout)
{
	mConnectIdTimeout = pTimeout;
}

TcpSocket* TcpMuxSocket::CreateSocket(sys_socket pSocket, const SocketAddress& pTargetAddress,
	TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver)
{
	return (new TcpVSocket(pSocket, pTargetAddress, *(TcpMuxSocket*)pServerSocket, pReceiver));
}

void TcpMuxSocket::AddConnectedSocket(TcpVSocket* pSocket)
{
	ScopeLock lLock(&mIoLock);
	log_volatile(mLog.Tracef(_T("AddConnectedSocket(%i)"), pSocket->GetSysSocket()));
	mConnectedSocketMap.insert(std::pair<sys_socket, TcpVSocket*>(pSocket->GetSysSocket(), pSocket));
	mActiveReceiverMapChanged = true;
	mConnectedSocketSemaphore.Signal();
}

bool TcpMuxSocket::RemoveConnectedSocketNoLock(TcpVSocket* pSocket)
{
	log_volatile(mLog.Tracef(_T("RemoveConnectedSocketNoLock(%i)"), pSocket->GetSysSocket()));
	size_t lEraseCount = mConnectedSocketMap.erase(pSocket->GetSysSocket());
	mActiveReceiverMapChanged = true;
	mConnectedSocketSemaphore.Signal();
	return (lEraseCount > 0);
}

int TcpMuxSocket::BuildConnectedSocketSet(FdSet& pSocketSet)
{
	int lSocketCount;
	if (mActiveReceiverMapChanged)
	{
		ScopeLock lLock(&mIoLock);
		mActiveReceiverMapChanged = false;
		//lSocketCount = mConnectedSocketMap.size();
		//log_volatile(mLog.Tracef(_T("Rebuilding active receiver map with %i possible sockets."), lSocketCount));
		LEPRA_FD_ZERO(&pSocketSet);
		for (SocketVMap::iterator x = mConnectedSocketMap.begin(); x != mConnectedSocketMap.end(); ++x)
		{
			TcpVSocket* lSocket = x->second;
			if (!IsReceiverNoLock(lSocket))
			{
				sys_socket lSysSocket = x->first;
#pragma warning(push)
#pragma warning(disable: 4127)	// MSVC warning: conditional expression is constant.
				LEPRA_FD_SET(lSysSocket, &pSocketSet);
#pragma warning(pop)
			}
		}
		lSocketCount = FdSetHelper::Copy(mBackupFdSet, pSocketSet);
	}
	else
	{
		lSocketCount = FdSetHelper::Copy(pSocketSet, mBackupFdSet);
	}
	assert(lSocketCount == (int)LEPRA_FD_GET_COUNT(&pSocketSet));
	return (lSocketCount);
}

void TcpMuxSocket::PushReceiverSockets(const FdSet& pSocketSet)
{
	ScopeLock lLock(&mIoLock);
	// TODO: optimize (using platform specifics?).
	bool lAdded = false;
	for (SocketVMap::iterator y = mConnectedSocketMap.begin(); y != mConnectedSocketMap.end(); ++y)
	{
		sys_socket lSysSocket = y->first;
		if (FD_ISSET(lSysSocket, LEPRA_FDS(&pSocketSet)))
		{
			TcpVSocket* lSocket = y->second;
			log_adebug("Adding receiver socket.");
			AddReceiverNoLock(lSocket);
			lAdded = true;
		}
		else
		{
			log_adebug("Didn't add receiver socket.");
		}
	}
	if (lAdded)
	{
		mActiveReceiverMapChanged = true;
		mConnectedSocketSemaphore.Signal();
	}
}

TcpMuxSocket::AcceptStatus TcpMuxSocket::QueryReceiveConnectString(TcpVSocket* pSocket)
{
	if (mBannedIPTable.Find(pSocket->GetTargetAddress().GetIP()) != mBannedIPTable.End())
	{
		mLog.AWarning("Received a connect from a banned client.");
		return (ACCEPT_CLOSE);	// RAII is great here.
	}

	// Check if we get the connection string (containing an ID that connects the TCP and UDP sockets).
	AcceptStatus lAcceptStatus = ACCEPT_QUERY_WAIT;
	Datagram lBuffer;
	lBuffer.mDataSize = pSocket->Receive(lBuffer.mDataBuffer, sizeof(lBuffer.mDataBuffer), false);
	if (lBuffer.mDataSize > 0)
	{
		log_atrace("Received a connect string.");

		// Look for "VSocket connect magic".
		if (lBuffer.mDataSize >= (int)sizeof(mConnectionString) &&
			::memcmp(lBuffer.mDataBuffer, mConnectionString, sizeof(mConnectionString)) == 0)
		{
			std::string lConnectionId((const char*)&lBuffer.mDataBuffer[sizeof(mConnectionString)], lBuffer.mDataSize-sizeof(mConnectionString));
			unsigned lSocketCount = GetConnectionCount();
			if (lSocketCount < mMaxConnectionCount)
			{
				pSocket->SetConnectionId(lConnectionId);
				if (pSocket->Send(mAcceptionString, sizeof(mAcceptionString)) == sizeof(mAcceptionString))
				{
					lAcceptStatus = ACCEPT_OK;
					AddConnectedSocket(pSocket);
				}
				else
				{
					lAcceptStatus = ACCEPT_CLOSE;
				}
			}
			else
			{
				mLog.AWarning("Too many sockets!");
				lAcceptStatus = ACCEPT_CLOSE;
			}
		}
		else
		{
			mLog.AWarning("Invalid connection string.");
			lAcceptStatus = ACCEPT_CLOSE;
		}
	}
	else
	{
		log_atrace("Waited for connect magic, but none came this loop.");
	}
	return (lAcceptStatus);
}

void TcpMuxSocket::ReleaseSocketThreads()
{
	MuxIo::ReleaseSocketThreads();
	for (int x = 0; x < 10; ++x)
	{
		mConnectedSocketSemaphore.Signal();
	}
}

void TcpMuxSocket::AcceptThreadEntry()
{
	log_atrace("Accept thread running");

	while (IsOpen() && !mAcceptThread.GetStopRequest())
	{
		TcpVSocket* lSocket = (TcpVSocket*)TcpListenerSocket::Accept(&TcpMuxSocket::CreateSocket);
		if (lSocket)
		{
			log_atrace("Received a connect.");
			Timer lTime;
			ScopeLock lLock(&mIoLock);
			mPendingConnectIdMap.PushBack(lSocket, lTime);
			mAcceptSemaphore.Signal();
		}
	}
}

void TcpMuxSocket::SelectThreadEntry()
{
	log_atrace("Select thread running");

	FdSet lReadSet;
	LEPRA_FD_ZERO(&mBackupFdSet);
	mActiveReceiverMapChanged = true;
	while (IsOpen() && !mSelectThread.GetStopRequest())
	{
		int lSocketCount = BuildConnectedSocketSet(lReadSet);
		if (lSocketCount > 0)
		{
			timeval lTimeout;
			lTimeout.tv_sec = 0;
			lTimeout.tv_usec = 200000;
			assert(lSocketCount == (int)LEPRA_FD_GET_COUNT(&lReadSet));
			FdSet lExceptionSet;
			FdSetHelper::Copy(lExceptionSet, lReadSet);
			int lSelectCount = ::select(LEPRA_FD_GET_MAX_HANDLE(&lReadSet)+1, LEPRA_FDS(&lReadSet), 0, LEPRA_FDS(&lExceptionSet), &lTimeout);
			if (lSelectCount > 0)
			{
				log_atrace("Picked up a receive socket.");
				PushReceiverSockets(lReadSet);
			}
			else if (lSelectCount < 0)
			{
				int e = SOCKET_LAST_ERROR();
				mLog.Warningf(_T("Could not ::select() properly. Error=%i, sockets=%u, exception set=%u."), e, lSocketCount, LEPRA_FD_GET_COUNT(&lExceptionSet));

				for (u_int x = 0; x < LEPRA_FD_GET_COUNT(&lExceptionSet); ++x)
				{
					const sys_socket lSysSocket = LEPRA_FD_GET(&lExceptionSet, x);
					int lErrorCode = 0;
					socklen_t lErrorSize = sizeof(lErrorCode);
					if (::getsockopt(lSysSocket, SOL_SOCKET, SO_ERROR, (char*)&lErrorCode, &lErrorSize) == 0 &&
						lErrorCode == 0)
					{
						continue;
					}

					TcpVSocket* lSocket = 0;
					{
						ScopeLock lLock(&mIoLock);
						SocketVMap::iterator y = mConnectedSocketMap.find(lSysSocket);
						if (y != mConnectedSocketMap.end())
						{
							lSocket = y->second;
						}
					}
					if (lSocket)
					{
						mLog.AWarning("Kicking socket due to ::select() failure.");
						DispatchCloseSocket(lSocket);
					}
				}
			}
		}
		else
		{
			log_debug(_T("Going into \"wait for socket connect\" state."));
			mConnectedSocketSemaphore.Wait(10.0f);
			log_debug(_T("Leaving \"wait for socket connect\" state."));
		}
	}
}

const double TcpMuxSocket::DEFAULT_CONNECT_ID_TIMEOUT = 2.0;

LOG_CLASS_DEFINE(NETWORK, TcpMuxSocket);



TcpVSocket::TcpVSocket(TcpMuxSocket& pMuxSocket, DatagramReceiver* pReceiver):
	TcpSocket(pReceiver)
{
	log_atrace("TcpVSocket()");
	mMuxIo = &pMuxSocket;
}

TcpVSocket::TcpVSocket(sys_socket pSocket, const SocketAddress& pTargetAddress, TcpMuxSocket& pMuxSocket,
	DatagramReceiver* pReceiver):
	TcpSocket(pSocket, pTargetAddress, &pMuxSocket, pReceiver)
{
	log_atrace("TcpVSocket()");
	mMuxIo = &pMuxSocket;
	MakeNonBlocking();
}

TcpVSocket::~TcpVSocket()
{
	log_atrace("~TcpVSocket()");
	SendBuffer();
}

bool TcpVSocket::Connect(const SocketAddress& pTargetAddress, const std::string& pConnectionId, double /*pTimeout*/)
{
	// TODO: respect lTimeout?
	SetConnectionId(pConnectionId);
	bool lConnected = TcpSocket::Connect(pTargetAddress);
	if (lConnected)
	{
		MakeNonBlocking();
		DisableNagleAlgo();
		// TODO: send connection ID to acceptor.
	}
	return (lConnected);
}

int TcpVSocket::SendBuffer()
{
	int lSendResult = 0;
	if (mSendBuffer.mDataSize > 0)
	{
		lSendResult = Send(mSendBuffer.mDataBuffer, mSendBuffer.mDataSize);
		if (lSendResult > 0)
		{
			((TcpMuxSocket*)mMuxIo)->AddTotalSentByteCount(lSendResult);
		}
		mSendBuffer.Init();
	}
	return (lSendResult);
}

int TcpVSocket::Receive(void* pData, int pMaxSize, bool pDatagram)
{
	int lReceiveResult = pDatagram? TcpSocket::ReceiveDatagram(pData, pMaxSize) : TcpSocket::Receive(pData, pMaxSize);
	if (lReceiveResult > 0)
	{
		((TcpMuxSocket*)mMuxIo)->AddTotalReceivedByteCount(lReceiveResult);
	}
	return (lReceiveResult);
}

int TcpVSocket::Receive(void* pData, int pMaxSize, double pTimeout, bool pDatagram)
{
	HiResTimer lTimer;
	int lReceiveCount;
	do
	{
		lReceiveCount = Receive(pData, pMaxSize, pDatagram);
		if (lReceiveCount == 0)
		{
			Thread::Sleep(0.001);
			lTimer.UpdateTimer();
		}
	}
	while (lReceiveCount == 0 && lTimer.GetTimeDiff() < pTimeout && !SystemManager::GetQuitRequest());
	return (lReceiveCount);
}

LOG_CLASS_DEFINE(NETWORK, TcpVSocket);



UdpSocket::UdpSocket(const SocketAddress& pLocalAddress) :
	mLocalAddress(pLocalAddress)
{
	// Initialize UDP socket.
	mSocket = CreateUdpSocket();

	if (mSocket != INVALID_SOCKET)
	{
		if (::bind(mSocket, (const sockaddr*)&mLocalAddress.GetAddr(), sizeof(mLocalAddress.GetAddr())) != 0)
		{
			mLog.Warning(_T("Failed to bind UDP socket to ")+pLocalAddress.GetAsString()+_T("."));
			Close();
		}
	}
}

UdpSocket::UdpSocket(const UdpSocket& pSocket):
	mLocalAddress(pSocket.mLocalAddress)
{
	log_atrace("UdpSocket()");
}

UdpSocket::~UdpSocket()
{
	log_atrace("~UdpSocket()");
}

const SocketAddress& UdpSocket::GetLocalAddress() const
{
	return (mLocalAddress);
}

int UdpSocket::SendTo(const uint8* pData, unsigned pSize, const SocketAddress& pTargetAddress)
{
	int lSentByteCount = 0;
	if (mSocket != INVALID_SOCKET && pData != 0 && pSize > 0)
	{
		lSentByteCount = ::sendto(mSocket, (const char*)pData, pSize, 0, (const sockaddr*)&pTargetAddress.GetAddr(), sizeof(pTargetAddress.GetAddr()));
		if (lSentByteCount != (int)pSize)
		{
			CloseKeepHandle();
		}
		else
		{
			log_volatile(str lData = strutil::DumpData((uint8*)pData, std::min(pSize, (unsigned)20)));
			log_volatile(mLog.Tracef(_T("UDP -> %u bytes (%s -> %s): %s."), pSize,
				mLocalAddress.GetAsString().c_str(), pTargetAddress.GetAsString().c_str(),
				lData.c_str()));
			mSentByteCount += lSentByteCount;
		}
	}

	return (lSentByteCount);
}

int UdpSocket::ReceiveFrom(uint8* pData, unsigned pMaxSize, SocketAddress& pSourceAddress)
{
	int lSize = 0;
	if (mSocket != INVALID_SOCKET && pData != 0 && pMaxSize > 0)
	{
		socklen_t lAddrSize = (socklen_t)sizeof(pSourceAddress.GetAddr());
		lSize = ::recvfrom(mSocket, (char*)pData, pMaxSize, 0, (sockaddr*)&pSourceAddress.GetAddr(), &lAddrSize);
		if (lSize <= 0)
		{
			// Disconnected.
			lSize = -1;
		}
		else
		{
			log_volatile(str lData = strutil::DumpData((uint8*)pData, std::min(lSize, 20)));
			log_volatile(mLog.Tracef(_T("UDP <- %u bytes (%s <- %s): %s."), lSize,
				mLocalAddress.GetAsString().c_str(), pSourceAddress.GetAsString().c_str(),
				lData.c_str()));
			mReceivedByteCount += lSize;
		}
	}
	return (lSize);
}

LOG_CLASS_DEFINE(NETWORK, UdpSocket);



UdpMuxSocket::UdpMuxSocket(const str& pName, const SocketAddress& pLocalAddress,
	unsigned pMaxPendingConnectionCount, unsigned pMaxConnectionCount):
	MuxIo(pMaxPendingConnectionCount, pMaxConnectionCount),
	Thread(pName+_T("UdpMuxRecv ")+pLocalAddress.GetAsString()),
	UdpSocket(pLocalAddress)
{
	log_atrace("UdpMuxSocket()");

	if (IsOpen())
	{
		Start();
	}
}

UdpMuxSocket::~UdpMuxSocket()
{
	log_atrace("~UdpMuxSocket()");

	RequestStop();

	sys_socket lKiller = CreateUdpSocket();
	const SocketAddress& lAddress = GetLocalAddress();
	const int lReleaseByteCount = 8;
	::sendto(lKiller, "Release!", lReleaseByteCount, 0, (const sockaddr*)&lAddress.GetAddr(), sizeof(lAddress.GetAddr()));
	Thread::Sleep(0.01);
	CloseSysSocket(lKiller);

	Close();
	ReleaseSocketThreads();
	Signal(0);
	Join(5.0f);
}

UdpVSocket* UdpMuxSocket::Connect(const SocketAddress& pTargetAddress, const std::string& pConnectionId, double pTimeout)
{
	UdpVSocket* lSocket = 0;
	{
		ScopeLock lLock(&mIoLock);
		SocketTable::Iterator lIter;
		lIter = mSocketTable.Find(pTargetAddress);
		if (lIter != mSocketTable.End())
		{
			lSocket = *lIter;
			if (lSocket->GetTargetAddress() != pTargetAddress)
			{
				assert(false);
				lSocket = 0;
			}
		}
		else
		{
			lSocket = mSocketAllocator.Alloc();
			lSocket->Init(*this, pTargetAddress, pConnectionId);
			mSocketTable.Insert(pTargetAddress, lSocket);
			std::string lConnectString(mConnectionString, sizeof(mConnectionString));
			lConnectString += pConnectionId;
			if (lSocket->DirectSend(lConnectString.c_str(), (int)lConnectString.length()) != (int)lConnectString.length())
			{
				mLog.AError("Send to server (as connect) failed.");
				CloseSocket(lSocket);
				lSocket = 0;
			}
		}
	}
	if (lSocket)
	{
		lSocket->WaitAvailable(pTimeout);
		Datagram lBuffer;
		lBuffer.mDataSize = lSocket->Receive(lBuffer.mDataBuffer, sizeof(lBuffer.mDataBuffer));
		if (lBuffer.mDataSize != sizeof(mAcceptionString) || ::memcmp(mAcceptionString, lBuffer.mDataBuffer, sizeof(mAcceptionString)) != 0)
		{
			if (lBuffer.mDataSize == 0)
			{
				mLog.AInfo("Remote end seems dead. Firewall?");
			}
			else if (lBuffer.mDataSize < 0)
			{
				mLog.AError("Connect was refused. Firewall?");
			}
			else if (lBuffer.mDataSize > 0)
			{
				mLog.AError("Connect was replied to with jibberish. Wassup?");
			}
			CloseSocket(lSocket);
			lSocket = 0;
		}
		else
		{
			log_atrace("Connect went through!");
			lSocket->SetReceiverFollowupActive(true);
		}
	}
	return (lSocket);
}

UdpVSocket* UdpMuxSocket::Accept()
{
	// Leave this function unlocked, since we are not accessing any critical data.
	UdpVSocket* lSocket = 0;
	while (IsOpen() && (lSocket = PollAccept()) == 0)
	{
		mAcceptSemaphore.Wait();
	}
	return (lSocket);
}

UdpVSocket* UdpMuxSocket::PollAccept()
{
	UdpVSocket* lSocket = 0;
	ScopeLock lLock(&mIoLock);
	if (mAcceptList.empty() == false)
	{
		// Move the socket from the accept list & table to the socket table.
		lSocket = mAcceptList.front();
		mAcceptList.pop_front();
		lSocket->SetReceiverFollowupActive(true);
		mAcceptTable.Remove(lSocket->GetTargetAddress());
		if (lSocket->DirectSend(mAcceptionString, sizeof(mAcceptionString)) == sizeof(mAcceptionString))
		{
			log_atrace("Replied to connect with an ACK.");
			mSocketTable.Insert(lSocket->GetTargetAddress(), lSocket);
		}
		else
		{
			mLog.AError("Could not reply to connect with an ACK.");
			// TODO: blacklist after x number of incorrect queries?
			CloseSocket(lSocket);
			lSocket = 0;
		}
	}

	return (lSocket);
}

void UdpMuxSocket::CloseSocket(UdpVSocket* pSocket)
{
	ScopeLock lLock(&mIoLock);
	mSocketTable.Remove(pSocket->GetTargetAddress());
	RemoveSenderNoLock(pSocket);
	RemoveReceiverNoLock(pSocket);

	pSocket->ClearAll();

	mSocketAllocator.Free(pSocket);
}

unsigned UdpMuxSocket::GetConnectionCount() const
{
	return (mSocketTable.GetCount());
}

UdpVSocket* UdpMuxSocket::PopReceiverSocket()
{
	UdpVSocket* lSocket = (UdpVSocket*)PopReceiver();
	return (lSocket);
}

UdpVSocket* UdpMuxSocket::PopSenderSocket()
{
	UdpVSocket* lSocket = (UdpVSocket*)PopSender();
	if (lSocket)
	{
		log_atrace("Popped UDP sender socket.");
	}
	return (lSocket);
}

void UdpMuxSocket::Run()
{
	log_atrace("Receive thread running");

	SocketAddress lSourceAddress;
	Datagram* lBuffer = 0;

	while (IsOpen() && !GetStopRequest())
	{
		if (!lBuffer)
		{
			// Allocate a new buffer in which we store the received data.
			ScopeLock lLock(&mIoLock);
			lBuffer = mBufferAllocator.Alloc();
		}
		lBuffer->Init();

		// Wait for data. This will block the thread forever if no data arrives.
		lBuffer->mDataSize = ReceiveFrom(lBuffer->mDataBuffer, BUFFER_SIZE, lSourceAddress);
		if (lBuffer->mDataSize > 0)
		{
			ScopeLock lLock(&mIoLock);

			// Check if the IP is already 'connected'.
			SocketTable::Iterator lIter = mSocketTable.Find(lSourceAddress);
			if (lIter != mSocketTable.End())
			{
				// Already connected.
				UdpVSocket* lSocket = *lIter;
				lSocket->AddInputBuffer(lBuffer);
				lBuffer = 0;
			}
			else if(mBannedIPTable.Find(lSourceAddress.GetIP()) != mBannedIPTable.End())
			{
				mLog.AWarning("Banned socket sent us something. Playing ignorant.");
			}
			else if(mAcceptTable.Find(lSourceAddress) == mAcceptTable.End())
			{
				// Look for "VSocket connect magic".
				if (lBuffer->mDataSize >= (int)sizeof(mConnectionString) &&
					::memcmp(lBuffer->mDataBuffer, mConnectionString, sizeof(mConnectionString)) == 0)
				{
					std::string lConnectionId((const char*)&lBuffer->mDataBuffer[sizeof(mConnectionString)], lBuffer->mDataSize-sizeof(mConnectionString));
					unsigned lAcceptCount = (unsigned)mAcceptList.size();
					unsigned lSocketCount = (unsigned)mSocketTable.GetCount();
					// TODO: drop old acceptance socket instead of new?
					if (lAcceptCount < mMaxPendingConnectionCount &&
						(lAcceptCount + lSocketCount) < mMaxConnectionCount)
					{
						UdpVSocket* lSocket = mSocketAllocator.Alloc();
						lSocket->Init(*this, lSourceAddress, lConnectionId);

						mAcceptList.push_back(lSocket);
						mAcceptTable.Insert(lSourceAddress, lSocket);

						mAcceptSemaphore.Signal();
					}
					else
					{
						mLog.AWarning("Too many sockets - didn't accept connect.");
					}
				}
				else
				{
					mLog.AWarning("Non-connected socket sent us junk.");
				}
			}
			else
			{
				mLog.AWarning("Non-connecting send us something. (Could mean internal error...)");
			}
		}
		else if (lBuffer->mDataSize < 0)
		{
			// Socket died.
		}
		else
		{
			mLog.AWarning("Could not receive any data on the socket.");
		}
	}

	if (lBuffer)
	{
		RecycleBuffer(lBuffer);
	}
}

void UdpMuxSocket::RecycleBuffer(Datagram* pBuffer)
{
	ScopeLock lLock(&mIoLock);
	mBufferAllocator.Free(pBuffer);
}

LOG_CLASS_DEFINE(NETWORK, UdpMuxSocket);



UdpVSocket::UdpVSocket()
{
	log_atrace("UdpVSocket()");
	ClearAll();
}

UdpVSocket::~UdpVSocket()
{
	log_atrace("~UdpVSocket()");
}

void UdpVSocket::ClearAll()
{
	if (mMuxIo)
	{
		SendBuffer();
	}

	mTargetAddress = SocketAddress();
	while (mReceiveBufferList.GetCount() != 0)
	{
		Receive(0, 0);
	}
	mMuxIo = 0;
	mReceiveBufferList.Clear();
	mRawReadBufferIndex = 0;
	ClearOutputData();
	SetInSenderList(false);
	mReceiverFollowupActive = false;
}

void UdpVSocket::Init(UdpMuxSocket& pSocket, const SocketAddress& pTargetAddress, const std::string& pConnectionId)
{
	ClearAll();

	mMuxIo = &pSocket;
	mTargetAddress = pTargetAddress;
	SetConnectionId(pConnectionId);
}

int UdpVSocket::Receive(void* pData, int pLength)
{
	int lReadSize = 0;
	if (mReceiveBufferList.GetCount() > 0)
	{
		// Optimizing, deadlock avoiding code.
		Datagram* lReceiveBuffer = 0;
		{
			ScopeLock lLock(&mLock);
			if (mReceiveBufferList.GetCount() > 0)
			{
				lReceiveBuffer = mReceiveBufferList[0];
				mReceiveBufferList.PopFront();
			}
		}

		if (lReceiveBuffer)
		{
			lReadSize = lReceiveBuffer->mDataSize;
			if (lReceiveBuffer->mDataSize > 0)
			{
				lReadSize = (int)std::min(lReceiveBuffer->mDataSize, (int)pLength);
				::memcpy(pData, lReceiveBuffer->mDataBuffer, lReadSize);
			}
			((UdpMuxSocket*)mMuxIo)->RecycleBuffer(lReceiveBuffer);
		}
	}
	return (lReadSize);
}

int UdpVSocket::SendBuffer()
{
	int lSendResult = 0;
	if (mSendBuffer.mDataSize > 0)
	{
		lSendResult = ((UdpMuxSocket*)mMuxIo)->SendTo(mSendBuffer.mDataBuffer, mSendBuffer.mDataSize, mTargetAddress);
		mSendBuffer.Init();
	}
	return (lSendResult);
}

int UdpVSocket::DirectSend(const void* pData, int pLength)
{
	return (((UdpMuxSocket*)mMuxIo)->SendTo((const uint8*)pData, pLength, mTargetAddress));
}

const SocketAddress& UdpVSocket::GetLocalAddress() const
{
	return (((UdpMuxSocket*)mMuxIo)->GetLocalAddress());
}

const SocketAddress& UdpVSocket::GetTargetAddress() const
{
	return (mTargetAddress);
}

void UdpVSocket::TryAddReceiverSocket()
{
	if (NeedInputPeek())
	{
		((UdpMuxSocket*)mMuxIo)->AddReceiver(this);
	}
}

void UdpVSocket::AddInputBuffer(Datagram* pBuffer)
{
	int lPreInsertCount;
	{
		ScopeLock lLock(&mLock);
		lPreInsertCount = (int)mReceiveBufferList.GetCount();
		if (lPreInsertCount < MAX_INPUT_BUFFERS)
		{
			mReceiveBufferList.PushBack(pBuffer);
		}
		else
		{
			// The array is full and we have to throw the data away. This should be ok
			// since UDP is unsafe per definition, and the user thread can't keep
			// up processing the data anyway.

			// Throw old data away.
			Datagram* lReceiveBuffer = mReceiveBufferList[0];
			mReceiveBufferList.PopFront();
			mLock.Release();
			((UdpMuxSocket*)mMuxIo)->RecycleBuffer(lReceiveBuffer);
			mLock.Acquire();
			mReceiveBufferList.PushBack(pBuffer);
			mRawReadBufferIndex = 0;
		}
	}

	// If this is our first input packet, we add us to the list of sockets.
	if (lPreInsertCount == 0 && mReceiverFollowupActive)
	{
		((UdpMuxSocket*)mMuxIo)->AddReceiver(this);
	}
}

bool UdpVSocket::NeedInputPeek() const
{
	return (mReceiveBufferList.GetCount() != 0);
}

void UdpVSocket::SetReceiverFollowupActive(bool pActive)
{
	mReceiverFollowupActive = pActive;
}

bool UdpVSocket::WaitAvailable(double pTimeout)
{
	HiResTimer lTimer;
	while (GetAvailable() == 0 && lTimer.GetTimeDiff() < pTimeout && !SystemManager::GetQuitRequest())
	{
		Thread::Sleep(Math::Clamp(pTimeout/10.0, 0.001, 60*60.0));
		lTimer.UpdateTimer();
	}
	return (GetAvailable() != 0);
}

// Stream interface-inherited dummy method.
void UdpVSocket::Close()
{
}

int64 UdpVSocket::GetAvailable() const
{
	int lReadSize = 0;
	ScopeLock lLock(&mLock);
	if (mReceiveBufferList.GetCount() > 0)
	{
		Datagram* lReceiveBuffer = mReceiveBufferList[0];
		lReadSize = lReceiveBuffer->mDataSize;
	}
	return (lReadSize);
}

IOError UdpVSocket::ReadRaw(void* pData, size_t pLength)
{
	IOError lResult = IO_NO_DATA_AVAILABLE;
	ScopeLock lLock(&mLock);
	while (mReceiveBufferList.GetCount() > 0)
	{
		Datagram* lReceiveBuffer = mReceiveBufferList[0];
		if ((int)(mRawReadBufferIndex+pLength) <= lReceiveBuffer->mDataSize)
		{
			::memcpy(pData, &lReceiveBuffer->mDataBuffer[mRawReadBufferIndex], pLength);
			if ((int)(mRawReadBufferIndex+pLength) == lReceiveBuffer->mDataSize)
			{
				mReceiveBufferList.PopFront();
				mRawReadBufferIndex = 0;
			}
			else
			{
				mRawReadBufferIndex += (int)pLength;
			}
			lResult = IO_OK;
			break;
		}
	}
	return (lResult);
}

IOError UdpVSocket::Skip(size_t /*pLength*/)
{
	IOError lResult = IO_NO_DATA_AVAILABLE;
	ScopeLock lLock(&mLock);
	if (mReceiveBufferList.GetCount() > 0)
	{
		mReceiveBufferList.PopFront();
		lResult = IO_OK;
	}
	return (lResult);
}

IOError UdpVSocket::WriteRaw(const void* pData, size_t pLength)
{
	return (AppendSendBuffer(pData, (int)pLength));
}

void UdpVSocket::Flush()
{
	SendBuffer();
}

LOG_CLASS_DEFINE(NETWORK, UdpVSocket);



DualMuxSocket::DualMuxSocket(const str& pName, const SocketAddress& pLocalAddress, bool pIsServer,
	unsigned pMaxPendingConnectionCount, unsigned pMaxConnectionCount):
	mTcpMuxSocket(new TcpMuxSocket(pName, pLocalAddress, pIsServer, pMaxConnectionCount)),
	mUdpMuxSocket(new UdpMuxSocket(pName, pLocalAddress, pMaxPendingConnectionCount, pMaxConnectionCount)),
	mConnectDualTimeout(DEFAULT_CONNECT_DUAL_TIMEOUT)
{
	mTcpMuxSocket->SetCloseCallback(this, &DualMuxSocket::OnCloseTcpSocket);
	log_atrace("DualMuxSocket()");
}

DualMuxSocket::~DualMuxSocket()
{
	log_atrace("~DualMuxSocket()");

	ScopeLock lLock(&mLock);
	while (!mIdSocketMap.empty())
	{
		CloseSocket(mIdSocketMap.begin()->second);
	}

	delete (mTcpMuxSocket);
	mTcpMuxSocket = 0;
	delete (mUdpMuxSocket);
	mUdpMuxSocket = 0;
}

bool DualMuxSocket::IsOpen() const
{
	ScopeLock lLock(&mLock);
	return (mTcpMuxSocket && mUdpMuxSocket && mTcpMuxSocket->IsOpen() && mUdpMuxSocket->IsOpen());
}

DualSocket* DualMuxSocket::Connect(const SocketAddress& pTargetAddress, double pTimeout)
{
	// Simulatanously connect TCP and UDP.
	ScopeLock lLock(&mLock);
	std::string lConnectionId = SystemManager::GetRandomId();
	Semaphore lConnectedSemaphore;
	Connector<TcpMuxSocket, TcpVSocket> lTcpConnector(_T("TCP connector"), mTcpMuxSocket, pTargetAddress, lConnectionId, pTimeout, lConnectedSemaphore);
	Connector<UdpMuxSocket, UdpVSocket> lUdpConnector(_T("UDP connector"), mUdpMuxSocket, pTargetAddress, lConnectionId, pTimeout, lConnectedSemaphore);
	if (lTcpConnector.Start() && lUdpConnector.Start())
	{
		// Wait for both connectors to finish.
		HiResTimer lTime;
		lConnectedSemaphore.Wait(pTimeout);
		lConnectedSemaphore.Wait(pTimeout - lTime.PopTimeDiff());
		lUdpConnector.Join();
		lTcpConnector.Join();	// Join TCP last, in hope that it'll have made it through.
	}
	DualSocket* lSocket = 0;
	if (lTcpConnector.mSocket && lUdpConnector.mSocket)
	{
		lSocket = new DualSocket(this, lConnectionId);
		AddSocket(lSocket, lTcpConnector.mSocket, lUdpConnector.mSocket);
	}
	else
	{
		if (lTcpConnector.mSocket)
		{
			mTcpMuxSocket->CloseSocket(lTcpConnector.mSocket, true);
		}
		if (lUdpConnector.mSocket)
		{
			mUdpMuxSocket->CloseSocket(lUdpConnector.mSocket);
		}
	}
	return (lSocket);
}

void DualMuxSocket::Close()
{
	ScopeLock lLock(&mLock);
	if (mTcpMuxSocket)
	{
		mTcpMuxSocket->Close();
	}
	if (mUdpMuxSocket)
	{
		mUdpMuxSocket->Close();
	}
}

DualSocket* DualMuxSocket::PollAccept()
{
	ScopeLock lLock(&mLock);

	// Slaughter old single-sided-connector (ONLY TCP or ONLY UDP).
	KillNonDualConnected();

	DualSocket* lSocket = 0;

	// Try to connect TCP to an existing DualSocket, or create a new one.
	TcpVSocket* lTcpSocket = mTcpMuxSocket->PollAccept();
	if (lTcpSocket)
	{
		IdSocketMap::iterator x = mIdSocketMap.find(lTcpSocket->GetConnectionId());
		if (x != mIdSocketMap.end())
		{
			// UDP is already connected.
			lSocket = x->second;
			AddSocket(lSocket, lTcpSocket, 0);
		}
		else
		{
			// TCP is first.
			lSocket = new DualSocket(this, lTcpSocket->GetConnectionId());
			AddSocket(lSocket, lTcpSocket, 0);
		}
	}

	if (!lTcpSocket)
	{
		// Try to connect UDP to an existing DualSocket, or create a new one.
		UdpVSocket* lUdpSocket = mUdpMuxSocket->PollAccept();
		if (lUdpSocket)
		{
			IdSocketMap::iterator x = mIdSocketMap.find(lUdpSocket->GetConnectionId());
			if (x != mIdSocketMap.end())
			{
				// TCP is already connected.
				lSocket = x->second;
				AddSocket(lSocket, 0, lUdpSocket);
			}
			else
			{
				// UDP is first.
				lSocket = new DualSocket(this, lUdpSocket->GetConnectionId());
				AddSocket(lSocket, 0, lUdpSocket);
			}
		}
	}

	// If both TCP and UDP are connected, we roll.
	if (lSocket && !lSocket->IsOpen())
	{
		lSocket = 0;
	}
	return (lSocket);
}

DualSocket* DualMuxSocket::PopReceiverSocket(bool pSafe)
{
	ScopeLock lLock(&mLock);

	DualSocket* lSocket = 0;

	if (pSafe)
	{
		TcpVSocket* lTcpSocket = mTcpMuxSocket->PopReceiverSocket();
		if (lTcpSocket)
		{
			lSocket = HashUtil::FindMapObject(mTcpSocketMap, lTcpSocket);
		}
	}
	else
	{
		UdpVSocket* lUdpSocket = mUdpMuxSocket->PopReceiverSocket();
		if (lUdpSocket)
		{
			lSocket = HashUtil::FindMapObject(mUdpSocketMap, lUdpSocket);
		}
	}

	// If we received something, even though the socket isn't fully opened we throw it away.
	if (lSocket && !lSocket->IsOpen())
	{
		CloseSocket(lSocket);
		lSocket = 0;
	}
	return (lSocket);
}

DualSocket* DualMuxSocket::PopSenderSocket()
{
	ScopeLock lLock(&mLock);

	DualSocket* lSocket = 0;

	//if (!lSocket)
	{
		TcpVSocket* lTcpSocket = mTcpMuxSocket->PopSenderSocket();
		if (lTcpSocket)
		{
			lSocket = HashUtil::FindMapObject(mTcpSocketMap, lTcpSocket);
		}
	}
	if (!lSocket)
	{
		UdpVSocket* lUdpSocket = mUdpMuxSocket->PopSenderSocket();
		if (lUdpSocket)
		{
			lSocket = HashUtil::FindMapObject(mUdpSocketMap, lUdpSocket);
		}
	}

	assert(!lSocket || lSocket->IsOpen());
	return (lSocket);
}

uint64 DualMuxSocket::GetSentByteCount(bool pSafe) const
{
	ScopeLock lLock(&mLock);

	uint64 lCount;
	if (pSafe)
	{
		lCount = mTcpMuxSocket->GetTotalSentByteCount();
	}
	else
	{
		lCount = mUdpMuxSocket->GetSentByteCount();
	}
	return (lCount);
}

uint64 DualMuxSocket::GetReceivedByteCount(bool pSafe) const
{
	ScopeLock lLock(&mLock);

	uint64 lCount;
	if (pSafe)
	{
		lCount = mTcpMuxSocket->GetTotalReceivedByteCount();
	}
	else
	{
		lCount = mUdpMuxSocket->GetReceivedByteCount();
	}
	return (lCount);
}

SocketAddress DualMuxSocket::GetLocalAddress() const
{
	if (mTcpMuxSocket)
	{
		// RAII.
		return (mTcpMuxSocket->GetLocalAddress());
	}
	if (mUdpMuxSocket)
	{
		// RAII.
		return (mUdpMuxSocket->GetLocalAddress());
	}
	return (SocketAddress());
}

unsigned DualMuxSocket::GetConnectionCount() const
{
	ScopeLock lLock(&mLock);
	return ((unsigned)mIdSocketMap.size());
}

void DualMuxSocket::SetSafeConnectIdTimeout(double pTimeout)
{
	mTcpMuxSocket->SetConnectIdTimeout(pTimeout);
}

void DualMuxSocket::SetConnectDualTimeout(double pTimeout)
{
	mConnectDualTimeout = pTimeout;
}

void DualMuxSocket::SetDatagramReceiver(DatagramReceiver* pReceiver)
{
	mTcpMuxSocket->SetDatagramReceiver(pReceiver);
}

void DualMuxSocket::CloseSocket(DualSocket* pSocket)
{
	ScopeLock lLock(&mLock);
	if (HashUtil::FindMapObject(mIdSocketMap, pSocket->GetConnectionId()) == pSocket)
	{
		if (pSocket->GetTcpSocket())
		{
			mTcpSocketMap.erase(pSocket->GetTcpSocket());
			mTcpMuxSocket->CloseSocket(pSocket->GetTcpSocket(), false);
		}

		if (pSocket->GetUdpSocket())
		{
			mUdpSocketMap.erase(pSocket->GetUdpSocket());
			mUdpMuxSocket->CloseSocket(pSocket->GetUdpSocket());
		}

		mIdSocketMap.erase(pSocket->GetConnectionId());
		mPendingDualConnectMap.Remove(pSocket);
		delete (pSocket);
	}
}

void DualMuxSocket::DispatchCloseSocket(DualSocket* pSocket)
{
	if (!mCloseDispatcher.empty())
	{
		mCloseDispatcher(pSocket);
	}
	CloseSocket(pSocket);
}

void DualMuxSocket::AddUdpReceiverSocket(UdpVSocket* pUdpSocket)
{
	ScopeLock lLock(&mLock);
	assert(HashUtil::FindMapObject(mUdpSocketMap, pUdpSocket));
	mUdpMuxSocket->AddReceiver(pUdpSocket);
}

void DualMuxSocket::AddSocket(DualSocket* pSocket, TcpVSocket* pTcpSocket, UdpVSocket* pUdpSocket)
{
	if (pTcpSocket && pUdpSocket)
	{
		assert(!HashUtil::FindMapObject(mIdSocketMap, pSocket->GetConnectionId()));
	}
	if (pTcpSocket)
	{
		assert(!HashUtil::FindMapObject(mTcpSocketMap, pTcpSocket));
		pSocket->SetSocket(pTcpSocket);
		mTcpSocketMap.insert(std::pair<TcpVSocket*, DualSocket*>(pTcpSocket, pSocket));
	}
	if (pUdpSocket)
	{
		assert(!HashUtil::FindMapObject(mUdpSocketMap, pUdpSocket));
		pSocket->SetSocket(pUdpSocket);
		mUdpSocketMap.insert(std::pair<UdpVSocket*, DualSocket*>(pUdpSocket, pSocket));
	}
	if (!HashUtil::FindMapObject(mIdSocketMap, pSocket->GetConnectionId()))
	{
		log_trace(_T("Adding DualSocket with ID ")+
			astrutil::ToCurrentCode(astrutil::ReplaceCtrlChars(pSocket->GetConnectionId(), '.'))+
			(pSocket->GetTcpSocket()?_T(" TCP set,"):_T(" no TCP,"))+str()+
			(pSocket->GetUdpSocket()?_T(" UDP set."):_T(" no UDP.")));
		mIdSocketMap.insert(std::pair<std::string, DualSocket*>(pSocket->GetConnectionId(), pSocket));
	}
	else
	{
		log_debug(_T("Appending info to DualSocket with ID ")+
			astrutil::ToCurrentCode(astrutil::ReplaceCtrlChars(pSocket->GetConnectionId(), '.')) +
			(pSocket->GetTcpSocket()?_T(" TCP set,"):_T(" no TCP,"))+str()+
			(pSocket->GetUdpSocket()?_T(" UDP set."):_T(" no UDP.")));
	}
	if (!pSocket->GetTcpSocket() || !pSocket->GetUdpSocket())
	{
		log_atrace("Adding a not-yet-fully-connected DualSocket to 'pending dual' list.");
		mPendingDualConnectMap.PushBack(pSocket, Timer());
	}
	else
	{
		log_atrace("Dropping a fully connected DualSocket from 'pending dual' list.");
		mPendingDualConnectMap.Remove(pSocket);
	}
}

void DualMuxSocket::KillNonDualConnected()
{
	for (SocketTimeMap::Iterator x = mPendingDualConnectMap.First(); x != mPendingDualConnectMap.End();)
	{
		Timer& lTime = x.GetObject();
		lTime.UpdateTimer();
		if (lTime.GetTimeDiffF() >= mConnectDualTimeout)
		{
			log_adebug("Connected socket dual-timed out => dropped.");
			DualSocket* lSocket = x.GetKey();
			++x;	// Must be increased before we close the socket (implicitly removes it from the table).
			CloseSocket(lSocket);
		}
		else
		{
			++x;
		}
	}
}

void DualMuxSocket::OnCloseTcpSocket(TcpVSocket* pTcpSocket)
{
	DualSocket* lSocket;
	{
		ScopeLock lLock(&mLock);
		lSocket = HashUtil::FindMapObject(mTcpSocketMap, pTcpSocket);
	}
	if (lSocket)
	{
		DispatchCloseSocket(lSocket);
	}
}

const double DualMuxSocket::DEFAULT_CONNECT_DUAL_TIMEOUT = 5.0;

LOG_CLASS_DEFINE(NETWORK, DualMuxSocket);



DualSocket::DualSocket(DualMuxSocket* pMuxSocket, const std::string& pConnectionId):
	ConnectionWithId(),
	mMuxSocket(pMuxSocket),
	mTcpSocket(0),
	mUdpSocket(0)
{
	log_atrace("DualSocket()");
	SetConnectionId(pConnectionId);
}

bool DualSocket::SetSocket(TcpVSocket* pSocket)
{
	bool lOk = (!mTcpSocket);
	assert(lOk);
	if (lOk)
	{
		mTcpSocket = pSocket;
	}
	return (lOk);
}

bool DualSocket::SetSocket(UdpVSocket* pSocket)
{
	bool lOk = (!mUdpSocket);
	assert(lOk);
	if (lOk)
	{
		mUdpSocket = pSocket;
	}
	return (lOk);
}

bool DualSocket::IsOpen() const
{
	return (mTcpSocket && mUdpSocket && mTcpSocket->IsOpen() && mMuxSocket->IsOpen());
}

void DualSocket::ClearAll()
{
	mTcpSocket = 0;
	mUdpSocket = 0;
	ClearConnectionId();
}

SocketAddress DualSocket::GetLocalAddress() const
{
	if (mMuxSocket)
	{
		// RAII.
		return (mMuxSocket->GetLocalAddress());
	}
	return (SocketAddress());
}

SocketAddress DualSocket::GetTargetAddress() const
{
	if (mTcpSocket)
	{
		// RAII.
		return (mTcpSocket->GetTargetAddress());
	}
	if (mUdpSocket)
	{
		// RAII.
		mUdpSocket->GetTargetAddress();
	}
	return (SocketAddress());
}

void DualSocket::ClearOutputData()
{
	if (mTcpSocket)
	{
		mTcpSocket->ClearOutputData();
	}
	if (mUdpSocket)
	{
		mUdpSocket->ClearOutputData();
	}
}

Datagram& DualSocket::GetSendBuffer(bool pSafe) const
{
	if (pSafe)
	{
		return (mTcpSocket->GetSendBuffer());
	}
	else
	{
		return (mUdpSocket->GetSendBuffer());
	}
}

IOError DualSocket::AppendSendBuffer(bool pSafe, const void* pData, int pLength)
{
	IOError lSendResult = IO_OK;
	if (pSafe)
	{
		lSendResult = mTcpSocket->AppendSendBuffer(pData, (int)pLength);
	}
	else
	{
		lSendResult = mUdpSocket->AppendSendBuffer(pData, pLength);
	}
	return (lSendResult);
}

int DualSocket::SendBuffer()
{
	int lTcpSentCount = 0;
	if (mTcpSocket && !mTcpSocket->GetInSenderList())	// If it is in the sender list, it will be served later.
	{
		lTcpSentCount = mTcpSocket->SendBuffer();
		log_volatile(mLog.Tracef(_T("TCP send returned %i."), lTcpSentCount));
	}
	int lUdpSentCount = 0;
	if (mUdpSocket && !mUdpSocket->GetInSenderList())	// If it is in the sender list, it will be served later.
	{
		lUdpSentCount = mUdpSocket->SendBuffer();
		log_volatile(mLog.Tracef(_T("UDP send returned %i."), lUdpSentCount));
	}
	int lSentCount = -1;
	if (lTcpSentCount >= 0 && lUdpSentCount >= 0)
	{
		lSentCount = lTcpSentCount+lUdpSentCount;
	}
	return (lSentCount);
}

bool DualSocket::HasSendData() const
{
	return (mTcpSocket->HasSendData() || mUdpSocket->HasSendData());
}

int DualSocket::Receive(bool pSafe, void* pData, int pLength)
{
	int lReceiveCount;
	if (pSafe)
	{
		lReceiveCount = mTcpSocket->Receive(pData, pLength, true);
		//log_volatile(mLog.Tracef(_T("TCP recv returned %i."), lReceiveCount));
	}
	else
	{
		lReceiveCount = mUdpSocket->Receive(pData, pLength);
		//log_volatile(mLog.Tracef(_T("UDP recv returned %i."), lReceiveCount));
	}
	return (lReceiveCount);
}

void DualSocket::TryAddReceiverSocket()
{
	// TODO: add TCP receiver socket...
	if (mUdpSocket->NeedInputPeek())
	{
		mMuxSocket->AddUdpReceiverSocket(mUdpSocket);
	}
}

DualSocket::~DualSocket()
{
	log_atrace("~DualSocket()");
	ClearAll();
	mMuxSocket = 0;
}

TcpVSocket* DualSocket::GetTcpSocket() const
{
	return (mTcpSocket);
}

UdpVSocket* DualSocket::GetUdpSocket() const
{
	return (mUdpSocket);
}

LOG_CLASS_DEFINE(NETWORK, DualSocket);



}
