
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "Cure.h"
#include "../../ThirdParty/FastDelegate/FastDelegate.h"



namespace Cure
{



class ContextObject;



class ContextObjectAttribute
{
public:
	enum NetworkType
	{
		TYPE_SERVER,		// Server -> avatar client.
		TYPE_SERVER_BROADCAST,	// Server -> all clients.
		TYPE_BOTH,		// Server <--> avatar client.
		TYPE_BOTH_BROADCAST	// Server -> all clients, avatar client -> server.
	};

	ContextObjectAttribute(ContextObject* pContextObject, const str& pName);
	virtual ~ContextObjectAttribute();
	const str& GetName() const;

	typedef fastdelegate::FastDelegate2<ContextObject*, const str&, ContextObjectAttribute*> Factory;
	static void SetCreator(const Factory& pFactory);

	virtual int QuerySend() const;	// Returns number of bytes it needs to send.
	virtual int Pack(uint8* pDestination);
	static int Unpack(ContextObject* pContextObject, const uint8* pSource, int pMaxSize);	// Retuns number of bytes consumed, or -1.
	NetworkType GetNetworkType() const;
	void SetNetworkType(NetworkType pNetworkType);

protected:
	virtual int Unpack(const uint8* pSource, int pMaxSize) = 0;	// Retuns number of bytes unpacked, or -1.

	ContextObject* mContextObject;
	const str mName;
	NetworkType mNetworkType;
	static Factory mFactory;

	void operator=(const ContextObjectAttribute&);
};



}
