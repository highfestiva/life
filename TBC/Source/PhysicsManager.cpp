
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/PhysicsManager.h"



namespace TBC
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



int PhysicsManager::QueryRayCollisionAgainst(const TransformationF& pRayTransform, float pLength, BodyID pBody,
	Vector3DF* pCollisionPoints, int pMaxCollisionCount)
{
	Vector3DF lDir = pRayTransform.GetOrientation() * Vector3DF(0, 0, 1);
	return QueryRayCollisionAgainst(pRayTransform.GetPosition(), lDir, pLength, pBody, pCollisionPoints, pMaxCollisionCount);
}



}
