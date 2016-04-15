
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/ContextObjectAttribute.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/Packer.h"
#include "../Include/ContextObject.h"



namespace Cure
{



ContextObjectAttribute::ContextObjectAttribute(ContextObject* pContextObject, const str& pName):
	mContextObject(pContextObject),
	mName(pName),
	mNetworkType(TYPE_SERVER_BROADCAST)
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
	mFactory = pFactory;
}



int ContextObjectAttribute::QuerySend() const
{
	return PackerUnicodeString::Pack(0, mName);
}

int ContextObjectAttribute::Pack(uint8* pDestination)
{
	return PackerUnicodeString::Pack(pDestination, mName);
}

int ContextObjectAttribute::Unpack(ContextObject* pContextObject, const uint8* pSource, int pMaxSize)
{
	str lAttributeName;
	int lSize = PackerUnicodeString::Unpack(lAttributeName, pSource, pMaxSize);
	if (lSize < 0)
	{
		return -1;
	}

	ContextObjectAttribute* lAttribute = pContextObject->GetAttribute(lAttributeName);
	if (!lAttribute)
	{
		deb_assert(mFactory);
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



ContextObjectAttribute::NetworkType ContextObjectAttribute::GetNetworkType() const
{
	return mNetworkType;
}

void ContextObjectAttribute::SetNetworkType(NetworkType pNetworkType)
{
	mNetworkType = pNetworkType;
}



void ContextObjectAttribute::operator=(const ContextObjectAttribute&)
{
	deb_assert(false);
}



ContextObjectAttribute::Factory ContextObjectAttribute::mFactory = 0;



}
