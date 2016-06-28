
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "serverfastprojectile.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../launcher.h"
#include "../projectileutil.h"



namespace life {



ServerFastProjectile::ServerFastProjectile(cure::ResourceManager* resource_manager, const str& class_id, Launcher* launcher):
	Parent(resource_manager, class_id),
	launcher_(launcher),
	max_velocity_(0),
	acceleration_(0),
	explosive_energy_(0),
	is_detonated_(false) {
}

ServerFastProjectile::~ServerFastProjectile() {
}



void ServerFastProjectile::OnLoaded() {
	Parent::OnLoaded();

	const tbc::ChunkyClass::Tag* tag = FindTag("ammo", 4, 2);
	deb_assert(tag);
	const float muzzle_velocity = tag->float_value_list_[0];
	ProjectileUtil::StartBullet(this, muzzle_velocity, true);
	max_velocity_ = tag->float_value_list_[1];
	acceleration_ = tag->float_value_list_[2];
	explosive_energy_ = tag->float_value_list_[3];
}

void ServerFastProjectile::OnMicroTick(float frame_time) {
	Parent::OnMicroTick(frame_time);
	ProjectileUtil::BulletMicroTick(this, frame_time, max_velocity_, acceleration_);
}

void ServerFastProjectile::OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	(void)trigger_id;
	(void)body_id;
	(void)position;
	if (explosive_energy_) {
		ProjectileUtil::Detonate(this, &is_detonated_, launcher_, GetPosition(), GetVelocity(), normal, explosive_energy_, 0);
	} else {
		ProjectileUtil::OnBulletHit(this, &is_detonated_, launcher_, other_object);
	}
}



loginstance(kGameContextCpp, ServerFastProjectile);



}
