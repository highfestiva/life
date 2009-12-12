
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
	Lepra::ScopeLock lLock(&mLock);
	SetMuxSocket(0);
}



void NetworkAgent::SetPacketFactory(PacketFactory* pPacketFactory)
{
	Lepra::ScopeLock lLock(&mLock);
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



Lepra::Lock* NetworkAgent::GetLock() const
{
	return (&mLock);
}



bool NetworkAgent::IsOpen() const
{
	return (mMuxSocket && mMuxSocket->IsOpen());
}

Lepra::uint64 NetworkAgent::GetSentByteCount(bool pSafe) const
{
	return (mMuxSocket->GetSentByteCount(pSafe));
}

Lepra::uint64 NetworkAgent::GetReceivedByteCount(bool pSafe) const
{
	return (mMuxSocket->GetReceivedByteCount(pSafe));
}

Lepra::uint64 NetworkAgent::GetTotalSentByteCount() const
{
	return (mMuxSocket->GetSentByteCount(true)+mMuxSocket->GetSentByteCount(false));
}

Lepra::uint64 NetworkAgent::GetTotalReceivedByteCount() const
{
	return (mMuxSocket->GetReceivedByteCount(true)+mMuxSocket->GetReceivedByteCount(false));
}

unsigned NetworkAgent::GetConnectionCount() const
{
	return (mMuxSocket->GetConnectionCount());
}


bool NetworkAgent::SendStatusMessage(Lepra::GameSocket* pSocket, Lepra::int32 pInteger, RemoteStatus pStatus, Lepra::UnicodeString pMessage, Packet* pPacket)
{
	pPacket->Release();
	MessageStatus* lStatus = (MessageStatus*)mPacketFactory->GetMessageFactory()->Allocate(MESSAGE_TYPE_STATUS);
	pPacket->AddMessage(lStatus);
	lStatus->Store(pPacket, pStatus, pInteger, pMessage);
	bool lOk = PlaceInSendBuffer(true, pSocket, pPacket);
	return (lOk);
}

bool NetworkAgent::SendNumberMessage(bool pSafe, Lepra::GameSocket* pSocket, Cure::MessageNumber::InfoType pInfo, Lepra::int32 pInteger, Lepra::float32 pFloat)
{
	Packet* lPacket = mPacketFactory->Allocate();
	MessageNumber* lNumber = (MessageNumber*)mPacketFactory->GetMessageFactory()->Allocate(MESSAGE_TYPE_NUMBER);
	lPacket->AddMessage(lNumber);
	lNumber->Store(lPacket, pInfo, pInteger, pFloat);
	bool lOk = PlaceInSendBuffer(pSafe, pSocket, lPacket);
	GetPacketFactory()->Release(lPacket);
	return (lOk);
}

bool NetworkAgent::SendObjectFullPosition(Lepra::GameSocket* pSocket, GameObjectId pInstanceId, Lepra::int32 pFrameIndex, const ObjectPositionalData& pData)
{
	Packet* lPacket = mPacketFactory->Allocate();
	MessageObjectPosition* lPosition = (MessageObjectPosition*)mPacketFactory->GetMessageFactory()->Allocate(MESSAGE_TYPE_OBJECT_POSITION);
	lPacket->AddMessage(lPosition);
	lPosition->Store(lPacket, pInstanceId, pFrameIndex, pData);
	bool lOk = PlaceInSendBuffer(false, pSocket, lPacket);
	GetPacketFactory()->Release(lPacket);
	return (lOk);
}

bool NetworkAgent::PlaceInSendBuffer(bool pSafe, Lepra::GameSocket* pSocket, Packet* pPacket)
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
		lOk = (pSocket->AppendSendBuffer(pSafe, pPacket->GetReadBuffer(), pPacket->GetPacketSize()) == Lepra::IO_OK);
	}
	if (!lOk)
	{
		mLog.AError("PlaceInSendBuffer(): unable to place data in socket output buffer.");
	}
	return (lOk);
}

void NetworkAgent::SetMuxSocket(Lepra::GameMuxSocket* pSocket)
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
