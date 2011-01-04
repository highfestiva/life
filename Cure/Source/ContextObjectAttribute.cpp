
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ContextObjectAttribute.h"
#include <assert.h>
#include "../../Lepra/Include/Packer.h"
#include "../Include/ContextObject.h"



namespace Cure
{



ContextObjectAttribute::ContextObjectAttribute(ContextObject* pContextObject, const str& pName):
	mContextObject(pContextObject),
	mName(pName)
{
	mContextObject->AddAttribute(this);
}

ContextObjectAttribute::~ContextObjectAttribute()
{
	mContextObject = 0;
}



const str& ContextObjectAttribute::GetName() const
{
	return mName;
}



void ContextObjectAttribute::SetCreator(const Factory& pFactory)
{
	assert(mFactory == 0);
	mFactory = pFactory;
}



int ContextObjectAttribute::QuerySend() const
{
	wstr lAttributeName = wstrutil::Encode(mName);
	return PackerUnicodeString::Pack(0, lAttributeName);
}

int ContextObjectAttribute::Pack(uint8* pDestination)
{
	wstr lAttributeName = wstrutil::Encode(mName);
	return PackerUnicodeString::Pack(pDestination, lAttributeName);
}

int ContextObjectAttribute::Unpack(ContextObject* pContextObject, const uint8* pSource, int pMaxSize)
{
	wstr lAttributeName;
	int lSize = PackerUnicodeString::Unpack(lAttributeName, pSource, pMaxSize);
	if (lSize < 0)
	{
		return -1;
	}
	ContextObjectAttribute* lAttribute = pContextObject->GetAttribute(strutil::Encode(lAttributeName));
	if (!lAttribute)
	{
		assert(mFactory);
		lAttribute = mFactory(pContextObject, lAttributeName);
		if (!lAttribute)
		{
			return -1;
		}
	}
	int lParamsSize = lAttribute->Unpack(&pSource[lSize], pMaxSize-lSize);
	if (lParamsSize < 0)
	{
		return -1;
	}
	lSize += lParamsSize;
	return lSize;
}



void ContextObjectAttribute::operator=(const ContextObjectAttribute&)
{
	assert(false);
}



ContextObjectAttribute::Factory ContextObjectAttribute::mFactory = 0;



}
