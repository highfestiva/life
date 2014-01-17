
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/Vector3D.h"
#include "Life.h"



namespace Cure
{
class CppContextObject;
}
namespace TBC
{
class PhysicsManager;
}



namespace Life
{



class Explosion
{
public:
	static float CalculateForce(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength);
	static float PushObject(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength);
	static void FallApart(TBC::PhysicsManager* pPhysicsManager, Cure::CppContextObject* pObject);

private:
	static float Force(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength, bool pApplyForce);

	LOG_CLASS_DECLARE();
};



}
