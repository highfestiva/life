
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/IntAttribute.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/Packer.h"
#include "../Include/ContextManager.h"
#include "../Include/GameManager.h"



namespace Cure
{



IntAttribute::IntAttribute(ContextObject* pContextObject, const str& pName, int pValue):
	Parent(pContextObject, pName),
	mIsUpdated(true),
	mValue(pValue)
{
}

IntAttribute::~IntAttribute()
{
}



int IntAttribute::GetValue() const
{
	return mValue;
}

void IntAttribute::SetValue(int pValue)
{
	if (mValue != pValue)
	{
		mValue = pValue;
		mIsUpdated = true;
		mContextObject->OnAttributeUpdated(this);
	}
}



int IntAttribute::QuerySend() const
{
	return mIsUpdated? Parent::QuerySend()+sizeof(mValue) : 0;
}

int IntAttribute::Pack(uint8* pDestination)
{
	const int lParentSize = Parent::Pack(pDestination);
	pDestination += lParentSize;
	PackerInt32::Pack(pDestination, mValue);
	mIsUpdated = false;
	return lParentSize + sizeof(mValue);
}

int IntAttribute::Unpack(const uint8* pSource, int pMaxSize)
{
	if (pMaxSize < sizeof(mValue))
	{
		return -1;
	}
	PackerInt32::Unpack(mValue, pSource, pMaxSize);
	mIsUpdated = mContextObject->GetManager()->GetGameManager()->IsServer();
	return sizeof(mValue);
}



void IntAttribute::operator=(const IntAttribute&)
{
	deb_assert(false);
}



}
