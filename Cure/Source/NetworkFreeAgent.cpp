
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
 


/*#include "../../Lepra/Include/IOBuffer.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Reader.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../Lepra/Include/Timer.h"*/
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
		str lStrAddress = _T("0.0.0.0:12345");
		if (!lAddress.Resolve(lStrAddress))
		{
			mLog.Warningf(_T("Could not resolve address '%s'."), lStrAddress.c_str());
			lStrAddress = _T(":12345");
			if (!lAddress.Resolve(lStrAddress))
			{
				mLog.Errorf(_T("Could not resolve address '%s'!"), lStrAddress.c_str());
				return false;
			}
		}
		mMuxSocket = new MuxIoSocket(_T("FreeAgent"), lAddress, false);
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



LOG_CLASS_DEFINE(NETWORK_CLIENT, NetworkFreeAgent);



}
