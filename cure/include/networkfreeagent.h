
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "socketiohandler.h"



namespace cure {



class NetworkFreeAgent: public SocketIoHandler {
public:
	NetworkFreeAgent();
	virtual ~NetworkFreeAgent();

	bool Tick();

private:
	virtual MuxIoSocket* GetMuxIoSocket() const;
	virtual void AddFilterIoSocket(VIoSocket* socket, const DropFilterCallback& on_drop_callback);
	virtual void RemoveAllFilterIoSockets();
	virtual void KillIoSocket(VIoSocket* socket);

	typedef std::unordered_map<VIoSocket*, DropFilterCallback, LEPRA_VOIDP_HASHER> SocketReceiveFilterTable;

	MuxIoSocket* mux_socket_;
	SocketReceiveFilterTable socket_receive_filter_table_;

	logclass();
};



}
