
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "bombplane.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/health.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/random.h"
#include "../../life/launcher.h"
#include "../../life/projectileutil.h"

#define BOMBING_RADIUS 80.0f



namespace HoverTank {



BombPlane::BombPlane(cure::ResourceManager* resource_manager, const str& class_id, life::Launcher* launcher, const vec3& target):
	Parent(resource_manager, class_id),
	launcher_(launcher),
	target_(target),
	last_bomb_tick_(0),
	is_detonated_(false) {
	cure::Health::Set(this, 0.1f);	// Partially excempted in explosion logic, so definitely higher in practice.

	// Randomize so bombers won't have perfect (robotic) synchronization.
	bombing_radius_squared_ = BOMBING_RADIUS*BOMBING_RADIUS * Random::Uniform(0.6f, 1.7f);
	drop_interval_ = Random::Uniform(0.5f, 0.8f);
}

BombPlane::~BombPlane() {
}



void BombPlane::OnLoaded() {
	SetEnginePower(0, 1.0f);	// Jet.
	SetEnginePower(4, 1.0f);	// Hoover.

	Parent::OnLoaded();
}

void BombPlane::OnTick() {
	const float health = cure::Health::Get(this);
	if (health <= 0) {
		life::ProjectileUtil::Detonate(this, &is_detonated_, launcher_, GetPosition(), GetVelocity(), vec3(), 1, 5);
	}

	const cure::TimeManager* time_manager = GetManager()->GetGameManager()->GetTimeManager();
	if (time_manager->ConvertPhysicsFramesToSeconds(time_manager->GetCurrentPhysicsFrameDelta(last_bomb_tick_)) < drop_interval_) {
		return;
	}

	const vec3 position = GetPosition();
	const vec3 velocity = GetVelocity();

	// g*t^2/2 - v0*t + h = 0
	const float g2 = +9.82f/2;	// Positive.
	const float v0 = 0;//velocity.z;
	const float h = target_.z - position.z;	// Negative.
	float t1;
	float t2;
	if (Math::CalculateRoot(g2, v0, h, t1, t2)) {
		vec3 horizontal_projected_target(target_.x, target_.y, position.z);
		const float d = horizontal_projected_target.GetDistanceSquared(position + velocity*t1);
		if (d < bombing_radius_squared_) {
			last_bomb_tick_ = time_manager->GetCurrentPhysicsFrame();
			launcher_->Shoot(this, -10);
		}
	}
}



loginstance(kGameContextCpp, BombPlane);



}
