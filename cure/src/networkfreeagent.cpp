
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/networkfreeagent.h"
#include "../include/runtimevariable.h"



namespace cure {



NetworkFreeAgent::NetworkFreeAgent():
	mux_socket_(0) {
}

NetworkFreeAgent::~NetworkFreeAgent() {
	delete mux_socket_;
	mux_socket_ = 0;
}



bool NetworkFreeAgent::Tick() {
	if (!mux_socket_) {
		SocketAddress address;
		str str_address = "0.0.0.0:12345";
		if (!address.Resolve(str_address)) {
			log_.Warningf("Could not resolve address '%s'.", str_address.c_str());
			str_address = ":12345";
			if (!address.Resolve(str_address)) {
				log_.Errorf("Could not resolve address '%s'!", str_address.c_str());
				return false;
			}
		}
		mux_socket_ = new MuxIoSocket("FreeAgent", address, false);
	}

	bool ok = true;
	VIoSocket* _socket;
	while (mux_socket_ && (_socket = mux_socket_->PopSenderSocket()) != 0) {
		ok &= (_socket->SendBuffer() > 0);
	}
	return (ok);
}



NetworkFreeAgent::MuxIoSocket* NetworkFreeAgent::GetMuxIoSocket() const {
	return mux_socket_;
}

void NetworkFreeAgent::AddFilterIoSocket(VIoSocket* socket, const DropFilterCallback& on_drop_callback) {
	socket_receive_filter_table_.insert(SocketReceiveFilterTable::value_type(socket, on_drop_callback));
}

void NetworkFreeAgent::RemoveAllFilterIoSockets() {
	socket_receive_filter_table_.clear();
}

void NetworkFreeAgent::KillIoSocket(VIoSocket* socket) {
	SocketReceiveFilterTable::iterator x = socket_receive_filter_table_.find(socket);
	if (x != socket_receive_filter_table_.end()) {
		x->second(x->first);
		socket_receive_filter_table_.erase(x);
	}
	if (mux_socket_) {
		mux_socket_->CloseSocket(socket);
	}
}



loginstance(kNetworkClient, NetworkFreeAgent);



}
