
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/FloatAttribute.h"
#include <assert.h>
#include "../../Lepra/Include/Packer.h"
#include "../Include/ContextManager.h"
#include "../Include/GameManager.h"



namespace Cure
{



FloatAttribute::FloatAttribute(ContextObject* pContextObject, const str& pName, float pValue):
	Parent(pContextObject, pName),
	mIsUpdated(true),
	mValue(pValue)
{
}

FloatAttribute::~FloatAttribute()
{
}



float FloatAttribute::GetValue() const
{
	return mValue;
}

void FloatAttribute::SetValue(float pValue)
{
	if (mValue != pValue)
	{
		mValue = pValue;
		mIsUpdated = true;
		mContextObject->OnAttributeUpdated(this);
	}
}



int FloatAttribute::QuerySend() const
{
	return mIsUpdated? Parent::QuerySend()+sizeof(mValue) : 0;
}

int FloatAttribute::Pack(uint8* pDestination)
{
	const int lParentSize = Parent::Pack(pDestination);
	pDestination += lParentSize;
	PackerReal::Pack(pDestination, mValue);
	mIsUpdated = false;
	return lParentSize + sizeof(mValue);
}

int FloatAttribute::Unpack(const uint8* pSource, int pMaxSize)
{
	if (pMaxSize < sizeof(mValue))
	{
		return -1;
	}
	PackerReal::Unpack(mValue, pSource, pMaxSize);
	mIsUpdated = mContextObject->GetManager()->GetGameManager()->IsServer();
	return sizeof(mValue);
}



void FloatAttribute::operator=(const FloatAttribute&)
{
	assert(false);
}



}
