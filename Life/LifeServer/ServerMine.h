
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../cure/include/cppcontextobject.h"
#include "../life.h"



namespace life {



class Launcher;



class ServerMine: public cure::CppContextObject {
public:
	typedef cure::CppContextObject Parent;

	ServerMine(cure::ResourceManager* resource_manager, const str& class_id, Launcher* launcher);
	virtual ~ServerMine();

private:
	virtual void OnTick();
	virtual void OnForceApplied(cure::ContextObject* other_object,
		tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque,
		const vec3& position, const vec3& relative_velocity);

	Launcher* launcher_;
	int ticks_til_fully_activated_;
	int ticks_til_detonation_;
	bool is_detonated_;

	logclass();
};



}
