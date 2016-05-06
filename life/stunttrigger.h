
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/cppcontextobject.h"
#include "../lepra/include/hirestimer.h"
#include "life.h"



namespace life {



class StuntTrigger: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	StuntTrigger(cure::ContextManager* manager, const str& class_id);
	virtual ~StuntTrigger();

protected:
	virtual void FinalizeTrigger(const tbc::PhysicsTrigger* trigger);
	virtual void OnTick();
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);
	virtual void DidTrigger(cure::ContextObject* body) = 0;

	bool allow_bullet_time_;

	bool last_frame_triggered_;
	StopWatch trigger_timer_;

	float min_speed_;
	float max_speed_;
	float min_time_;
	float real_time_ratio_;
	float bullet_time_duration_;
	str client_start_command_;
	str client_stop_command_;

	logclass();
};



}
