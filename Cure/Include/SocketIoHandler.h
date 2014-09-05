
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../ThirdParty/FastDelegate/FastDelegate.h"
#include "../../Lepra/Include/Socket.h"
#include "Cure.h"



namespace Lepra
{
class UdpMuxSocket;
class UdpVSocket;
class DualMuxSocket;
class DualSocket;
}



namespace Cure
{



class SocketIoHandler
{
public:
	//typedef DualSocket VIoSocket;
	//typedef DualMuxSocket MuxIoSocket;
	typedef UdpVSocket VIoSocket;
	typedef UdpMuxSocket MuxIoSocket;
	typedef fastdelegate::FastDelegate1<VIoSocket*, void> DropFilterCallback;

	virtual MuxIoSocket* GetMuxIoSocket() const = 0;
	virtual void AddFilterIoSocket(VIoSocket* pSocket, const DropFilterCallback& pOnDropCallback) = 0;
	virtual void RemoveAllFilterIoSockets() = 0;
	virtual void KillIoSocket(VIoSocket* pSocket) = 0;
};



}
