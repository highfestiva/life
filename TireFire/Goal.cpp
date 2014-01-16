
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "Goal.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../TBC/Include/PhysicsTrigger.h"
#include "../UiCure/Include/UiCppContextObject.h"



namespace TireFire
{



Goal::Goal(Cure::ContextManager* pManager):
	Parent(pManager->GetGameManager()->GetResourceManager(), _T("Goal")),
	mTrigger(0),
	mIsTriggered(false)
{
	pManager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

Goal::~Goal()
{
}



bool Goal::IsTriggered() const
{
	return mIsTriggered;
}

Vector3DF Goal::GetPosition() const
{
	const TBC::ChunkyBoneGeometry* lGoalGeometry = mTrigger->GetTriggerGeometry(0);
	return GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lGoalGeometry->GetTriggerId());
}

void Goal::FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger)
{
	mTrigger = pTrigger;
}

void Goal::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, TBC::PhysicsManager::BodyID pBodyId, const Vector3DF& pNormal)
{
	(void)pTriggerId;
	(void)pOtherObject;
	(void)pBodyId;
	(void)pNormal;
	mIsTriggered = true;
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Goal);




}
