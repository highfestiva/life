
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Push.h"



namespace Push
{



class ServerLauncher
{
public:
	virtual void GetBarrel(Cure::GameObjectId pInstanceId, TransformationF& pTransform, Vector3DF& pVelocity) const = 0;

	virtual void Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pExplosive, Cure::ContextObject* pHitObject,
		TBC::PhysicsManager::BodyID pExplosiveBodyId, TBC::PhysicsManager::BodyID pHitBodyId) = 0;
};



}
