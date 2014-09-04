
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/Lock.h"
#include "../../Lepra/Include/SocketAddress.h"
#include "Packet.h"
#include "RemoteStatus.h"



namespace Lepra
{
class UdpVSocket;
class UdpMuxSocket;
}



namespace Cure
{



class NetworkAgent
{
public:
	//typedef DualSocket VSocket;
	//typedef DualMuxSocket MuxSocket;
	typedef UdpVSocket VSocket;
	typedef UdpMuxSocket MuxSocket;

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
	uint64 GetSentByteCount() const;
	uint64 GetReceivedByteCount() const;
	unsigned GetConnectionCount() const;
	const SocketAddress& GetLocalAddress() const;

	bool SendStatusMessage(VSocket* pSocket, int32 pInteger, RemoteStatus pStatus,
		MessageStatus::InfoType pInfoType, wstr pMessage, Packet* pPacket);
	bool SendNumberMessage(bool pSafe, VSocket* pSocket, Cure::MessageNumber::InfoType pInfo, int32 pInteger, float32 pFloat, Packet* pPacket = 0);
	bool SendObjectFullPosition(VSocket* pSocket, GameObjectId pInstanceId, int32 pFrameIndex, const ObjectPositionalData& pData);
	bool PlaceInSendBuffer(bool pSafe, VSocket* pSocket, Packet* pPacket);

protected:
	void SetMuxSocket(MuxSocket* pSocket);

	mutable Lock mLock;
	RuntimeVariableScope* mVariableScope;
	MuxSocket* mMuxSocket;
	PacketFactory* mPacketFactory;

private:
	logclass();
};



}
