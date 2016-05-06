
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/thread.h"
#include "../../lepra/include/socketaddress.h"
#include "packet.h"
#include "remotestatus.h"



namespace lepra {
class UdpVSocket;
class UdpMuxSocket;
}



namespace cure {



class NetworkAgent {
public:
	//typedef DualSocket VSocket;
	//typedef DualMuxSocket MuxSocket;
	typedef UdpVSocket VSocket;
	typedef UdpMuxSocket MuxSocket;

	enum ReceiveStatus {
		kReceiveOk,
		kReceiveNoData,
		kReceiveParseError,
		kReceiveConnectionBroken
	};

	NetworkAgent(RuntimeVariableScope* variable_scope);
	virtual ~NetworkAgent();
	virtual void Stop();

	void SetPacketFactory(PacketFactory* packet_factory);
	PacketFactory* GetPacketFactory() const;

	Lock* GetLock() const;

	virtual bool SendAll() = 0;
	bool IsOpen() const;
	uint64 GetSentByteCount() const;
	uint64 GetReceivedByteCount() const;
	unsigned GetConnectionCount() const;
	const SocketAddress& GetLocalAddress() const;

	bool SendStatusMessage(VSocket* socket, int32 integer, RemoteStatus status,
		MessageStatus::InfoType info_type, str message, Packet* packet);
	bool SendNumberMessage(bool safe, VSocket* socket, cure::MessageNumber::InfoType info, int32 integer, float32 f, Packet* packet = 0);
	bool SendObjectFullPosition(VSocket* socket, GameObjectId instance_id, int32 frame_index, const ObjectPositionalData& data);
	bool PlaceInSendBuffer(bool safe, VSocket* socket, Packet* packet);

protected:
	void SetMuxSocket(MuxSocket* socket);

	mutable Lock lock_;
	RuntimeVariableScope* variable_scope_;
	MuxSocket* mux_socket_;
	PacketFactory* packet_factory_;

private:
	logclass();
};



}
