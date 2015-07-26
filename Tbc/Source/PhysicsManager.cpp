
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/PhysicsManager.h"



namespace Tbc
{



PhysicsManager::PhysicsManager():
	mTriggerCallback(0),
	mForceFeedbackCallback(0)
{
}

PhysicsManager::~PhysicsManager()
{
}



void PhysicsManager::SetTriggerListener(TriggerListener* pTriggerCallback)
{
	mTriggerCallback = pTriggerCallback;
}

void PhysicsManager::SetForceFeedbackListener(ForceFeedbackListener* pForceFeedbackCallback)
{
	mForceFeedbackCallback = pForceFeedbackCallback;
}



int PhysicsManager::QueryRayCollisionAgainst(const xform& pRayTransform, float pLength, BodyID pBody,
	vec3* pCollisionPoints, int pMaxCollisionCount)
{
	vec3 lDir = pRayTransform.GetOrientation() * vec3(0, 0, 1);
	return QueryRayCollisionAgainst(pRayTransform.GetPosition(), lDir, pLength, pBody, pCollisionPoints, pMaxCollisionCount);
}



}
