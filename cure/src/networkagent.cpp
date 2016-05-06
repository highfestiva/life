
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/network.h"
#include "../../lepra/include/socket.h"
#include "../include/networkagent.h"



namespace cure {



NetworkAgent::NetworkAgent(RuntimeVariableScope* variable_scope):
	variable_scope_(variable_scope),
	mux_socket_(0),
	packet_factory_(new PacketFactory(new MessageFactory())) {
}

NetworkAgent::~NetworkAgent() {
	variable_scope_ = 0;
	SetPacketFactory(0);
	Stop();
}

void NetworkAgent::Stop() {
	ScopeLock lock(&lock_);
	SetMuxSocket(0);
}



void NetworkAgent::SetPacketFactory(PacketFactory* packet_factory) {
	ScopeLock lock(&lock_);
	delete (packet_factory_);
	packet_factory_ = packet_factory;
	/*if (mux_socket_) {
		mux_socket_->SetDatagramReceiver(packet_factory_);
	}*/
}

PacketFactory* NetworkAgent::GetPacketFactory() const {
	return (packet_factory_);
}



Lock* NetworkAgent::GetLock() const {
	return (&lock_);
}



bool NetworkAgent::IsOpen() const {
	return (mux_socket_ && mux_socket_->IsOpen());
}

uint64 NetworkAgent::GetSentByteCount() const {
	return (mux_socket_->GetSentByteCount());
}

uint64 NetworkAgent::GetReceivedByteCount() const {
	return (mux_socket_->GetReceivedByteCount());
}

unsigned NetworkAgent::GetConnectionCount() const {
	return (mux_socket_->GetConnectionCount());
}

const SocketAddress& NetworkAgent::GetLocalAddress() const {
	return mux_socket_->GetLocalAddress();
}



bool NetworkAgent::SendStatusMessage(VSocket* socket, int32 integer, RemoteStatus status,
	MessageStatus::InfoType info_type, str message, Packet* packet) {
	packet->Release();
	MessageStatus* _status = (MessageStatus*)packet_factory_->GetMessageFactory()->Allocate(kMessageTypeStatus);
	packet->AddMessage(_status);
	_status->Store(packet, status, info_type, integer, message);
	bool ok = PlaceInSendBuffer(true, socket, packet);
	return (ok);
}

bool NetworkAgent::SendNumberMessage(bool safe, VSocket* socket, MessageNumber::InfoType info, int32 integer, float32 f, Packet* packet) {
	Packet* _packet = packet;
	if (!packet) {
		_packet = packet_factory_->Allocate();
	}
	MessageNumber* number = (MessageNumber*)packet_factory_->GetMessageFactory()->Allocate(kMessageTypeNumber);
	_packet->AddMessage(number);
	number->Store(_packet, info, integer, f);
	bool ok = PlaceInSendBuffer(safe, socket, _packet);
	if (!packet) {
		GetPacketFactory()->Release(_packet);
	}
	return (ok);
}

bool NetworkAgent::SendObjectFullPosition(VSocket* socket, GameObjectId instance_id, int32 frame_index, const ObjectPositionalData& data) {
	Packet* _packet = packet_factory_->Allocate();
	MessageObjectPosition* position = (MessageObjectPosition*)packet_factory_->GetMessageFactory()->Allocate(kMessageTypeObjectPosition);
	_packet->AddMessage(position);
	position->Store(_packet, instance_id, frame_index, data);
	bool ok = PlaceInSendBuffer(false, socket, _packet);
	GetPacketFactory()->Release(_packet);
	return (ok);
}

bool NetworkAgent::PlaceInSendBuffer(bool safe, VSocket* socket, Packet* packet) {
	if (!socket) {
		log_.Error("PlaceInSendBuffer(): unable send data via uninitialized socket.");
		return false;
	}

	/*for (size_t x = 0; x < packet->GetMessageCount(); ++x) {
		log_volatile(log_.Tracef("Sending message of type %i.", packet->GetMessageAt(x)->GetType()));
	}*/

	socket->SetSafeSend(safe);

	// Try to append this packet to the existing packet buffer.
	bool ok = packet->AppendToPacketBuffer(socket->GetSendBuffer());
	if (!ok) {
		packet->StoreHeader();
		// Buffer was full. We go for the socket way (send existing buffer, then copy this one for next send).
		ok = (socket->AppendSendBuffer(packet->GetReadBuffer(), packet->GetPacketSize()) == kIoOk);
	}
	if (!ok) {
		log_.Error("PlaceInSendBuffer(): unable to place data in socket output buffer.");
	}
	return (ok);
}

void NetworkAgent::SetMuxSocket(MuxSocket* socket) {
	delete (mux_socket_);
	mux_socket_ = socket;
	/*if (mux_socket_) {
		mux_socket_->SetDatagramReceiver(packet_factory_);
	}*/
}



loginstance(kNetwork, NetworkAgent);



}
