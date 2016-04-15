
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Goal.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Tbc/Include/PhysicsTrigger.h"
#include "../UiCure/Include/UiCppContextObject.h"



namespace TireFire
{



Goal::Goal(Cure::ContextManager* pManager):
	Parent(pManager->GetGameManager()->GetResourceManager(), "Goal"),
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

vec3 Goal::GetPosition() const
{
	const Tbc::ChunkyBoneGeometry* lGoalGeometry = mTrigger->GetTriggerGeometry(0);
	return GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lGoalGeometry->GetBodyId());
}

void Goal::FinalizeTrigger(const Tbc::PhysicsTrigger* pTrigger)
{
	mTrigger = pTrigger;
}

void Goal::OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal)
{
	(void)pTriggerId;
	(void)pOtherObject;
	(void)pBodyId;
	(void)pNormal;
	mIsTriggered = true;
}



loginstance(GAME_CONTEXT_CPP, Goal);




}
