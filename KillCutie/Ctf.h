
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/cppcontextobject.h"
#include "../lepra/include/hirestimer.h"
#include "game.h"



namespace grenaderun {



class Ctf: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	Ctf(cure::ContextManager* manager);
	virtual ~Ctf();

	vec3 GetPosition() const;
	float GetCaptureLevel() const;
	void StartSlideDown();

private:
	virtual void FinalizeTrigger(const tbc::PhysicsTrigger* trigger);
	virtual void OnTick();
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);

	const tbc::PhysicsTrigger* trigger_;
	vec3 flag_offset_;
	vec3 flag_top_;
	vec3 catching_flag_velocity_;
	vec3 start_flag_velocity_;
	bool last_frame_triggered_;
	bool is_trigger_timer_started_;
	HiResTimer trigger_timer_;
	tbc::GeometryReference* flag_mesh_;
	tbc::GeometryReference* blink_mesh_;
	bool slide_down_;
	vec3 blink_start_color_;
	float blink_time_;

	logclass();
};



}
