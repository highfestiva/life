
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "autopathdriver.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/health.h"
#include "../cure/include/runtimevariable.h"
#include "basemachine.h"
#include "level.h"

#define AIM_DISTANCE			10.0f



namespace Fire {



AutoPathDriver::AutoPathDriver(FireManager* game, cure::GameObjectId vehicle_id, const str& path_name):
	Parent(game->GetResourceManager(), "AutoPathDriver"),
	game_(game),
	vehicle_id_(vehicle_id),
	path_name_(path_name),
	path_(0) {
	game->GetContext()->AddLocalObject(this);
	game->GetContext()->EnableTickCallback(this);
}

AutoPathDriver::~AutoPathDriver() {
	delete path_;
	path_ = 0;
}


void AutoPathDriver::OnTick() {
	Parent::OnTick();

	BaseMachine* vehicle = (BaseMachine*)manager_->GetObject(vehicle_id_, true);
	if (!vehicle) {
		manager_->PostKillObject(GetInstanceId());
		return;
	}
	if (!vehicle->IsLoaded() || vehicle->GetPhysics()->GetEngineCount() < 3) {
		return;
	}

	const vec3 vehicle_direction3d = vehicle->GetOrientation()*vec3(0,1,0);
	const vec3 _position = vehicle->GetPosition();
	vec3 _closest_point;
	GetClosestPathDistance(_position, _closest_point, AIM_DISTANCE);
	vec3 direction(_closest_point - _position);
	if (path_) {
		direction = direction.GetNormalized() + path_->GetSlope();
	}

	float engine_power = Math::Lerp(0.4f, 1.0f, vehicle->panic_level_) * vehicle->level_speed_;
	const vec2 wanted_direction(direction.x, direction.y);
	const vec2 vehicle_direction(vehicle_direction3d.x, vehicle_direction3d.y);
	const float steering_angle = wanted_direction.GetAngle(vehicle_direction);
	engine_power *= Math::Lerp(1.0f, 0.6f, std::min(1.0f, std::abs(steering_angle)));
	vehicle->SetEnginePower(0, engine_power);
	vehicle->SetEnginePower(1, steering_angle);
	const float low_limit = 10.0f;
	const float panic_limit = 30.0f;
	const float brake_limit = Math::Lerp(low_limit, panic_limit, vehicle->panic_level_);
	const float speed = vehicle->GetVelocity().GetLength();
	float brake_power = 0;
	if (speed > brake_limit/2) {
		float velocity_brake_factor = Math::Clamp(speed, 0.0f, brake_limit) / brake_limit;
		brake_power += std::max(0.0f, (std::abs(steering_angle)-0.5f)*0.5f*velocity_brake_factor);
	}
	if (steering_angle > PIF/6 && speed > low_limit / 3) {
		float velocity_brake_factor = Math::Clamp(speed, 0.0f, low_limit/2) / (low_limit/2);
		brake_power += velocity_brake_factor;
	}
	brake_power = Math::Clamp(brake_power, 0.0f, 0.3f);
	vehicle->SetEnginePower(2, brake_power);

	if (vehicle->GetVelocity().GetLengthSquared() < 1.0f) {
		still_timer_.TryStart();
		if (still_timer_.QueryTimeDiff() > 4.0f) {
			cure::Health::Set(vehicle, 0);
		}
	} else {
		still_timer_.Stop();
	}
}

void AutoPathDriver::GetClosestPathDistance(const vec3& position, vec3& closest_point, float wanted_distance) {
	if (!path_) {
		if (!game_->GetLevel() || !game_->GetLevel()->QueryPath()->GetPath(path_name_)) {
			manager_->PostKillObject(GetInstanceId());
			return;
		}
		path_ = new cure::ContextPath::SplinePath(*game_->GetLevel()->QueryPath()->GetPath(path_name_));
		path_->StartInterpolation(0);
	}

	float nearest_distance;
	const float search_step_length = 0.06f;
	const int search_steps = 3;
	path_->FindNearestTime(search_step_length, position, nearest_distance, closest_point, search_steps);

	{
		float current_time = path_->GetCurrentInterpolationTime();
		float delta_time = wanted_distance * path_->GetDistanceNormal();
		if (current_time+delta_time < 0.1f) {
			delta_time = 0.1f-current_time;
		}
		path_->StepInterpolation(delta_time);
		closest_point = path_->GetValue();
	}
}



loginstance(kGameContextCpp, AutoPathDriver);



}
