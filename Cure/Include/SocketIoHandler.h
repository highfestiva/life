
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../thirdparty/FastDelegate/FastDelegate.h"
#include "../../lepra/include/socket.h"
#include "cure.h"



namespace lepra {
class UdpMuxSocket;
class UdpVSocket;
class DualMuxSocket;
class DualSocket;
}



namespace cure {



class SocketIoHandler {
public:
	//typedef DualSocket VIoSocket;
	//typedef DualMuxSocket MuxIoSocket;
	typedef UdpVSocket VIoSocket;
	typedef UdpMuxSocket MuxIoSocket;
	typedef fastdelegate::FastDelegate1<VIoSocket*, void> DropFilterCallback;

	virtual MuxIoSocket* GetMuxIoSocket() const = 0;
	virtual void AddFilterIoSocket(VIoSocket* socket, const DropFilterCallback& on_drop_callback) = 0;
	virtual void RemoveAllFilterIoSockets() = 0;
	virtual void KillIoSocket(VIoSocket* socket) = 0;
};



}
