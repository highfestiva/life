
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games
 


#include "../../Lepra/Include/Network.h"
#include "../Include/NetworkAgent.h"



namespace Cure
{



NetworkAgent::NetworkAgent(RuntimeVariableScope* pVariableScope):
	mVariableScope(pVariableScope),
	mMuxSocket(0),
	mPacketFactory(new PacketFactory(new MessageFactory()))
{
}

NetworkAgent::~NetworkAgent()
{
	mVariableScope = 0;
	SetPacketFactory(0);
	Stop();
}

void NetworkAgent::Stop()
{
	ScopeLock lLock(&mLock);
	SetMuxSocket(0);
}



void NetworkAgent::SetPacketFactory(PacketFactory* pPacketFactory)
{
	ScopeLock lLock(&mLock);
	delete (mPacketFactory);
	mPacketFactory = pPacketFactory;
	if (mMuxSocket)
	{
		mMuxSocket->SetDatagramReceiver(mPacketFactory);
	}
}

PacketFactory* NetworkAgent::GetPacketFactory() const
{
	return (mPacketFactory);
}



Lock* NetworkAgent::GetLock() const
{
	return (&mLock);
}



bool NetworkAgent::IsOpen() const
{
	return (mMuxSocket && mMuxSocket->IsOpen());
}

uint64 NetworkAgent::GetSentByteCount(bool pSafe) const
{
	return (mMuxSocket->GetSentByteCount(pSafe));
}

uint64 NetworkAgent::GetReceivedByteCount(bool pSafe) const
{
	return (mMuxSocket->GetReceivedByteCount(pSafe));
}

uint64 NetworkAgent::GetTotalSentByteCount() const
{
	return (mMuxSocket->GetSentByteCount(true)+mMuxSocket->GetSentByteCount(false));
}

uint64 NetworkAgent::GetTotalReceivedByteCount() const
{
	return (mMuxSocket->GetReceivedByteCount(true)+mMuxSocket->GetReceivedByteCount(false));
}

unsigned NetworkAgent::GetConnectionCount() const
{
	return (mMuxSocket->GetConnectionCount());
}


bool NetworkAgent::SendStatusMessage(GameSocket* pSocket, int32 pInteger, RemoteStatus pStatus, wstr pMessage, Packet* pPacket)
{
	pPacket->Release();
	MessageStatus* lStatus = (MessageStatus*)mPacketFactory->GetMessageFactory()->Allocate(MESSAGE_TYPE_STATUS);
	pPacket->AddMessage(lStatus);
	lStatus->Store(pPacket, pStatus, pInteger, pMessage);
	bool lOk = PlaceInSendBuffer(true, pSocket, pPacket);
	return (lOk);
}

bool NetworkAgent::SendNumberMessage(bool pSafe, GameSocket* pSocket, Cure::MessageNumber::InfoType pInfo, int32 pInteger, float32 pFloat)
{
	Packet* lPacket = mPacketFactory->Allocate();
	MessageNumber* lNumber = (MessageNumber*)mPacketFactory->GetMessageFactory()->Allocate(MESSAGE_TYPE_NUMBER);
	lPacket->AddMessage(lNumber);
	lNumber->Store(lPacket, pInfo, pInteger, pFloat);
	bool lOk = PlaceInSendBuffer(pSafe, pSocket, lPacket);
	GetPacketFactory()->Release(lPacket);
	return (lOk);
}

bool NetworkAgent::SendObjectFullPosition(GameSocket* pSocket, GameObjectId pInstanceId, int32 pFrameIndex, const ObjectPositionalData& pData)
{
	Packet* lPacket = mPacketFactory->Allocate();
	MessageObjectPosition* lPosition = (MessageObjectPosition*)mPacketFactory->GetMessageFactory()->Allocate(MESSAGE_TYPE_OBJECT_POSITION);
	lPacket->AddMessage(lPosition);
	lPosition->Store(lPacket, pInstanceId, pFrameIndex, pData);
	bool lOk = PlaceInSendBuffer(false, pSocket, lPacket);
	GetPacketFactory()->Release(lPacket);
	return (lOk);
}

bool NetworkAgent::PlaceInSendBuffer(bool pSafe, GameSocket* pSocket, Packet* pPacket)
{
	bool lOk = (pSocket != 0);

	/*for (size_t x = 0; x < pPacket->GetMessageCount(); ++x)
	{
		log_volatile(mLog.Tracef(_T("Sending message of type %i."), pPacket->GetMessageAt(x)->GetType()));
	}*/

	// Try to append this packet to the existing packet buffer.
	if (lOk)
	{
		lOk = pPacket->AppendToPacketBuffer(pSocket->GetSendBuffer(pSafe));
	}
	if (!lOk)
	{
		pPacket->StoreHeader();
		// Buffer was full. We go for the socket way (send existing buffer, then copy this one for next send).
		lOk = (pSocket->AppendSendBuffer(pSafe, pPacket->GetReadBuffer(), pPacket->GetPacketSize()) == IO_OK);
	}
	if (!lOk)
	{
		mLog.AError("PlaceInSendBuffer(): unable to place data in socket output buffer.");
	}
	return (lOk);
}

void NetworkAgent::SetMuxSocket(GameMuxSocket* pSocket)
{
	delete (mMuxSocket);
	mMuxSocket = pSocket;
	if (mMuxSocket)
	{
		mMuxSocket->SetDatagramReceiver(mPacketFactory);
	}
}



LOG_CLASS_DEFINE(NETWORK, NetworkAgent);



}
