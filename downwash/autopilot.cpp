
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "autopilot.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/health.h"
#include "../cure/include/runtimevariable.h"
#include "downwashmanager.h"
#include "level.h"

#define AIM_DISTANCE			2.0f
#define AHEAD_TIME			0.5f



namespace Downwash {



Autopilot::Autopilot(DownwashManager* game):
	Parent(game->GetResourceManager(), "Autopilot"),
	game_(game),
	closest_path_distance_(5.0f),
	path_(0) {
	game_->GetContext()->AddLocalObject(this);
}

Autopilot::~Autopilot() {
	delete path_;
	path_ = 0;
}



void Autopilot::Reset() {
	Level* level = game_->GetLevel();
	if (!level || level->QueryPath()->GetPathCount() < 1) {
		return;
	}
	delete path_;
	path_ = new cure::ContextPath::SplinePath(*game_->GetLevel()->QueryPath()->GetPath("player_path"));
	path_->StartInterpolation(0);
	stalled_rotor_timer_.Stop();
}

vec3 Autopilot::GetSteering() {
	Level* level = game_->GetLevel();
	if (!level || level->QueryPath()->GetPathCount() < 1) {
		return vec3();
	}
	cure::ContextObject* _chopper = game_->GetAvatar();
	if (!_chopper || !_chopper->IsLoaded() || _chopper->GetPhysics()->GetEngineCount() < 3) {
		path_->GotoAbsoluteTime(0);
		return vec3();
	}

	CheckStalledRotor(_chopper);

	last_avatar_position_ = _chopper->GetPosition();
	const vec3 velocity = _chopper->GetVelocity();
	const vec3 up = _chopper->GetOrientation() * vec3(0,0,1);
	const vec3 towards = last_avatar_position_ + velocity*AHEAD_TIME;

	closest_path_distance_ = GetClosestPathDistance(towards, closest_path_position_);
	vec3 aim = closest_path_position_ - towards;
	vec3 aim_near(0, 0, ::std::max(aim.z, 0.0f));
	const bool going_wrong_way = (velocity*aim <= 0);
	const float speed_limit = (going_wrong_way || path_->GetDistanceLeft() < 20.0f) ? 4.0f : 60.0f;
	if (!going_wrong_way) {
		aim_near.x = aim.x;
	}
	aim = Math::Lerp(aim, aim_near-velocity, std::min(1.0f, velocity.GetLength()/speed_limit));

	// Brake before upcoming drops.
	if (closest_path_distance_ < 20) {
		const float time = path_->GetCurrentInterpolationTime();
		GetClosestPathDistance(last_avatar_position_ + velocity*AHEAD_TIME*20, closest_path_position_);
		const vec3 upcoming_slope = path_->GetSlope().GetNormalized();
		path_->GotoAbsoluteTime(time);
		aim.x += Math::Lerp(-15.0f, -50.0f, ::fabs(upcoming_slope.z)) * up.x;
	}
	// End braking before drops.

	aim.x = Math::Clamp(aim.x, -0.9f, +0.9f);
	aim.z = Math::Clamp(aim.z, -0.0f, +1.0f);
	aim.z = Math::Lerp(0.05f, 0.9f, aim.z);
	return aim;
}

void Autopilot::AttemptCloserPathDistance() {
	float start_time = path_->GetCurrentInterpolationTime();
	float shortest_time = start_time;
	float shortest_distance = path_->GetValue().GetDistanceSquared(last_avatar_position_);
	for (float x = 0; x < 1; x += 0.05f) {
		const float time = fmod(start_time + x, 1.0f);
		path_->GotoAbsoluteTime(time);
		const float distance = path_->GetValue().GetDistanceSquared(last_avatar_position_);
		if (distance < shortest_distance) {
			shortest_time = time;
			shortest_distance = distance;
		}
	}
	path_->GotoAbsoluteTime(shortest_time);
}

float Autopilot::GetClosestPathDistance() const {
	return closest_path_distance_;
}

vec3 Autopilot::GetClosestPathVector() const {
	return closest_path_position_-last_avatar_position_;
}

vec3 Autopilot::GetLastAvatarPosition() const {
	return last_avatar_position_;
}

float Autopilot::GetRotorSpeed(const cure::ContextObject* chopper) const {
	if (chopper->GetPhysics()->GetEngineCount() < 3) {
		return 0;
	}
	const int rotor_index = chopper->GetPhysics()->GetChildIndex(0, 0);
	tbc::ChunkyBoneGeometry* bone = chopper->GetPhysics()->GetBoneGeometry(rotor_index);
	vec3 rotor_speed;
	game_->GetPhysicsManager()->GetBodyAngularVelocity(bone->GetBodyId(), rotor_speed);
	return rotor_speed.GetLength();
}

void Autopilot::CheckStalledRotor(cure::ContextObject* chopper) {
	const float rotor_speed = GetRotorSpeed(chopper);
	if (rotor_speed < 6.0f*lepra::GameTimer::GetRealTimeRatio()) {
		stalled_rotor_timer_.TryStart();
		if (stalled_rotor_timer_.QueryTimeDiff() > 3.0f) {
			cure::Health::Add(chopper, -0.02f, true);
		}
	} else {
		stalled_rotor_timer_.Stop();
	}
}

float Autopilot::GetClosestPathDistance(const vec3& position, vec3& closest_point) const {
	const float current_time = path_->GetCurrentInterpolationTime();

	float nearest_distance;
	const float search_step_length = 0.06f;
	const int search_steps = 3;
	path_->FindNearestTime(search_step_length, position, nearest_distance, closest_point, search_steps);

	{
		const float wanted_distance = AIM_DISTANCE;
		float delta_time = wanted_distance * path_->GetDistanceNormal();
		if (current_time+delta_time < 0.965f) {	// Only move forward if we stay within the curve; otherwise we would loop.
			if (current_time+delta_time < 0) {
				delta_time = -current_time;
			}
			path_->StepInterpolation(delta_time);
			closest_point = path_->GetValue();
		}
	}

	return nearest_distance;
}



loginstance(kGameContextCpp, Autopilot);



}
