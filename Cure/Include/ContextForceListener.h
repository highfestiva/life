
// Author: Jonas Byström
// Copyright (c) 2002-2012, Righteous Games



#pragma once

#include "../../TBC/Include/PhysicsManager.h"



namespace Cure
{



class ContextForceListener
{
public:
	virtual void OnForceApplied(Cure::ContextObject* pObject, Cure::ContextObject* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity) = 0;
};



}
