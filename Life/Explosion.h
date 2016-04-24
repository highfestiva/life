
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/Vector3D.h"
#include "Life.h"



namespace Cure
{
class CppContextObject;
}
namespace Tbc
{
class PhysicsManager;
}



namespace Life
{



class Explosion
{
public:
	static float CalculateForce(Tbc::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const vec3& pPosition, float pStrength);
	static float PushObject(Tbc::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const vec3& pPosition, float pStrength, float pTimeFactor);
	static void FallApart(Tbc::PhysicsManager* pPhysicsManager, Cure::CppContextObject* pObject, bool pIncludeFixed);
	static void Freeze(Tbc::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject);

private:
	static float Force(Tbc::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const vec3& pPosition, float pStrength, float pTimeFactor);

	logclass();
};



}
