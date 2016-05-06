
// Author: Jonas Byström
// Copyright (c) 2002-2012, Pixel Doctrine



#pragma once

#include "../../tbc/include/physicsmanager.h"



namespace cure {



class ContextForceListener {
public:
	virtual void OnForceApplied(cure::ContextObject* object, cure::ContextObject* other_object,
		tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque,
		const vec3& position, const vec3& relative_velocity) = 0;
};



}
