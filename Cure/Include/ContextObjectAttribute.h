
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "Cure.h"



namespace Cure
{



class ContextObject;



class ContextObjectAttribute
{
public:
	ContextObjectAttribute(ContextObject* pContextObject, const str& pName);
	virtual ~ContextObjectAttribute();
	const str& GetName() const;

	typedef ContextObjectAttribute* (*Factory)(ContextObject*, const str&);
	static void SetCreator(const Factory& pFactory);

	virtual int QuerySend() const = 0;	// Returns number of bytes it needs to send.
	virtual void Pack(uint8* pDestination) = 0;
	static int Unpack(ContextObject* pContextObject, const uint8* pSource, int pMaxSize);	// Retuns number of bytes consumed, or -1.

private:
	virtual int Unpack(const uint8* pSource, int pMaxSize) = 0;	// Retuns number of bytes unpacked, or -1.

	ContextObject* mContextObject;
	const str mName;
	static Factory mFactory;

	void operator=(const ContextObjectAttribute&);
};



}
