
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
 


#include "pch.h"
#include "../Include/NetworkFreeAgent.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



NetworkFreeAgent::NetworkFreeAgent():
	mMuxSocket(0)
{
}

NetworkFreeAgent::~NetworkFreeAgent()
{
	delete mMuxSocket;
	mMuxSocket = 0;
}



bool NetworkFreeAgent::Tick()
{
	if (!mMuxSocket)
	{
		SocketAddress lAddress;
		str lStrAddress = "0.0.0.0:12345";
		if (!lAddress.Resolve(lStrAddress))
		{
			mLog.Warningf("Could not resolve address '%s'.", lStrAddress.c_str());
			lStrAddress = ":12345";
			if (!lAddress.Resolve(lStrAddress))
			{
				mLog.Errorf("Could not resolve address '%s'!", lStrAddress.c_str());
				return false;
			}
		}
		mMuxSocket = new MuxIoSocket("FreeAgent", lAddress, false);
	}

	bool lOk = true;
	VIoSocket* lSocket;
	while (mMuxSocket && (lSocket = mMuxSocket->PopSenderSocket()) != 0)
	{
		lOk &= (lSocket->SendBuffer() > 0);
	}
	return (lOk);
}



NetworkFreeAgent::MuxIoSocket* NetworkFreeAgent::GetMuxIoSocket() const
{
	return mMuxSocket;
}

void NetworkFreeAgent::AddFilterIoSocket(VIoSocket* pSocket, const DropFilterCallback& pOnDropCallback)
{
	mSocketReceiveFilterTable.insert(SocketReceiveFilterTable::value_type(pSocket, pOnDropCallback));
}

void NetworkFreeAgent::RemoveAllFilterIoSockets()
{
	mSocketReceiveFilterTable.clear();
}

void NetworkFreeAgent::KillIoSocket(VIoSocket* pSocket)
{
	SocketReceiveFilterTable::iterator x = mSocketReceiveFilterTable.find(pSocket);
	if (x != mSocketReceiveFilterTable.end())
	{
		x->second(x->first);
		mSocketReceiveFilterTable.erase(x);
	}
	if (mMuxSocket)
	{
		mMuxSocket->CloseSocket(pSocket);
	}
}



loginstance(NETWORK_CLIENT, NetworkFreeAgent);



}
