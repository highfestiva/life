
// Author: Jonas Byström
// Copyright (c) 2002-2012, Pixel Doctrine



#pragma once

#include "../../Tbc/Include/PhysicsManager.h"



namespace Cure
{



class ContextForceListener
{
public:
	virtual void OnForceApplied(Cure::ContextObject* pObject, Cure::ContextObject* pOtherObject,
		Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		const vec3& pForce, const vec3& pTorque,
		const vec3& pPosition, const vec3& pRelativeVelocity) = 0;
};



}
