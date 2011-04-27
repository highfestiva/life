
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "RaceTimer.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsTrigger.h"
#include "../RaceScore.h"
#include "GameServerManager.h"



namespace Life
{



RaceTimer::RaceTimer(Cure::ContextManager* pManager):
	Cure::CppContextObject(pManager->GetGameManager()->GetResourceManager(), _T("RaceTimer")),
	mTriggerCount(0)
{
	pManager->AddLocalObject(this);
	mAttributeName = strutil::Format(_T("race_timer_%u"), GetInstanceId());
	GetManager()->EnableTickCallback(this);
}

RaceTimer::~RaceTimer()
{
}



void RaceTimer::FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger)
{
	std::vector<int> lTriggerIndexArray;
	const TBC::ChunkyPhysics* lPhysics = mParent->GetPhysics();
	mTriggerCount = pTrigger->GetTriggerGeometryCount();
	for (int x = 0; x < (int)mTriggerCount; ++x)
	{
		const int lBoneIndex = lPhysics->GetIndex(pTrigger->GetTriggerGeometry(x));
		assert(lBoneIndex >= 0);
		lTriggerIndexArray.push_back(lBoneIndex);
	}
	const TBC::ChunkyClass::Tag* lTag = ((CppContextObject*)mParent)->FindTag(_T("race_trigger_data"), 0, 0, lTriggerIndexArray);
	assert(lTag);
	if (lTag)
	{
	}
}

void RaceTimer::OnTick(float)
{
	const Cure::TimeManager* lTimeManager = GetManager()->GetGameManager()->GetTimeManager();
	DoneMap::iterator x = mDoneMap.begin();
	while (x != mDoneMap.end())
	{
		if (lTimeManager->GetCurrentPhysicsFrameDelta(x->second) >= 0)
		{
			Cure::ContextObject* lObject = GetManager()->GetObject(x->first);
			if (lObject)
			{
				lObject->DeleteAttribute(mAttributeName);
			}
			mDoneMap.erase(x++);
		}
		else
		{
			++x;
		}
	}
}

void RaceTimer::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pListener)
{
	Cure::ContextObject* lObject = (Cure::ContextObject*)pListener;

	// Check if just finished this race.
	DoneMap::iterator i = mDoneMap.find(lObject->GetInstanceId());
	if (i != mDoneMap.end())
	{
		return;
	}

	RaceScore* lRaceScore = (RaceScore*)lObject->GetAttribute(mAttributeName);
	if (!lRaceScore)
	{
		lRaceScore = new RaceScore(lObject, mAttributeName, 1, pTriggerId);
		mLog.AHeadline("Race started!");
	}
	RaceScore::TriggerSet::iterator x = lRaceScore->mTriggerSet.find(pTriggerId);
	if (x == lRaceScore->mTriggerSet.end())
	{
		lRaceScore->mTriggerSet.insert(pTriggerId);
		mLog.Infof(_T("Hit %u/%u triggers in %f s."), lRaceScore->mTriggerSet.size(), mTriggerCount, lRaceScore->mTimer.QueryTimeDiff());
		
	}
	else if (lRaceScore->mTriggerSet.size() == mTriggerCount && pTriggerId == lRaceScore->mStartTrigger)
	{
		if (--lRaceScore->mLapCountLeft <= 0)
		{
			mLog.Headlinef(_T("Congratulations - finished race in %f s!"), lRaceScore->mTimer.QueryTimeDiff());
			mDoneMap.insert(DoneMap::value_type(lObject->GetInstanceId(), GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrameAddSeconds(5.0)));
		}
		else
		{
			mLog.Headlinef(_T("Lap completed; time is %f s!"), lRaceScore->mTimer.QueryTimeDiff());
			lRaceScore->mTriggerSet.clear();
			lRaceScore->mTriggerSet.insert(pTriggerId);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, RaceTimer);




}