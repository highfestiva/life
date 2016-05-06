
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../cure/include/cppcontextobject.h"
#include "../life.h"



namespace life {



class Launcher;



class ServerFastProjectile: public cure::CppContextObject {
public:
	typedef cure::CppContextObject Parent;

	ServerFastProjectile(cure::ResourceManager* resource_manager, const str& class_id, Launcher* launcher);
	virtual ~ServerFastProjectile();

private:
	virtual void OnLoaded();
	virtual void OnMicroTick(float frame_time);
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* body, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);

	Launcher* launcher_;
	float max_velocity_;
	float acceleration_;
	float explosive_energy_;
	bool is_detonated_;

	logclass();
};



}
