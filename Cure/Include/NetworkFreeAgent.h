
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "SocketIoHandler.h"



namespace Cure
{



class NetworkFreeAgent: public SocketIoHandler
{
public:
	NetworkFreeAgent();
	virtual ~NetworkFreeAgent();

	bool Tick();

private:
	virtual MuxIoSocket* GetMuxIoSocket() const;
	virtual void AddFilterIoSocket(VIoSocket* pSocket, const DropFilterCallback& pOnDropCallback);
	virtual void RemoveAllFilterIoSockets();
	virtual void KillIoSocket(VIoSocket* pSocket);

	typedef std::unordered_map<VIoSocket*, DropFilterCallback, LEPRA_VOIDP_HASHER> SocketReceiveFilterTable;

	MuxIoSocket* mMuxSocket;
	SocketReceiveFilterTable mSocketReceiveFilterTable;

	LOG_CLASS_DECLARE();
};



}
