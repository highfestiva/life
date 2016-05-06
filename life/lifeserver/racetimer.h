
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../cure/include/cppcontextobject.h"
#include "../../lepra/include/unordered.h"
#include "../../lepra/include/timer.h"
#include "../life.h"



namespace life {



class RaceTimer: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	RaceTimer(cure::ContextManager* manager);
	virtual ~RaceTimer();

private:
	virtual void FinalizeTrigger(const tbc::PhysicsTrigger* trigger);
	virtual void OnTick();
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);

	typedef std::unordered_map<cure::GameObjectId, int> DoneMap;

	str attribute_name_;
	unsigned trigger_count_;
	DoneMap done_map_;

	logclass();
};



}
