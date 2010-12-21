
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "RaceScore.h"
#include <assert.h>
#include "../Lepra/Include/Packer.h"



namespace Life
{



RaceScore::RaceScore(Cure::ContextObject* pContextObject, const str& pName):
	Parent(pContextObject, pName),
	mIsUpdated(true),
	mLapCountLeft(0),
	mStartTrigger(TBC::INVALID_TRIGGER),
	mTriggedCount(0),
	mTriggerCount(2)
{
}

RaceScore::RaceScore(Cure::ContextObject* pContextObject, const str& pName, int pLapCount, TBC::PhysicsManager::TriggerID pStartTrigger):
	Parent(pContextObject, pName),
	mIsUpdated(true),
	mLapCountLeft(pLapCount),
	mStartTrigger(pStartTrigger),
	mTriggedCount(1),
	mTriggerCount(2)
{
	mTriggerSet.insert(mStartTrigger);
}

RaceScore::~RaceScore()
{
}



int RaceScore::QuerySend() const
{
	return mIsUpdated? 1+sizeof(uint32)+1+1 : 0;
}

void RaceScore::Pack(uint8* pDestination)
{
	pDestination[0] = (uint8)mLapCountLeft;
	PackerInt32::Pack(pDestination+1, -1);
	pDestination[5] = (uint8)mTriggerSet.size();
	pDestination[6] = (uint8)mTriggerCount;

	mIsUpdated = false;
}

int RaceScore::Unpack(const uint8* pSource, int pMaxSize)
{
	if (pMaxSize < 1+sizeof(uint32)+1+1)
	{
		return -1;
	}
	mLapCountLeft = pSource[0];
	int lTimeTick;
	PackerInt32::Unpack(lTimeTick, pSource+1, pMaxSize);
	mTriggedCount = pSource[5];
	mTriggerCount = pSource[6];
	mIsUpdated = true;
	return 1+sizeof(uint32)+1+1;
}



void RaceScore::operator=(const RaceScore&)
{
	assert(false);
}



}
