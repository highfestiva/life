
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

	Lock* GetLock() const;

	virtual bool SendAll() = 0;
	bool IsOpen() const;
	uint64 GetSentByteCount(bool pSafe) const;
	uint64 GetReceivedByteCount(bool pSafe) const;
	uint64 GetTotalSentByteCount() const;
	uint64 GetTotalReceivedByteCount() const;
	unsigned GetConnectionCount() const;

	bool SendStatusMessage(GameSocket* pSocket, int32 pInteger, RemoteStatus pStatus, wstr pMessage, Packet* pPacket);
	bool SendNumberMessage(bool pSafe, GameSocket* pSocket, Cure::MessageNumber::InfoType pInfo, int32 pInteger, float32 pFloat);
	bool SendObjectFullPosition(GameSocket* pSocket, GameObjectId pInstanceId, int32 pFrameIndex, const ObjectPositionalData& pData);
	bool PlaceInSendBuffer(bool pSafe, GameSocket* pSocket, Packet* pPacket);

protected:
	void SetMuxSocket(GameMuxSocket* pSocket);

	mutable Lock mLock;
	RuntimeVariableScope* mVariableScope;
	GameMuxSocket* mMuxSocket;
	PacketFactory* mPacketFactory;

private:
	LOG_CLASS_DECLARE();
};



}
