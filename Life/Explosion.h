
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/vector3d.h"
#include "life.h"



namespace cure {
class CppContextObject;
}
namespace tbc {
class PhysicsManager;
}



namespace life {



class Explosion {
public:
	static float CalculateForce(tbc::PhysicsManager* physics_manager, const cure::ContextObject* object, const vec3& position, float strength);
	static float PushObject(tbc::PhysicsManager* physics_manager, const cure::ContextObject* object, const vec3& position, float strength, float time_factor);
	static void FallApart(tbc::PhysicsManager* physics_manager, cure::CppContextObject* object, bool include_fixed);
	static void Freeze(tbc::PhysicsManager* physics_manager, const cure::ContextObject* object);

private:
	static float Force(tbc::PhysicsManager* physics_manager, const cure::ContextObject* object, const vec3& position, float strength, float time_factor);

	logclass();
};



}
