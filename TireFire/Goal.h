
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/cppcontextobject.h"
#include "../lepra/include/hirestimer.h"
#include "game.h"



namespace tirefire {



class Goal: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	Goal(cure::ContextManager* manager);
	virtual ~Goal();

	vec3 GetPosition() const;
	bool IsTriggered() const;

private:
	virtual void FinalizeTrigger(const tbc::PhysicsTrigger* trigger);
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);

	const tbc::PhysicsTrigger* trigger_;
	bool is_triggered_;

	logclass();
};



}
