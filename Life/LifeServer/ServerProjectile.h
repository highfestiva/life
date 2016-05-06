
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../cure/include/cppcontextobject.h"
#include "../life.h"



namespace life {



class Launcher;



class ServerProjectile: public cure::CppContextObject {
public:
	typedef cure::CppContextObject Parent;

	ServerProjectile(cure::ResourceManager* resource_manager, const str& class_id, float muzzle_velocity, Launcher* launcher);
	virtual ~ServerProjectile();

private:
	virtual void OnLoaded();
	virtual void OnForceApplied(cure::ContextObject* other_object,
		tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque,
		const vec3& position, const vec3& relative_velocity);

	float muzzle_velocity_;
	Launcher* launcher_;
	bool is_detonated_;
	float explosive_energy_;

	logclass();
};



}
