
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "RaceTimer.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Tbc/Include/ChunkyPhysics.h"
#include "../../Tbc/Include/PhysicsTrigger.h"
#include "../RaceScore.h"



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



void RaceTimer::FinalizeTrigger(const Tbc::PhysicsTrigger* pTrigger)
{
	std::vector<int> lTriggerIndexArray;
	const Tbc::ChunkyPhysics* lPhysics = mParent->GetPhysics();
	mTriggerCount = pTrigger->GetTriggerGeometryCount();
	for (int x = 0; x < (int)mTriggerCount; ++x)
	{
		const int lBoneIndex = lPhysics->GetIndex(pTrigger->GetTriggerGeometry(x));
		deb_assert(lBoneIndex >= 0);
		lTriggerIndexArray.push_back(lBoneIndex);
	}
	const Tbc::ChunkyClass::Tag* lTag = ((CppContextObject*)mParent)->FindTag(_T("race_trigger_data"), 0, 0, &lTriggerIndexArray);
	deb_assert(lTag);
	if (lTag)
	{
	}
}

void RaceTimer::OnTick()
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

void RaceTimer::OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal)
{
	(void)pBodyId;
	(void)pPosition;
	(void)pNormal;

	Cure::ContextObject* lObject = pOtherObject;
	if (lObject->GetPhysics()->GetEngineCount() <= 0)	// Only self-movable stuff can let the games begin!
	{
		return;
	}

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
	if (!lRaceScore->IsTriggered(pTriggerId))
	{
		lRaceScore->AddTriggered(pTriggerId);
		mLog.Infof(_T("Hit %u/%u triggers in %f s."), lRaceScore->GetTriggedCount(), mTriggerCount, lRaceScore->GetTime());
	}
	else if (lRaceScore->GetTriggedCount() == mTriggerCount && pTriggerId == lRaceScore->GetStartTrigger())
	{
		if (lRaceScore->StartNewLap() <= 0)
		{
			mLog.Headlinef(_T("Congratulations - finished race in %f s!"), lRaceScore->GetTime());
			mDoneMap.insert(DoneMap::value_type(lObject->GetInstanceId(), GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrameAddSeconds(5.0)));
		}
		else
		{
			mLog.Headlinef(_T("Lap completed; time is %f s!"), lRaceScore->GetTime());
		}
	}
}



loginstance(GAME_CONTEXT_CPP, RaceTimer);




}
