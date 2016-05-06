
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "serverprojectile.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../launcher.h"
#include "../projectileutil.h"



namespace life {



ServerProjectile::ServerProjectile(cure::ResourceManager* resource_manager, const str& class_id, float muzzle_velocity, Launcher* launcher):
	Parent(resource_manager, class_id),
	muzzle_velocity_(muzzle_velocity),
	launcher_(launcher),
	is_detonated_(false),
	explosive_energy_(1) {
}

ServerProjectile::~ServerProjectile() {
}



void ServerProjectile::OnLoaded() {
	Parent::OnLoaded();

	const tbc::ChunkyClass::Tag* tag = FindTag("ammo", 4, -1);
	deb_assert(tag);
	explosive_energy_ = tag->float_value_list_[3];

	xform transform;
	vec3 parent_velocity;
	ProjectileUtil::GetBarrel(this, transform, parent_velocity);
	vec3 velocity = transform.GetOrientation() * vec3(0, 0, muzzle_velocity_);
	velocity += parent_velocity;
	transform.GetPosition() += transform.GetOrientation() * vec3(0, 0, +3);
	const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(geometry->GetBodyId(), transform);
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(geometry->GetBodyId(), velocity);
}

void ServerProjectile::OnForceApplied(cure::ContextObject* other_object,
	tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
	const vec3& force, const vec3& torque,
	const vec3& position, const vec3& relative_velocity) {
	(void)other_object;
	(void)other_body_id;
	(void)own_body_id;
	(void)force;
	(void)torque;
	(void)relative_velocity;

	ProjectileUtil::Detonate(this, &is_detonated_, launcher_, position, relative_velocity, vec3(), explosive_energy_, 0);
}



loginstance(kGameContextCpp, ServerProjectile);



}
