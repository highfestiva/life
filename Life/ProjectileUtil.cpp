
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "projectileutil.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/cppcontextobject.h"
#include "../cure/include/gamemanager.h"
#include "../cure/include/runtimevariable.h"
#include "../lepra/include/random.h"
#include "launcher.h"



namespace life {



bool ProjectileUtil::GetBarrel(cure::ContextObject* projectile, xform& transform, vec3& velocity) {
	deb_assert(projectile);
	//deb_assert(projectile->GetOwnerInstanceId());
	cure::CppContextObject* _shooter = (cure::CppContextObject*)projectile->GetManager()->GetObject(projectile->GetOwnerInstanceId());
	if (!_shooter) {
		//projectile->GetManager()->PostKillObject(projectile->GetInstanceId());
		return false;
	}
	return GetBarrelByShooter(_shooter, transform, velocity);
}

bool ProjectileUtil::GetBarrelByShooter(cure::CppContextObject* shooter, xform& transform, vec3& velocity) {
	transform.SetOrientation(shooter->GetOrientation());
	transform.GetOrientation().RotateAroundOwnX(-PIF/2);
	transform.SetPosition(shooter->GetPosition());
	velocity = shooter->GetVelocity();
	const tbc::ChunkyClass::Tag* tag = shooter->FindTag("muzzle", 0, 0);
	if (tag) {
		const int bone_index = tag->body_index_list_[0];
		const tbc::ChunkyBoneGeometry* bone = shooter->GetPhysics()->GetBoneGeometry(bone_index);
		deb_assert(bone->GetBoneType() == tbc::ChunkyBoneGeometry::kBonePosition);
#ifdef LEPRA_DEBUG
		//tbc::ChunkyBoneGeometry* root_geometry = shooter->GetPhysics()->GetBoneGeometry(0);
		//quat q = game_manager->GetPhysicsManager()->GetBodyOrientation(root_geometry->GetBodyId());
		//quat p = shooter->GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		//log_.Infof("Shooting with body orientation (%f;%f;%f;%f and initial orientation (%f;%f;%f;%f)."),
		//	q.a, q.b, q.c, q.d,
		//	p.a, p.b, p.c, p.d);
#endif // Debug
		const int parent_index = shooter->GetPhysics()->GetIndex(bone->GetParent());
		const tbc::PhysicsManager::BodyID parent_body_id = shooter->GetPhysics()->GetBoneGeometry(parent_index)->GetBodyId();
		const quat parent_orientation = shooter->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyOrientation(parent_body_id);
		const vec3 muzzle_offset = shooter->GetPhysics()->GetOriginalBoneTransformation(bone_index).GetPosition();
		transform.GetPosition() += parent_orientation * muzzle_offset;
		transform.SetOrientation(parent_orientation);
	}
	return true;
}

void ProjectileUtil::StartBullet(cure::ContextObject* bullet, float muzzle_velocity, bool use_barrel) {
	xform _transform;
	if (use_barrel) {
		vec3 parent_velocity;
		if (!GetBarrel(bullet, _transform, parent_velocity)) {
			return;
		}
		vec3 _velocity = _transform.GetOrientation() * vec3(0, 0, muzzle_velocity);
		_velocity += parent_velocity;
		bullet->SetRootOrientation(_transform.GetOrientation());
		bullet->SetRootVelocity(_velocity);
		_transform.GetPosition() += _transform.GetOrientation() * vec3(0, 0, 2);
	} else {
		_transform = bullet->GetInitialTransform();
	}
	const tbc::ChunkyBoneGeometry* geometry = bullet->GetPhysics()->GetBoneGeometry(bullet->GetPhysics()->GetRootBone());
	bullet->GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(geometry->GetBodyId(), _transform);

	bullet->GetManager()->EnableMicroTickCallback(bullet);	// Used hires movement + collision detection.
}


void ProjectileUtil::BulletMicroTick(cure::ContextObject* bullet, float frame_time, float max_velocity, float acceleration) {
	const tbc::ChunkyBoneGeometry* root_geometry = bullet->GetPhysics()->GetBoneGeometry(0);
	tbc::PhysicsManager::BodyID body = root_geometry->GetBodyId();
	xform _transform;
	bullet->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyTransform(body, _transform);
	vec3 _velocity = bullet->GetVelocity();
	_transform.GetPosition() += _velocity * frame_time;
	_transform.GetOrientation() = bullet->GetOrientation();
	bullet->GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(body, _transform);
	if (acceleration && _velocity.GetLengthSquared() < max_velocity*max_velocity) {
		const vec3 forward(0, acceleration*frame_time, 0);
		_velocity += _transform.GetOrientation() * forward;
		bullet->SetRootVelocity(_velocity);
	}
}

void ProjectileUtil::Detonate(cure::ContextObject* grenade, bool* is_detonated, Launcher* launcher, const vec3& position, const vec3& velocity, const vec3& normal,
	float strength, float delete_delay) {
	/*if (other_object->GetInstanceId() == GetOwnerInstanceId()) {
		return;
	}*/
	if (*is_detonated) {
		return;
	}
	*is_detonated = true;

	tbc::ChunkyPhysics* physics = grenade->GetPhysics();
	if (physics) {
		launcher->Detonate(grenade, physics->GetBoneGeometry(0), position, velocity, normal, strength);
		if (delete_delay == 0) {
			grenade->GetManager()->PostKillObject(grenade->GetInstanceId());
		} else if (delete_delay > 0) {
			grenade->GetManager()->DelayKillObject(grenade, delete_delay);
		}
	}
}

void ProjectileUtil::OnBulletHit(cure::ContextObject* bullet, bool* is_detonated, Launcher* launcher, cure::ContextObject* target) {
	if (target->GetInstanceId() == bullet->GetOwnerInstanceId()) {	// Can't hit oneself.
		return;
	}
	if (*is_detonated) {
		return;
	}
	*is_detonated = true;

	launcher->OnBulletHit(bullet, target);
	bullet->GetManager()->PostKillObject(bullet->GetInstanceId());
}

float ProjectileUtil::GetShotSounds(cure::ContextManager* manager, const strutil::strvec& sound_names, str& launch_sound_name, str& shreek_sound_name) {
	const size_t sound_count = sound_names.size() / 2;	// First half are launch sounds, last half are shreek sounds.
	if (sound_count) {
		launch_sound_name = sound_names[Random::GetRandomNumber()%sound_count];
		shreek_sound_name = sound_names[Random::GetRandomNumber()%sound_count + sound_count];
		float pitch;
		v_get(pitch, = (float), manager->GetGameManager()->GetVariableScope(), kRtvarPhysicsRtr, 1.0);
		return pitch;
	}
	return 0;
}



vec3 ProjectileUtil::CalculateInitialProjectileDirection(const vec3& distance, float acceleration, float terminal_speed, const vec3& gravity, float acceleration_gravity_recip) {
	// 1. How long time, t, will it take the missile to accelerate to the endpoint?
	// 2. Given t, how much (d) will the missile fall during it's travel (excluding g for optimization)?
	// 3. Compensate for projectile acceleration in gravitational direction.
	// 4. Create a quaternion pointing to dir+d.
	const float l = distance.GetLength();
	const float a = acceleration;
	const float vt = terminal_speed;
	const float r = ::exp(l*a/(vt*vt));
	float d = 0;
	deb_assert(r >= 1);
	if (r >= 1) {
		const float t = vt/a*Math::acosh(r);	// Derived from "free fall with air resistance" in Wikipedia. Thanks a bunch!
		// 2
		d = t*t*0.5f;
	}
	// 3
	const vec3 g = gravity.GetNormalized(acceleration_gravity_recip);
	const float f = 1 + distance*g/l;
	d *= f;
	// 4
	const vec3 _target = distance - d*gravity;
	const float xy = ::sqrt(_target.x*_target.x + _target.y*_target.y);
	//const float zy = ::sqrt(_target.z*_target.z + _target.y*_target.y);
	return vec3(::atan2(-_target.x, _target.y), ::atan2(_target.z, xy), 0);
}



loginstance(kGameContext, ProjectileUtil);



}
