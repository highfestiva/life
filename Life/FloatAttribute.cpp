
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "FloatAttribute.h"
#include <assert.h>
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextObject.h"
#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Packer.h"



namespace Life
{



FloatAttribute::FloatAttribute(Cure::ContextObject* pContextObject, const str& pName, float pValue):
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

FloatAttribute::NetworkType FloatAttribute::GetNetworkType() const
{
	return TYPE_BOTH_BROADCAST;
}



void FloatAttribute::operator=(const FloatAttribute&)
{
	assert(false);
}



}
