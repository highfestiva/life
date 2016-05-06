
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "canondriver.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/health.h"
#include "../lepra/include/random.h"
#include "../life/projectileutil.h"
#include "level.h"



namespace Downwash {



CanonDriver::CanonDriver(DownwashManager* game, cure::GameObjectId canon_id, int ammo_type):
	Parent(game->GetResourceManager(), "CanonDriver"),
	game_(game),
	canon_id_(canon_id),
	ammo_type_(ammo_type),
	distance_(1000),
	shoot_period_(1),
	tag_set_(false) {
	game->GetContext()->AddLocalObject(this);
	game->GetContext()->EnableTickCallback(this);

	cure::CppContextObject* canon = (cure::CppContextObject*)manager_->GetObject(canon_id_, true);
	deb_assert(canon);
	xform muzzle_transform;
	vec3 _;
	life::ProjectileUtil::GetBarrelByShooter(canon, muzzle_transform, _);
	joint_start_angle_ = (muzzle_transform.GetOrientation() * vec3(0,0,1)).GetAngle(vec3(0,0,1));
}

CanonDriver::~CanonDriver() {
}



void CanonDriver::OnTick() {
	Parent::OnTick();

	cure::CppContextObject* canon = (cure::CppContextObject*)manager_->GetObject(canon_id_, true);
	if (!canon) {
		manager_->PostKillObject(GetInstanceId());
		return;
	}
	if (!canon->IsLoaded() || canon->GetPhysics()->GetEngineCount() < 1) {
		return;
	}
	if (!tag_set_) {
		const tbc::ChunkyClass::Tag* tag = canon->FindTag("behavior", 2, 0);
		deb_assert(tag);
		distance_ = tag->float_value_list_[0];
		shoot_period_ = 1/tag->float_value_list_[1];
		tag_set_ = true;
	}

	cure::ContextObject* avatar = manager_->GetObject(game_->GetAvatarInstanceId());
	if (!avatar) {
		return;
	}

	const vec3 target(avatar->GetPosition() + avatar->GetVelocity()*0.2f);
	const vec2 d(target.z - canon->GetPosition().z, target.x - canon->GetPosition().x);
	if (d.GetLengthSquared() >= distance_*distance_) {
		return;	// Don't shoot at distant objects.
	}
	tbc::ChunkyBoneGeometry* barrel = canon->GetPhysics()->GetBoneGeometry(1);
	tbc::PhysicsManager::Joint1Diff diff;
	game_->GetPhysicsManager()->GetJoint1Diff(barrel->GetBodyId(), barrel->GetJointId(), diff);
	const float angle = -d.GetAngle() - (joint_start_angle_ + diff.value_);
	const float target_angle = Math::Clamp(angle*20, -2.0f, +2.0f);
	canon->SetEnginePower(1, -target_angle);

	if (last_shot_.QueryTimeDiff() >= 0) {
		float low_angle = 0;
		float high_angle = 0;
		float bounce;
		game_->GetPhysicsManager()->GetJointParams(barrel->GetJointId(), low_angle, high_angle, bounce);
		if (std::abs(angle) < std::abs(high_angle-low_angle)*0.1f) {
			last_shot_.Start(-Random::Normal(shoot_period_, shoot_period_/4, shoot_period_*0.75f, shoot_period_*1.25f));
			game_->Shoot(canon, ammo_type_);
		}
	}
}



loginstance(kGameContextCpp, CanonDriver);



}
