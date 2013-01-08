
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "Cure.h"
#include "ContextObjectAttribute.h"



namespace Cure
{



class IntAttribute: public ContextObjectAttribute
{
	typedef ContextObjectAttribute Parent;
public:
	IntAttribute(ContextObject* pContextObject, const str& pName, int pValue);
	virtual ~IntAttribute();

	int GetValue() const;
	void SetValue(int pValue);

private:
	virtual int QuerySend() const;	// Returns number of bytes it needs to send.
	virtual int Pack(uint8* pDestination);
	virtual int Unpack(const uint8* pSource, int pMaxSize);	// Retuns number of bytes unpacked, or -1.

	bool mIsUpdated;
	int mValue;

	void operator=(const IntAttribute&);
};



}
