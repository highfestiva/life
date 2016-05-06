
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uicppcontextobject.h"
#include "../../cure/include/contextforcelistener.h"
#include "../../lepra/include/hirestimer.h"
#include "../../uitbc/include/uichunkyclass.h"



namespace UiCure {



class GravelEmitter: public cure::ContextForceListener {
public:
	GravelEmitter(cure::ResourceManager* resource_manager, GameUiManager* ui_manager, float sensitivity, float scale, float amount, float life_time);
	virtual ~GravelEmitter();

	virtual void OnForceApplied(cure::ContextObject* object, cure::ContextObject* other_object,
		tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque,
		const vec3& position, const vec3& relative_velocity);

	cure::ResourceManager* resource_manager_;
	GameUiManager* ui_manager_;
	HiResTimer particle_timer_;
	float sensitivity_factor_;
	float scale_;
	float delay_;
	float life_time_;

	logclass();
};



}
