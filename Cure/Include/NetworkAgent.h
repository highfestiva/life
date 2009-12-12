
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/Socket.h"
#include "../../Lepra/Include/Thread.h"
#include "Packet.h"
#include "RemoteStatus.h"



namespace Cure
{



class NetworkAgent
{
public:
	enum ReceiveStatus
	{
		RECEIVE_OK,
		RECEIVE_NO_DATA,
		RECEIVE_PARSE_ERROR,
		RECEIVE_CONNECTION_BROKEN
	};

	NetworkAgent(RuntimeVariableScope* pVariableScope);
	virtual ~NetworkAgent();
	virtual void Stop();

	void SetPacketFactory(PacketFactory* pPacketFactory);
	PacketFactory* GetPacketFactory() const;

	Lepra::Lock* GetLock() const;

	virtual bool SendAll() = 0;
	bool IsOpen() const;
	Lepra::uint64 GetSentByteCount(bool pSafe) const;
	Lepra::uint64 GetReceivedByteCount(bool pSafe) const;
	Lepra::uint64 GetTotalSentByteCount() const;
	Lepra::uint64 GetTotalReceivedByteCount() const;
	unsigned GetConnectionCount() const;

	bool SendStatusMessage(Lepra::GameSocket* pSocket, Lepra::int32 pInteger, RemoteStatus pStatus, Lepra::UnicodeString pMessage, Packet* pPacket);
	bool SendNumberMessage(bool pSafe, Lepra::GameSocket* pSocket, Cure::MessageNumber::InfoType pInfo, Lepra::int32 pInteger, Lepra::float32 pFloat);
	bool SendObjectFullPosition(Lepra::GameSocket* pSocket, GameObjectId pInstanceId, Lepra::int32 pFrameIndex, const ObjectPositionalData& pData);
	bool PlaceInSendBuffer(bool pSafe, Lepra::GameSocket* pSocket, Packet* pPacket);

protected:
	void SetMuxSocket(Lepra::GameMuxSocket* pSocket);

	mutable Lepra::Lock mLock;
	RuntimeVariableScope* mVariableScope;
	Lepra::GameMuxSocket* mMuxSocket;
	PacketFactory* mPacketFactory;

private:
	LOG_CLASS_DECLARE();
};



}
