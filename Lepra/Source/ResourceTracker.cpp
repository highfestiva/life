
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/LepraAssert.h"
#include "../Include/ResourceTracker.h"



namespace Lepra
{



ResourceTracker::ResourceTracker():
	mLock(new Lock())
{
}

void ResourceTracker::Add(const HashedString& pResourceName, int pValue)
{
	ScopeLock lLock(mLock);
	CounterMap::iterator x = mCounterMap.find(pResourceName);
	if (x == mCounterMap.end())
	{
		mCounterMap.insert(CounterMap::value_type(pResourceName, pValue));
	}
	else
	{
		x->second += pValue;
	}
}

ResourceTracker::CounterMap ResourceTracker::GetAll() const
{
	ScopeLock lLock(mLock);
	return mCounterMap;
}



ResourceTracker gResourceTracker;



}
