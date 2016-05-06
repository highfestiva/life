
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"
#include "../cure/include/contextforcelistener.h"
#include "game.h"



namespace cure {
class ContextPath;
}



namespace grenaderun {



class Level: public UiCure::Machine {
public:
	typedef UiCure::Machine Parent;

	Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, cure::ContextForceListener* gravel_emitter);
	virtual ~Level();

	cure::ContextPath* QueryPath();

private:
	virtual void OnForceApplied(cure::ContextObject* other_object,
		tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque,
		const vec3& position, const vec3& relative_velocity);

	cure::ContextPath* path_;
	cure::ContextForceListener* gravel_emitter_;

	logclass();
};



}
