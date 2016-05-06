
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "airballoonpilot.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/contextpath.h"
#include "downwashmanager.h"
#include "level.h"

#define AIM_DISTANCE			10.0f



namespace Downwash {



typedef cure::ContextPath::SplinePath Spline;



AirBalloonPilot::AirBalloonPilot(DownwashManager* game, cure::GameObjectId balloon_id):
	Parent(game->GetResourceManager(), "AirBalloonPilot"),
	game_(game),
	balloon_id_(balloon_id),
	path_(0) {
	game->GetContext()->AddLocalObject(this);
	game->GetContext()->EnableTickCallback(this);
}

AirBalloonPilot::~AirBalloonPilot() {
	delete path_;
	path_ = 0;
}



void AirBalloonPilot::OnTick() {
	Parent::OnTick();

	cure::ContextObject* balloon = manager_->GetObject(balloon_id_, true);
	if (!balloon || !game_->GetLevel()) {
		manager_->PostKillObject(GetInstanceId());
		return;
	}
	if (!balloon->IsLoaded() || balloon->GetPhysics()->GetEngineCount() < 1) {
		return;
	}

	const vec3 _position = balloon->GetPosition();
	vec3 _closest_point;
	GetClosestPathDistance(_position, _closest_point);
	const vec3 direction_force = (_closest_point - _position) * 0.2f;
	const vec3 anti_rotation_force = balloon->GetAngularVelocity() * -50.0f;
	const vec3 force(direction_force.x+anti_rotation_force.y, direction_force.y-anti_rotation_force.x, direction_force.z);
	balloon->SetEnginePower(0, Math::Clamp(force.y, -0.1f, +0.1f));
	balloon->SetEnginePower(1, Math::Clamp(force.x, -0.1f, +0.1f));
	balloon->SetEnginePower(3, Math::Clamp(force.z, +0.4f, +1.0f));
}



void AirBalloonPilot::GetClosestPathDistance(const vec3& position, vec3& closest_point) {
	if (!path_) {
		path_ = new cure::ContextPath::SplinePath(*game_->GetLevel()->QueryPath()->GetPath("air_balloon_path"));
		path_->StartInterpolation(0);
	}
	const float current_time = path_->GetCurrentInterpolationTime();

	float nearest_distance;
	const float search_step_length = 0.06f;
	const int search_steps = 3;
	path_->FindNearestTime(search_step_length, position, nearest_distance, closest_point, search_steps);

	{
		const float wanted_distance = AIM_DISTANCE;
		float delta_time = wanted_distance * path_->GetDistanceNormal();
		if (current_time+delta_time < 0) {
			delta_time = -current_time;
		}
		path_->StepInterpolation(delta_time);
		closest_point = path_->GetValue();
	}
}



loginstance(kGameContextCpp, AirBalloonPilot);



}
