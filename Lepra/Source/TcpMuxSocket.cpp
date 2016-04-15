
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/TcpMuxSocket.h"
#include "../Include/HashUtil.h"
#include "../Include/SystemManager.h"



namespace Lepra
{



TcpMuxSocket::TcpMuxSocket(const str& pName, const SocketAddress& pLocalAddress, bool pIsServer,
	unsigned pMaxPendingConnectionCount, unsigned pMaxConnectionCount):
	MuxIo(pMaxPendingConnectionCount, pMaxConnectionCount),
	TcpListenerSocket(pLocalAddress, pIsServer),
	mAcceptThread(pName+"TcpMuxAccept "+pLocalAddress.GetAsString()),
	mSelectThread(pName+"TcpMuxSelect "+pLocalAddress.GetAsString()),
	mConnectIdTimeout(DEFAULT_CONNECT_ID_TIMEOUT),
	mActiveReceiverMapChanged(false),
	mConnectedSocketSemaphore(100),
	mVSentByteCount(0),
	mVReceivedByteCount(0)
{
	log_trace("TcpMuxSocket()");
	if (pIsServer)
	{
		mAcceptThread.Start(this, &TcpMuxSocket::AcceptThreadEntry);
	}
	mSelectThread.Start(this, &TcpMuxSocket::SelectThreadEntry);
}

TcpMuxSocket::~TcpMuxSocket()
{
	log_trace("~TcpMuxSocket()");
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
			mLog.Error("Could not send connect data to server!");
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
			log_trace("Connect went through!");
		}
		else
		{
			if (lBuffer.mDataSize == 0)
			{
				log_debug("Remote end seems dead. Firewall?");
			}
			else if (lBuffer.mDataSize < 0)
			{
				mLog.Error("Connect was refused. Firewall?");
			}
			else if (lBuffer.mDataSize > 0)
			{
				mLog.Error("Connect was replied to with jibberish. Wassup?");
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
	HiResTimer lTime(false);
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
			log_trace("Popped a connected socket.");
			AcceptStatus lAcceptStatus = QueryReceiveConnectString(lTcpSocket);
			if (lAcceptStatus == ACCEPT_CLOSE)
			{
				CloseSocket(lTcpSocket, true);
				lTcpSocket = 0;
			}
			else if (lAcceptStatus == ACCEPT_QUERY_WAIT)
			{
				if (lTime.QueryTimeDiff() >= mConnectIdTimeout)
				{
					log_debug("Connected socket ID-timed out => dropped.");
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
		log_trace("Closing TcpVSocket.");
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
		log_trace("Popped TCP sender socket.");
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

TcpSocket* TcpMuxSocket::CreateSocket(s_socket pSocket, const SocketAddress& pTargetAddress,
	TcpListenerSocket* pServerSocket, DatagramReceiver* pReceiver)
{
	return (new TcpVSocket(pSocket, pTargetAddress, *(TcpMuxSocket*)pServerSocket, pReceiver));
}

void TcpMuxSocket::AddConnectedSocket(TcpVSocket* pSocket)
{
	ScopeLock lLock(&mIoLock);
	log_volatile(mLog.Tracef("AddConnectedSocket(%i)", pSocket->GetSysSocket()));
	mConnectedSocketMap.insert(SocketVMap::value_type(pSocket->GetSysSocket(), pSocket));
	mActiveReceiverMapChanged = true;
	mConnectedSocketSemaphore.Signal();
}

bool TcpMuxSocket::RemoveConnectedSocketNoLock(TcpVSocket* pSocket)
{
	log_volatile(mLog.Tracef("RemoveConnectedSocketNoLock(%i)", pSocket->GetSysSocket()));
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
		//log_volatile(mLog.Tracef("Rebuilding active receiver map with %i possible sockets.", lSocketCount));
		LEPRA_FD_ZERO(&pSocketSet);
		for (SocketVMap::iterator x = mConnectedSocketMap.begin(); x != mConnectedSocketMap.end(); ++x)
		{
			TcpVSocket* lSocket = x->second;
			if (!IsReceiverNoLock(lSocket))
			{
				s_socket lSysSocket = x->first;
#pragma warning(push)
#pragma warning(disable: 4127)	// MSVC warning: conditional expression is constant.
				LEPRA_FD_SET((sys_socket)lSysSocket, &pSocketSet);
#pragma warning(pop)
			}
		}
		lSocketCount = FdSetHelper::Copy(mBackupFdSet, pSocketSet);
	}
	else
	{
		lSocketCount = FdSetHelper::Copy(pSocketSet, mBackupFdSet);
	}
	deb_assert(lSocketCount == (int)LEPRA_FD_GET_COUNT(&pSocketSet));
	return (lSocketCount);
}

void TcpMuxSocket::PushReceiverSockets(const FdSet& pSocketSet)
{
	ScopeLock lLock(&mIoLock);
	// TODO: optimize (using platform specifics?).
	bool lAdded = false;
	for (SocketVMap::iterator y = mConnectedSocketMap.begin(); y != mConnectedSocketMap.end(); ++y)
	{
		s_socket lSysSocket = y->first;
		if (FD_ISSET(lSysSocket, (fd_set*)LEPRA_FDS(&pSocketSet)))
		{
			TcpVSocket* lSocket = y->second;
			log_debug("Adding receiver socket.");
			AddReceiverNoLock(lSocket);
			lAdded = true;
		}
		else
		{
			log_debug("Didn't add receiver socket.");
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
		mLog.Warning("Received a connect from a banned client.");
		return (ACCEPT_CLOSE);	// RAII is great here.
	}

	// Check if we get the connection string (containing an ID that connects the TCP and UDP sockets).
	AcceptStatus lAcceptStatus = ACCEPT_QUERY_WAIT;
	Datagram lBuffer;
	lBuffer.mDataSize = pSocket->Receive(lBuffer.mDataBuffer, sizeof(lBuffer.mDataBuffer), false);
	if (lBuffer.mDataSize > 0)
	{
		log_trace("Received a connect string.");

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
				mLog.Warning("Too many sockets!");
				lAcceptStatus = ACCEPT_CLOSE;
			}
		}
		else
		{
			mLog.Warning("Invalid connection string.");
			lAcceptStatus = ACCEPT_CLOSE;
		}
	}
	else
	{
		log_trace("Waited for connect magic, but none came this loop.");
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
	log_trace("Accept thread running");

	while (IsOpen() && !mAcceptThread.GetStopRequest())
	{
		TcpVSocket* lSocket = (TcpVSocket*)TcpListenerSocket::Accept(&TcpMuxSocket::CreateSocket);
		if (lSocket)
		{
			log_trace("Received a connect.");
			HiResTimer lTime(false);
			ScopeLock lLock(&mIoLock);
			mPendingConnectIdMap.PushBack(lSocket, lTime);
			mAcceptSemaphore.Signal();
		}
	}
}

void TcpMuxSocket::SelectThreadEntry()
{
	log_trace("Select thread running");

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
			deb_assert(lSocketCount == (int)LEPRA_FD_GET_COUNT(&lReadSet));
			FdSet lExceptionSet;
			FdSetHelper::Copy(lExceptionSet, lReadSet);
			int lSelectCount = ::select(LEPRA_FD_GET_MAX_HANDLE(&lReadSet)+1, LEPRA_FDS(&lReadSet), 0, LEPRA_FDS(&lExceptionSet), &lTimeout);
			if (lSelectCount > 0)
			{
				log_trace("Picked up a receive socket.");
				PushReceiverSockets(lReadSet);
			}
			else if (lSelectCount < 0)
			{
				int e = SOCKET_LAST_ERROR();
				mLog.Warningf("Could not ::select() properly. Error=%i, sockets=%u, exception set=%u.", e, lSocketCount, LEPRA_FD_GET_COUNT(&lExceptionSet));

				for (unsigned x = 0; x < LEPRA_FD_GET_COUNT(&lExceptionSet); ++x)
				{
					const s_socket lSysSocket = LEPRA_FD_GET(&lExceptionSet, x);
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
						mLog.Warning("Kicking socket due to ::select() failure.");
						DispatchCloseSocket(lSocket);
					}
				}
			}
		}
		else
		{
			log_debug("Going into \"wait for socket connect\" state.");
			mConnectedSocketSemaphore.Wait(10.0f);
			log_debug("Leaving \"wait for socket connect\" state.");
		}
	}
}



TcpVSocket::TcpVSocket(TcpMuxSocket& pMuxSocket, DatagramReceiver* pReceiver):
	TcpSocket(pReceiver)
{
	log_trace("TcpVSocket()");
	mMuxIo = &pMuxSocket;
}

TcpVSocket::TcpVSocket(s_socket pSocket, const SocketAddress& pTargetAddress, TcpMuxSocket& pMuxSocket,
	DatagramReceiver* pReceiver):
	TcpSocket(pSocket, pTargetAddress, &pMuxSocket, pReceiver)
{
	log_trace("TcpVSocket()");
	mMuxIo = &pMuxSocket;
	MakeNonBlocking();
}

TcpVSocket::~TcpVSocket()
{
	log_trace("~TcpVSocket()");
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
	HiResTimer lTimer(false);
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



DualMuxSocket::DualMuxSocket(const str& pName, const SocketAddress& pLocalAddress, bool pIsServer,
	unsigned pMaxPendingConnectionCount, unsigned pMaxConnectionCount):
	mTcpMuxSocket(new TcpMuxSocket(pName, pLocalAddress, pIsServer, pMaxPendingConnectionCount, pMaxConnectionCount)),
	mUdpMuxSocket(new UdpMuxSocket(pName, pLocalAddress, pIsServer, pMaxPendingConnectionCount, pMaxConnectionCount)),
	mConnectDualTimeout(DEFAULT_CONNECT_DUAL_TIMEOUT),
	mPopSafeToggle(false)
{
	mTcpMuxSocket->SetCloseCallback(this, &DualMuxSocket::OnCloseTcpSocket);
	log_trace("DualMuxSocket()");
}

DualMuxSocket::~DualMuxSocket()
{
	log_trace("~DualMuxSocket()");

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

bool DualMuxSocket::IsOpen(DualSocket* pSocket) const
{
	ScopeLock lLock(&mLock);
	return (HashUtil::FindMapObject(mIdSocketMap, pSocket->GetConnectionId()) == pSocket);
}

DualSocket* DualMuxSocket::Connect(const SocketAddress& pTargetAddress, const std::string& pConnectionId, double pTimeout)
{
	// Simulatanously connect TCP and UDP.
	ScopeLock lLock(&mLock);
	Semaphore lConnectedSemaphore;
	Connector<TcpMuxSocket, TcpVSocket> lTcpConnector("TCP connector", mTcpMuxSocket, pTargetAddress, pConnectionId, pTimeout, lConnectedSemaphore);
	Connector<UdpMuxSocket, UdpVSocket> lUdpConnector("UDP connector", mUdpMuxSocket, pTargetAddress, pConnectionId, pTimeout, lConnectedSemaphore);
	if (lTcpConnector.Start() && lUdpConnector.Start())
	{
		// Wait for both connectors to finish.
		HiResTimer lTime(false);
		lConnectedSemaphore.Wait(pTimeout);
		lConnectedSemaphore.Wait(pTimeout - lTime.PopTimeDiff());
		lUdpConnector.Join();
		lTcpConnector.Join();	// Join TCP last, in hope that it'll have made it through.
	}
	DualSocket* lSocket = 0;
	if (lTcpConnector.mSocket && lUdpConnector.mSocket)
	{
		lSocket = new DualSocket(this, pConnectionId);
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

DualSocket* DualMuxSocket::PopReceiverSocket()
{
	mPopSafeToggle = !mPopSafeToggle;
	return PopReceiverSocket(mPopSafeToggle);
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
	if (lSocket)
	{
		if (!lSocket->IsOpen())
		{
			CloseSocket(lSocket);
			lSocket = 0;
		}
		else
		{
			lSocket->SetSafeReceive(pSafe);
		}
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

	deb_assert(!lSocket || lSocket->IsOpen());
	return (lSocket);
}

uint64 DualMuxSocket::GetSentByteCount() const
{
	return GetSentByteCount(false) + GetSentByteCount(true);
}

uint64 DualMuxSocket::GetSentByteCount(bool pSafe) const
{
	ScopeLock lLock(&mLock);
	return pSafe? mTcpMuxSocket->GetTotalSentByteCount() : mUdpMuxSocket->GetSentByteCount();
}

uint64 DualMuxSocket::GetReceivedByteCount() const
{
	return GetReceivedByteCount(false) + GetReceivedByteCount(true);
}

uint64 DualMuxSocket::GetReceivedByteCount(bool pSafe) const
{
	ScopeLock lLock(&mLock);
	return pSafe? mTcpMuxSocket->GetTotalReceivedByteCount() : mUdpMuxSocket->GetReceivedByteCount();
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
	deb_assert(HashUtil::FindMapObject(mUdpSocketMap, pUdpSocket));
	mUdpMuxSocket->AddReceiver(pUdpSocket);
}

void DualMuxSocket::AddSocket(DualSocket* pSocket, TcpVSocket* pTcpSocket, UdpVSocket* pUdpSocket)
{
	if (pTcpSocket && pUdpSocket)
	{
		deb_assert(!HashUtil::FindMapObject(mIdSocketMap, pSocket->GetConnectionId()));
	}
	if (pTcpSocket)
	{
		deb_assert(!HashUtil::FindMapObject(mTcpSocketMap, pTcpSocket));
		pSocket->SetSocket(pTcpSocket);
		mTcpSocketMap.insert(TcpSocketMap::value_type(pTcpSocket, pSocket));
	}
	if (pUdpSocket)
	{
		deb_assert(!HashUtil::FindMapObject(mUdpSocketMap, pUdpSocket));
		pSocket->SetSocket(pUdpSocket);
		mUdpSocketMap.insert(UdpSocketMap::value_type(pUdpSocket, pSocket));
	}
	if (!HashUtil::FindMapObject(mIdSocketMap, pSocket->GetConnectionId()))
	{
		log_trace("Adding DualSocket with ID "+
			strutil::ReplaceCtrlChars(pSocket->GetConnectionId(), '.')+
			(pSocket->GetTcpSocket()?" TCP set,":" no TCP,")+str()+
			(pSocket->GetUdpSocket()?" UDP set.":" no UDP."));
		mIdSocketMap.insert(IdSocketMap::value_type(pSocket->GetConnectionId(), pSocket));
	}
	else
	{
		log_debug("Appending info to DualSocket with ID "+
			strutil::ReplaceCtrlChars(pSocket->GetConnectionId(), '.') +
			(pSocket->GetTcpSocket()?" TCP set,":" no TCP,")+str()+
			(pSocket->GetUdpSocket()?" UDP set.":" no UDP."));
	}
	if (!pSocket->GetTcpSocket() || !pSocket->GetUdpSocket())
	{
		log_trace("Adding a not-yet-fully-connected DualSocket to 'pending dual' list.");
		mPendingDualConnectMap.PushBack(pSocket, HiResTimer(false));
	}
	else
	{
		log_trace("Dropping a fully connected DualSocket from 'pending dual' list.");
		mPendingDualConnectMap.Remove(pSocket);
	}
}

void DualMuxSocket::KillNonDualConnected()
{
	for (SocketTimeMap::Iterator x = mPendingDualConnectMap.First(); x != mPendingDualConnectMap.End();)
	{
		HiResTimer& lTime = x.GetObject();
		if (lTime.QueryTimeDiff() >= mConnectDualTimeout)
		{
			log_debug("Connected socket dual-timed out => dropped.");
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



DualSocket::DualSocket(DualMuxSocket* pMuxSocket, const std::string& pConnectionId):
	ConnectionWithId(),
	mMuxSocket(pMuxSocket),
	mTcpSocket(0),
	mUdpSocket(0),
	mDefaultSafeSend(false),
	mDefaultSafeReceive(false)
{
	log_trace("DualSocket()");
	SetConnectionId(pConnectionId);
}

bool DualSocket::SetSocket(TcpVSocket* pSocket)
{
	bool lOk = (!mTcpSocket);
	deb_assert(lOk);
	if (lOk)
	{
		mTcpSocket = pSocket;
	}
	return (lOk);
}

bool DualSocket::SetSocket(UdpVSocket* pSocket)
{
	bool lOk = (!mUdpSocket);
	deb_assert(lOk);
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

void DualSocket::SetSafeSend(bool pSafe)
{
	mDefaultSafeSend = pSafe;
}

Datagram& DualSocket::GetSendBuffer() const
{
	return GetSendBuffer(mDefaultSafeSend);
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

IOError DualSocket::AppendSendBuffer(const void* pData, int pLength)
{
	return AppendSendBuffer(mDefaultSafeSend, pData, pLength);
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
		log_volatile(mLog.Tracef("TCP send returned %i.", lTcpSentCount));
	}
	int lUdpSentCount = 0;
	if (mUdpSocket && !mUdpSocket->GetInSenderList())	// If it is in the sender list, it will be served later.
	{
		lUdpSentCount = mUdpSocket->SendBuffer();
		log_volatile(mLog.Tracef("UDP send returned %i.", lUdpSentCount));
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

void DualSocket::SetSafeReceive(bool pSafe)
{
	mDefaultSafeReceive = pSafe;
}

int DualSocket::Receive(void* pData, int pLength)
{
	return Receive(mDefaultSafeReceive, pData, pLength);
}

int DualSocket::Receive(bool pSafe, void* pData, int pLength)
{
	int lReceiveCount;
	if (pSafe)
	{
		lReceiveCount = mTcpSocket->Receive(pData, pLength, true);
		//log_volatile(mLog.Tracef("TCP recv returned %i.", lReceiveCount));
	}
	else
	{
		lReceiveCount = mUdpSocket->Receive(pData, pLength);
		//log_volatile(mLog.Tracef("UDP recv returned %i.", lReceiveCount));
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
	log_trace("~DualSocket()");
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



const double TcpMuxSocket::DEFAULT_CONNECT_ID_TIMEOUT = 2.0;
const double DualMuxSocket::DEFAULT_CONNECT_DUAL_TIMEOUT = 5.0;

loginstance(NETWORK, TcpMuxSocket);
loginstance(NETWORK, TcpVSocket);
loginstance(NETWORK, DualMuxSocket);
loginstance(NETWORK, DualSocket);



}
