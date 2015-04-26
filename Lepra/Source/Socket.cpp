
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/Socket.h"
#include "../Include/FdSet.h"
#include "../Include/SocketAddressGetter.h"
#include "../Include/HashUtil.h"
#include "../Include/HiResTimer.h"
#include "../Include/ResourceTracker.h"
#include "../Include/SystemManager.h"



namespace Lepra
{



s_socket SocketBase::InitSocket(s_socket pSocket, int pSize, bool pReuse)
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
	if (pReuse)
	{
#ifndef LEPRA_WINDOWS
		int lFlag = 1;
		::setsockopt(pSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&lFlag, sizeof(lFlag));
#endif // !Windows
	}
	return (pSocket);
}

s_socket SocketBase::CreateTcpSocket()
{
	LEPRA_ACQUIRE_RESOURCE(Socket);
	s_socket s = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	return (InitSocket(s, 32*1024, true));
}

s_socket SocketBase::CreateUdpSocket()
{
	LEPRA_ACQUIRE_RESOURCE(Socket);
	s_socket s = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	return (InitSocket(s, 8*1024, false));
}

void SocketBase::CloseSysSocket(s_socket pSocket)
{
#ifdef LEPRA_WINDOWS
	::closesocket(pSocket);
#else // !LEPRA_WINDOWS
	::close(pSocket);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
	LEPRA_RELEASE_RESOURCE(Socket);
}



SocketBase::SocketBase(s_socket pSocket):
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

s_socket SocketBase::GetSysSocket() const
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

int SocketBase::ClearErrors() const
{
	int lError = 0;
	int lSize = sizeof(lError);
	::getsockopt(mSocket, SOL_SOCKET, SO_ERROR, (char*)&lError, (socklen_t*)&lSize);
	return lError;
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

	if (mSendBuffer.mDataSize + pLength < Datagram::BUFFER_SIZE)
	{
		if (!mInSendBuffer)
		{
			SetInSenderList(true);
			deb_assert(!mMuxIo->IsSender(this));
			mMuxIo->AddSender(this);
		}
		else
		{
			deb_assert(mMuxIo->IsSender(this));
		}
		::memcpy(&mSendBuffer.mDataBuffer[mSendBuffer.mDataSize], pData, pLength);
		mSendBuffer.mDataSize += (int)pLength;
	}
	else
	{
		// It's all or nothing. If half a network packet arrives, we're fucked.
		SendBuffer();
		if (pLength <= Datagram::BUFFER_SIZE)
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



TcpListenerSocket::TcpListenerSocket(const SocketAddress& pLocalAddress, bool pIsServer):
	mConnectionCount(0),
	mLocalAddress(pLocalAddress),
	mReceiver(0)
{
	log_atrace("TcpListenerSocket()");

	// Initialize the socket.
	mSocket = CreateTcpSocket();

	if (mSocket != INVALID_SOCKET && pIsServer)
	{
		// Init socket address and bind it to the socket.
		if (::bind(mSocket, SocketAddressGetter::GetRaw(mLocalAddress), sizeof(RawSocketAddress)) == 0)
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
			FdSet lAcceptSet;
			LEPRA_FD_ZERO(&lAcceptSet);
#pragma warning(push)
#pragma warning(disable: 4127)	// MSVC warning: conditional expression is constant.
			LEPRA_FD_SET((sys_socket)mSocket, &lAcceptSet);
#pragma warning(pop)
			timeval lTime;
			lTime.tv_sec = 1;
			lTime.tv_usec = 0;
			lAcceptCount = ::select((int)mSocket+1, LEPRA_FDS(&lAcceptSet), NULL, NULL, &lTime);
		}
		if (lAcceptCount >= 1)
		{
			SocketAddress lSockAddress;
			socklen_t lSize = (socklen_t)sizeof(RawSocketAddress);
			s_socket lSocket = ::accept(mSocket, SocketAddressGetter::GetRaw(lSockAddress), &lSize);
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

TcpSocket* TcpListenerSocket::CreateSocket(s_socket pSocket, const SocketAddress& pTargetAddress,
	TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver)
{
	return (new TcpSocket(pSocket, pTargetAddress, pServerSocket, pReceiver));
}

void TcpListenerSocket::DecNumConnections()
{
	BusLock::Add(&mConnectionCount, -1);
}

loginstance(NETWORK, TcpListenerSocket);



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
		if (::bind(mSocket, SocketAddressGetter::GetRaw(pLocalAddress), sizeof(RawSocketAddress)) != 0)
		{
			mLog.Warningf(_T("TCP socket binding failed! Error=%i"), SOCKET_LAST_ERROR());
			Close();
		}
	}
}

TcpSocket::TcpSocket(s_socket pSocket, const SocketAddress& pTargetAddress,
	TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver):
	SocketBase(pSocket),
	mReceiver(pReceiver),
	mUnreceivedByteCount(0),
	mTargetAddress(pTargetAddress),
	mServerSocket(pServerSocket)
{
	log_atrace("TcpSocket()");
	InitSocket(mSocket, 32*1024, false);
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
	bool lOk = (::connect(mSocket, SocketAddressGetter::GetRaw(mTargetAddress), sizeof(RawSocketAddress)) != SOCKET_ERROR);
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
		mServerSocket = 0;
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
			log_volatile(str lData = strutil::DumpData((uint8*)pData, std::min((int)pSize, 50)));
			log_volatile(str lString = strutil::Encode(astrutil::ReplaceCtrlChars((const char*)pData, '.')));
			log_volatile(lString.resize(15));
			log_volatile(mLog.Tracef(_T("TCP -> %u bytes (%s -> %s): %s %s."), pSize,
				lLocalAddress.c_str(), mTargetAddress.GetAsString().c_str(), lData.c_str(), lString.c_str()));

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
			log_volatile(str lData = strutil::DumpData((uint8*)pData, std::min(lSize, 50)));
			log_volatile(str lString = strutil::Encode(astrutil::ReplaceCtrlChars((const char*)pData, '.')));
			log_volatile(lString.resize(15));
			log_volatile(mLog.Tracef(_T("TCP <- %u bytes (%s <- %s): %s %s."), lSize,
				lLocalAddress.c_str(), mTargetAddress.GetAsString().c_str(), lData.c_str(), lString.c_str()));

			mReceivedByteCount += lSize;
		}
	}
	return (lSize);
}

