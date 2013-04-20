
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Lepra/Include/Vector3D.h"
#include "Push.h"



namespace Cure
{
class ContextObject;
}
namespace TBC
{
class PhysicsManager;
}



namespace Push
{



class Explosion
{
public:
	static float CalculateForce(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength);
	static float PushObject(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength);
	static void FallApart(TBC::PhysicsManager* pPhysicsManager, Cure::ContextObject* pObject);

	LOG_CLASS_DECLARE();
};



}
