
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/PhysicsManager.h"



namespace TBC
{



PhysicsManager::PhysicsManager()
{
}

PhysicsManager::~PhysicsManager()
{
}



int PhysicsManager::QueryRayCollisionAgainst(const TransformationF& pRayTransform, float pLength, BodyID pBody,
	Vector3DF* pCollisionPoints, int pMaxCollisionCount)
{
	Vector3DF lDir = pRayTransform.GetOrientation() * Vector3DF(0, 0, 1);
	return QueryRayCollisionAgainst(pRayTransform.GetPosition(), lDir, pLength, pBody, pCollisionPoints, pMaxCollisionCount);
}



}