int TcpSocket::Receive(void* pData, int pMaxSize, double pTimeout)
{
	if (pTimeout < 0)
	{
		pTimeout = 0;
	}
	FdSet lAcceptSet;
	LEPRA_FD_ZERO(&lAcceptSet);
#pragma warning(push)
#pragma warning(disable: 4127)	// MSVC warning: conditional expression is constant.
	LEPRA_FD_SET((sys_socket)mSocket, &lAcceptSet);
#pragma warning(pop)
	timeval lTime;
	lTime.tv_sec = (long)pTimeout;
	lTime.tv_usec = (long)((pTimeout-lTime.tv_sec) * 1000000);
	int lReadCount = ::select((int)mSocket+1, LEPRA_FDS(&lAcceptSet), NULL, NULL, &lTime);
	if (lReadCount == 1)
	{
		return Receive(pData, pMaxSize);
	}
	return 0;
}

bool TcpSocket::Unreceive(void* pData, int pByteCount)
{
	deb_assert(mUnreceivedByteCount == 0);
	deb_assert(pByteCount <= (int)sizeof(mUnreceivedArray));
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

loginstance(NETWORK, TcpSocket);



UdpSocket::UdpSocket(const SocketAddress& pLocalAddress, bool pIsServer):
	mLocalAddress(pLocalAddress)
{
	// Initialize UDP socket.
	mSocket = CreateUdpSocket();

	if (mSocket != INVALID_SOCKET && pIsServer)
	{
		if (::bind(mSocket, SocketAddressGetter::GetRaw(mLocalAddress), sizeof(RawSocketAddress)) != 0)
		{
			mLog.Warningf(_T("Failed to bind UDP socket to %s: %i."), pLocalAddress.GetAsString().c_str(), SOCKET_LAST_ERROR());
			Close();
			mLocalAddress.Set(IPAddress(), 0);
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
		lSentByteCount = ::sendto(mSocket, (const char*)pData, pSize, 0, SocketAddressGetter::GetRaw(pTargetAddress), sizeof(RawSocketAddress));
		if (lSentByteCount != (int)pSize)
		{
			CloseKeepHandle();
		}
		else
		{
			log_volatile(str lData = strutil::DumpData((uint8*)pData, std::min(pSize, (unsigned)50)));
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
		socklen_t lAddrSize = (socklen_t)sizeof(RawSocketAddress);
		lSize = ::recvfrom(mSocket, (char*)pData, pMaxSize, 0, SocketAddressGetter::GetRaw(pSourceAddress), &lAddrSize);
		if (lSize <= 0)
		{
			// Disconnected.
			lSize = -1;
		}
		else
		{
			log_volatile(str lData = strutil::DumpData((uint8*)pData, std::min(lSize, 50)));
			log_volatile(mLog.Tracef(_T("UDP <- %u bytes (%s <- %s): %s."), lSize,
				mLocalAddress.GetAsString().c_str(), pSourceAddress.GetAsString().c_str(),
				lData.c_str()));
			mReceivedByteCount += lSize;
		}
	}
	return (lSize);
}

int UdpSocket::ReceiveFrom(uint8* pData, unsigned pMaxSize, SocketAddress& pSourceAddress, double pTimeout)
{
	if (pTimeout < 0)
	{
		pTimeout = 0;
	}
	FdSet lReadSet;
	LEPRA_FD_ZERO(&lReadSet);
#pragma warning(push)
#pragma warning(disable: 4127)	// MSVC warning: conditional expression is constant.
	LEPRA_FD_SET((sys_socket)mSocket, &lReadSet);
#pragma warning(pop)
	timeval lTime;
	lTime.tv_sec = (long)pTimeout;
	lTime.tv_usec = (long)((pTimeout-lTime.tv_sec) * 1000000);
	int lReadCount = ::select((int)mSocket+1, LEPRA_FDS(&lReadSet), NULL, NULL, &lTime);
	if (lReadCount == 1)
	{
		return ReceiveFrom(pData, pMaxSize, pSourceAddress);
	}
	return 0;
}

loginstance(NETWORK, UdpSocket);



UdpMuxSocket::UdpMuxSocket(const str& pName, const SocketAddress& pLocalAddress, bool pIsServer,
	unsigned pMaxPendingConnectionCount, unsigned pMaxConnectionCount):
	MuxIo(pMaxPendingConnectionCount, pMaxConnectionCount),
	Thread(astrutil::Encode(pName+_T("UdpMuxRecv ")+pLocalAddress.GetAsString())),
	UdpSocket(pLocalAddress, pIsServer)
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

	const SocketAddress& lAddress = GetLocalAddress();
	if (lAddress.GetPort())
	{
		s_socket lKiller = CreateUdpSocket();
		const int lReleaseByteCount = 8;
		::sendto(lKiller, "Release!", lReleaseByteCount, 0, SocketAddressGetter::GetRaw(lAddress), sizeof(RawSocketAddress));
		//Thread::Sleep(0.01);
		//CloseSysSocket(lKiller);
	}

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
				deb_assert(false);
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
				log_adebug("Remote end seems dead. Firewall?");
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
	log_volatile(mLog.Debugf(_T("Dropping UDP MUX socket %s."), pSocket->GetTargetAddress().GetAsString().c_str()));

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

UdpVSocket* UdpMuxSocket::GetVSocket(const SocketAddress& pTargetAddress)
{
	ScopeLock lLock(&mIoLock);
	return mSocketTable.FindObject(pTargetAddress);
}

bool UdpMuxSocket::SendOpenFirewallData(const SocketAddress& pTargetAddress)
{
	return SendTo(mOpenFirewallString, sizeof(mOpenFirewallString), pTargetAddress) == sizeof(mOpenFirewallString);
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
					::memcmp(lBuffer->mDataBuffer, mConnectionString, sizeof(mConnectionString)-1) == 0)
				{
					std::string lConnectionId((const char*)&lBuffer->mDataBuffer[sizeof(mConnectionString)-1], lBuffer->mDataSize+1-sizeof(mConnectionString));
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
				else if (lBuffer->mDataSize == sizeof(mOpenFirewallString) &&
					::memcmp(lBuffer->mDataBuffer, mOpenFirewallString, sizeof(mOpenFirewallString)) == 0)
				{
					log_atrace("Received an \"open firewall\" datagram.");
				}
				else
				{
					mLog.AWarning("Non-connected socket sent us junk.");
					log_volatile(const str lData = strutil::DumpData(lBuffer->mDataBuffer, std::min(lBuffer->mDataSize, 50)));
					log_volatile(mLog.Debugf(_T("UDP <- %i bytes (%s): %s."), lBuffer->mDataSize,
						lSourceAddress.GetAsString().c_str(), lData.c_str()));
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

const uint8 UdpMuxSocket::mOpenFirewallString[27] = "Aaaarglebargle glop-glyph!";

loginstance(NETWORK, UdpMuxSocket);



UdpVSocket::UdpVSocket()
{
	//log_atrace("UdpVSocket()");
	ClearAll();
}

UdpVSocket::~UdpVSocket()
{
	//log_atrace("~UdpVSocket()");
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

int UdpVSocket::Receive(bool, void* pData, int pLength)
{
	return Receive(pData, pLength);
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
			mLog.AError("Throwing away network data, since receive buffer is full!");
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
	const Thread* lCurrentThread = Thread::GetCurrentThread();
	HiResTimer lTimer(false);
	while (GetAvailable() == 0 && lTimer.QueryTimeDiff() < pTimeout &&
		!lCurrentThread->GetStopRequest() && !SystemManager::GetQuitRequest())
	{
		Thread::Sleep(Math::Clamp(pTimeout*0.1, 0.001, 60*60.0));
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

void UdpVSocket::SetSafeSend(bool)
{
}

loginstance(NETWORK, UdpVSocket);



}
