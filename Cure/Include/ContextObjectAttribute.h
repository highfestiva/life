
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "cure.h"
#include "../../thirdparty/FastDelegate/FastDelegate.h"



namespace cure {



class ContextObject;



class ContextObjectAttribute {
public:
	enum NetworkType {
		kTypeServer,		// Server -> avatar client.
		kTypeServerBroadcast,	// Server -> all clients.
		kTypeBoth,		// Server <--> avatar client.
		TYPE_BOTH_BROADCAST	// Server -> all clients, avatar client -> server.
	};

	ContextObjectAttribute(ContextObject* context_object, const str& name);
	virtual ~ContextObjectAttribute();
	const str& GetName() const;

	typedef fastdelegate::FastDelegate2<ContextObject*, const str&, ContextObjectAttribute*> Factory;
	static void SetCreator(const Factory& factory);

	virtual int QuerySend() const;	// Returns number of bytes it needs to send.
	virtual int Pack(uint8* destination);
	static int Unpack(ContextObject* context_object, const uint8* source, int max_size);	// Retuns number of bytes consumed, or -1.
	NetworkType GetNetworkType() const;
	void SetNetworkType(NetworkType network_type);

protected:
	virtual int Unpack(const uint8* source, int max_size) = 0;	// Retuns number of bytes unpacked, or -1.

	ContextObject* context_object_;
	const str name_;
	NetworkType network_type_;
	static Factory factory_;

	void operator=(const ContextObjectAttribute&);
};



}
