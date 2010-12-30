
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "RaceScore.h"
#include <assert.h>
#include "../Cure/Include/ContextObject.h"
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
	AddTriggered(mStartTrigger);
}

RaceScore::~RaceScore()
{
}



RaceScore::TriggerId RaceScore::GetStartTrigger() const
{
	return mStartTrigger;
}

bool RaceScore::IsTriggered(TriggerId pTriggerId) const
{
	TriggerSet::const_iterator x = mTriggerSet.find(pTriggerId);
	return (x != mTriggerSet.end());
}

void RaceScore::AddTriggered(TriggerId pTriggerId)
{
	mTriggerSet.insert(pTriggerId);
	mIsUpdated = true;
	mContextObject->OnAttributeUpdated(this);
}

unsigned RaceScore::GetTriggedCount() const
{
	return mTriggerSet.size();
}

int RaceScore::StartNewLap()
{
	if (--mLapCountLeft > 0)
	{
		mTriggerSet.clear();
		mTriggerSet.insert(mStartTrigger);
		mIsUpdated = true;
		mContextObject->OnAttributeUpdated(this);
	}
	return mLapCountLeft;
}

double RaceScore::GetTime()
{
	return mTimer.QueryTimeDiff();
}



int RaceScore::QuerySend() const
{
	return mIsUpdated? Parent::QuerySend()+1+sizeof(uint16)+1+1 : 0;
}

int RaceScore::Pack(uint8* pDestination)
{
	const int lParentSize = Parent::Pack(pDestination);
	pDestination += lParentSize;
	pDestination[0] = (uint8)mLapCountLeft;
	PackerInt16::Pack(pDestination+1, -1);
	pDestination[3] = (uint8)mTriggerSet.size();
	pDestination[4] = (uint8)mTriggerCount;

	mIsUpdated = false;

	return lParentSize + 5;
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

RaceScore::NetworkType RaceScore::GetNetworkType() const
{
	return TYPE_SERVER;
}



void RaceScore::operator=(const RaceScore&)
{
	assert(false);
}



}
