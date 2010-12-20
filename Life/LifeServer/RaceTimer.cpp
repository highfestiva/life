
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "RaceTimer.h"
#include <algorithm>
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsTrigger.h"
#include "../LifeClient/RtVar.h"
#include "GameServerManager.h"



namespace Life
{



RaceTimer::RaceTimer(Cure::ContextManager* pManager):
	Cure::CppContextObject(pManager->GetGameManager()->GetResourceManager(), _T("RaceTimer"))
{
	pManager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

RaceTimer::~RaceTimer()
{
}



void RaceTimer::FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger)
{
	std::vector<int> lTriggerIndexArray;
	const TBC::ChunkyPhysics* lPhysics = mParent->GetPhysics();
	const int lBoneCount = pTrigger->GetTriggerGeometryCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		const int lBoneIndex = lPhysics->GetIndex(pTrigger->GetTriggerGeometry(x));
		assert(lBoneIndex >= 0);
		lTriggerIndexArray.push_back(lBoneIndex);
	}
	const TBC::ChunkyClass::Tag* lTag = ((CppContextObject*)mParent)->FindTag(_T("race_trigger_data"), 0, 0, lTriggerIndexArray);
	assert(lTag);
	if (lTag)
	{
		// TODO: ...
	}
}

void RaceTimer::OnTick(float)
{
	mTriggerTimer.UpdateTimer();
}

void RaceTimer::OnTrigger(TBC::PhysicsManager::TriggerID /*pTriggerId*/, TBC::PhysicsManager::ForceFeedbackListener* /*pListener*/)
{
	mLog.Headlinef(_T("Should start race timer..."));
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, RaceTimer);




}
