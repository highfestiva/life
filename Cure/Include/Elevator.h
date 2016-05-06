
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/hirestimer.h"
#include "cppcontextobject.h"
#include "cure.h"



namespace cure {



class Elevator: public CppContextObject {
	typedef CppContextObject Parent;
public:
	Elevator(ContextManager* manager);
	virtual ~Elevator();

	void SetStopDelay(double stop_delay);
	vec3 GetPosition() const;
	vec3 GetVelocity() const;
	float GetRadius() const;

protected:
	virtual void OnTick();
	virtual void OnAlarm(int alarm_id, void* extra_data);
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);
	void Trig(const tbc::PhysicsTrigger* trigger);
	tbc::ChunkyBoneGeometry* GetFirstBody() const;

	float GetSignedMaxSpeedSquare() const;
	void HaltActiveEngines(bool stop);

	void SetFunctionTarget(const str& function, tbc::PhysicsEngine* engine);

private:
	const tbc::PhysicsTrigger* active_trigger_;
	HiResTimer trig_time_;
	StopWatch stop_timer_;
	double exit_delay_;
	double stop_delay_;
	bool elevator_has_been_moving_;
	bool elevator_is_active_;
	float engine_activity_;

	logclass();
};



}
