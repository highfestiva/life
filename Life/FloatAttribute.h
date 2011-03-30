
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Cure/Include/ContextObjectAttribute.h"
#include "Life.h"



namespace Life
{



class FloatAttribute: public Cure::ContextObjectAttribute
{
	typedef Cure::ContextObjectAttribute Parent;
public:
	FloatAttribute(Cure::ContextObject* pContextObject, const str& pName, float pValue);
	virtual ~FloatAttribute();

	float GetValue() const;
	void SetValue(float pValue);

private:
	virtual int QuerySend() const;	// Returns number of bytes it needs to send.
	virtual int Pack(uint8* pDestination);
	virtual int Unpack(const uint8* pSource, int pMaxSize);	// Retuns number of bytes unpacked, or -1.
	virtual NetworkType GetNetworkType() const;

	bool mIsUpdated;
	float mValue;

	void operator=(const FloatAttribute&);
};



}
