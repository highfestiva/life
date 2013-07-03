
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "Cure.h"
#include "ContextObjectAttribute.h"



namespace Cure
{



class FloatAttribute: public ContextObjectAttribute
{
	typedef ContextObjectAttribute Parent;
public:
	FloatAttribute(ContextObject* pContextObject, const str& pName, float pValue);
	virtual ~FloatAttribute();

	float GetValue() const;
	void SetValue(float pValue);

private:
	virtual int QuerySend() const;	// Returns number of bytes it needs to send.
	virtual int Pack(uint8* pDestination);
	virtual int Unpack(const uint8* pSource, int pMaxSize);	// Retuns number of bytes unpacked, or -1.

	bool mIsUpdated;
	float mValue;

	void operator=(const FloatAttribute&);
};



}
