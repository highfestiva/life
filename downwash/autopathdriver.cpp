
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "autopathdriver.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/health.h"
#include "level.h"

#define AIM_DISTANCE			3.0f
#define REAR_WHEEL_STEERING_DISTANCE	5.0f



namespace Downwash {



AutoPathDriver::AutoPathDriver(DownwashManager* game, cure::GameObjectId vehicle_id, const str& path_name):
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

	cure::ContextObject* vehicle = manager_->GetObject(vehicle_id_, true);
	if (!vehicle) {
		manager_->PostKillObject(GetInstanceId());
		return;
	}
	if (!vehicle->IsLoaded() || vehicle->GetPhysics()->GetEngineCount() < 3) {
		return;
	}

	const vec3 vehicle_direction3d = vehicle->GetOrientation()*vec3(0,1,0);
	const vec3 _position = vehicle->GetPosition() + vehicle_direction3d * REAR_WHEEL_STEERING_DISTANCE;
	vec3 _closest_point;
	GetClosestPathDistance(_position, _closest_point, AIM_DISTANCE);
	const vec3 direction(_closest_point - _position);

	vehicle->SetEnginePower(0, 1);
	const vec2 wanted_direction(direction.x, direction.y);
	//wanted_direction.Normalize();
	const vec2 vehicle_direction(vehicle_direction3d.x, vehicle_direction3d.y);
	//vehicle_direction.Normalize();
	const float steering_angle = wanted_direction.GetAngle(vehicle_direction);
	vehicle->SetEnginePower(1, steering_angle);

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

	const float current_time = path_->GetCurrentInterpolationTime();

	float nearest_distance;
	const float search_step_length = 0.06f;
	const int search_steps = 3;
	path_->FindNearestTime(search_step_length, position, nearest_distance, closest_point, search_steps);

	{
		float delta_time = wanted_distance * path_->GetDistanceNormal();
		if (current_time+delta_time < 0) {
			delta_time = -current_time;
		}
		path_->StepInterpolation(delta_time);
		closest_point = path_->GetValue();
	}
}



loginstance(kGameContextCpp, AutoPathDriver);



}
