
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/elevator.h"
#include "../include/contextmanager.h"
#include "../include/gamemanager.h"
#include "../include/timemanager.h"
#include "../../lepra/include/hashutil.h"
#include "../../tbc/include/physicsengine.h"
#include "../../tbc/include/physicstrigger.h"



namespace cure {



#define STARTS_MOVING_VELOCITY	0.6f
#define STOPS_MOVING_VELOCITY	0.3f
#define kElevatorActivityReset	1000




Elevator::Elevator(ContextManager* manager):
	CppContextObject(manager->GetGameManager()->GetResourceManager(), "Elevator"),
	active_trigger_(0),
	exit_delay_(2.0),
	stop_delay_(5.0),
	elevator_has_been_moving_(true),	// Set to get this party started in some cases.
	elevator_is_active_(true),	// Set to get this party started in some cases.
	engine_activity_(1) {
	manager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

Elevator::~Elevator() {
}



void Elevator::SetStopDelay(double stop_delay) {
	stop_delay_ = stop_delay;
}

vec3 Elevator::GetPosition() const {
	return GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(GetFirstBody()->GetBodyId());
}

vec3 Elevator::GetVelocity() const {
	vec3 velocity;
	GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(GetFirstBody()->GetBodyId(), velocity);
	return velocity;
}

float Elevator::GetRadius() const {
	const vec3 size = GetFirstBody()->GetShapeSize();
	return std::max(size.x, size.y) * 0.5f;
}


void Elevator::OnTick() {
	Parent::OnTick();

	const float signed_max_speed2 = GetSignedMaxSpeedSquare();
	if (::fabs(signed_max_speed2) >= STARTS_MOVING_VELOCITY && elevator_is_active_) {
		elevator_has_been_moving_ = true;
	}

	const float frame_time = GetManager()->GetGameManager()->GetTimeManager()->GetNormalFrameTime();
	engine_activity_ = Math::Lerp(engine_activity_, signed_max_speed2, Math::GetIterateLerpTime(0.4f, frame_time));
	if (::fabs(signed_max_speed2) < STARTS_MOVING_VELOCITY && ::fabs(engine_activity_) < STOPS_MOVING_VELOCITY) {
		const tbc::PhysicsTrigger* _trigger = 0;
		const int trigger_count = GetTriggerCount((const void*&)_trigger);
		const bool is_non_stop = (trigger_count == 1 && _trigger->GetType() == tbc::PhysicsTrigger::kTriggerNonStop);
		bool has_stopped = true;
		if (is_non_stop) {
			stop_timer_.TryStart();
			has_stopped = (stop_timer_.QueryTimeDiff() >= stop_delay_);
		}
		if (has_stopped) {
			stop_timer_.ClearTimeDiff();
			const bool stop_engines = !is_non_stop;
			if (elevator_has_been_moving_) {
				log_debug("TRIGGER - elevator has stopped.");
				HaltActiveEngines(stop_engines);

				// Check if we need to restart "non_stop" or "always" trigger.
				if (trigger_count == 1 && _trigger->GetType() <= tbc::PhysicsTrigger::kTriggerAlways) {
					Trig(_trigger);
				}
			}
			engine_activity_ = kElevatorActivityReset;	// Don't try again in a long time.

			// Low engine activity and no longer actively triggered means we lost the previous trigger.
			if (active_trigger_ && active_trigger_->GetType() >= tbc::PhysicsTrigger::kTriggerMovement &&
				trig_time_.QueryTimeDiff() > exit_delay_) {
				log_debug("TRIGGER - exited trigger volume.");
				// Stop the engines once again, to handle the following scenario:
				// 1. Physically trigged, engine started.
				// 2. Physical object came to end-point.
				// 3. Elevator stopped due to (2).
				// 4. Still physically triggered, engine re-started!
				HaltActiveEngines(stop_engines);
				active_trigger_ = 0;
			}
		}
	} else {
		stop_timer_.Stop();
	}
}

void Elevator::OnAlarm(int alarm_id, void* extra_data) {
	Parent::OnAlarm(alarm_id, extra_data);

	typedef tbc::PhysicsTrigger::EngineTrigger EngineTrigger;
	const EngineTrigger* engine_trigger = (const EngineTrigger*)extra_data;
	if (engine_trigger) {
		SetFunctionTarget(engine_trigger->function_, engine_trigger->engine_);
		engine_activity_ = kElevatorActivityReset;	// Don't try again in a long time.
		if (active_trigger_ && active_trigger_->GetType() >= tbc::PhysicsTrigger::kTriggerMovement) {
			// Run engine for some time before *forcing* deactivation.
			GetManager()->AddGameAlarmCallback(this, alarm_id, 60, 0);
		}
	} else {
		log_debug("TRIGGER - no longer triggered.");
		HaltActiveEngines(true);
		active_trigger_ = 0;
	}
}

void Elevator::OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	(void)other_object;
	(void)body_id;
	(void)position;
	(void)normal;

	const tbc::PhysicsTrigger* _trigger = (const tbc::PhysicsTrigger*)GetTrigger(trigger_id);
	deb_assert(_trigger);
	if (_trigger == active_trigger_) {
		trig_time_.PopTimeDiff();
		return;
	}
	if (active_trigger_ && _trigger->GetPriority() > active_trigger_->GetPriority()) {
		return;
	}
	Trig(_trigger);
}

void Elevator::Trig(const tbc::PhysicsTrigger* trigger) {
	GetManager()->CancelPendingAlarmCallbacksById(this, 0);
	active_trigger_ = trigger;
	trig_time_.PopTimeDiff();
	const int engine_count = trigger->GetControlledEngineCount();
	for (int y = 0; y < engine_count; ++y) {
		const tbc::PhysicsTrigger::EngineTrigger& engine_trigger = trigger->GetControlledEngine(y);
		log_volatile(log_.Debugf("TRIGGER - trigging function %s.", engine_trigger.function_.c_str()));
		GetManager()->AddGameAlarmCallback(this, 0, engine_trigger.delay_, (void*)&engine_trigger);
	}
}

tbc::ChunkyBoneGeometry* Elevator::GetFirstBody() const {
	const tbc::PhysicsTrigger* _trigger = 0;
	GetTriggerCount((const void*&)_trigger);
	deb_assert(_trigger && _trigger->GetControlledEngineCount() > 0);
	const tbc::PhysicsTrigger::EngineTrigger& engine_trigger = _trigger->GetControlledEngine(0);
	typedef tbc::PhysicsEngine::GeometryList BodyList;
	BodyList body_list = engine_trigger.engine_->GetControlledGeometryList();
	deb_assert(!body_list.empty());
	return body_list[0];
}



float Elevator::GetSignedMaxSpeedSquare() const {
	float max_speed2 = 0;
	float signed_max_speed2 = 0;
	if (active_trigger_) {
		const tbc::PhysicsManager* physics_manager = GetManager()->GetGameManager()->GetPhysicsManager();
		const int engine_count = active_trigger_->GetControlledEngineCount();
		for (int y = 0; y < engine_count; ++y) {
			const tbc::PhysicsTrigger::EngineTrigger& engine_trigger = active_trigger_->GetControlledEngine(y);
			vec3 velocity = engine_trigger.engine_->GetCurrentMaxSpeed(physics_manager);
			const float speed2 = velocity.GetLengthSquared();
			if (speed2 > max_speed2) {
				max_speed2 = speed2;
			}
			signed_max_speed2 = (velocity.z < 0.5f)? -max_speed2 : max_speed2;
		}
	}
	return signed_max_speed2;
}

void Elevator::HaltActiveEngines(bool stop) {
	elevator_has_been_moving_ = false;
	elevator_is_active_ = false;
	if (active_trigger_ && stop) {
		const int engine_count = active_trigger_->GetControlledEngineCount();
		for (int y = 0; y < engine_count; ++y) {
			const tbc::PhysicsTrigger::EngineTrigger& engine_trigger = active_trigger_->GetControlledEngine(y);
			engine_trigger.engine_->SetValue(engine_trigger.engine_->GetControllerIndex(), 0);
		}
		parent_->ForceSend();	// Transmit our updated engine values.
	}
}



void Elevator::SetFunctionTarget(const str& function, tbc::PhysicsEngine* engine) {
	float target_value = 0;	// Default it to stop.
	if (function == "minimum") {
		target_value = -1;
	} else if (function == "maximum") {
		target_value = 1;
	} else if (function == "toggle") {
		target_value = -engine->GetValue();
		if (Math::IsEpsEqual(target_value, 0.0f)) {
			target_value = -engine->GetValues()[tbc::PhysicsEngine::kAspectLocalShadow];	// Invert shadow if stopped.
			if (Math::IsEpsEqual(target_value, 0.0f)) {
				target_value = -1;
			}
		}
	} else {
		deb_assert(false);
	}
	log_volatile(log_.Debugf("TRIGGER - activating engine for function %s.", function.c_str()));
	elevator_is_active_ = true;
	engine->ForceSetValue(tbc::PhysicsEngine::kAspectLocalShadow, target_value);	// Store shadow.
	engine->SetValue(engine->GetControllerIndex(), target_value);
	parent_->ForceSend();	// Transmit our updated engine values.
}



loginstance(kGameContextCpp, Elevator);




}
