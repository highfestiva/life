
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

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
	virtual void KillIoSocket(VIoSocket* pSocket) = 0;
};



}
