
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
	static float PushObject(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength, float pTimeFactor);
	static void FallApart(TBC::PhysicsManager* pPhysicsManager, Cure::CppContextObject* pObject);
	static void Freeze(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject);

private:
	static float Force(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength, float pTimeFactor);

	LOG_CLASS_DECLARE();
};



}
