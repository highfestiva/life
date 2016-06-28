
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "cutie.h"
#include "../lepra/include/math.h"
#include "../tbc/include/chunkybonegeometry.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/runtimevariable.h"
#include "game.h"



namespace grenaderun {



Cutie::Cutie(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager),
	health_(1),
	kill_joints_tick_count_(2),
	wheel_expel_tick_count_(3) {
	SetForceLoadUnique(true);	// Needs to be unique as physics are modified and reloaded on every death.
}

Cutie::~Cutie() {
}



void Cutie::DrainHealth(float drain) {
	if (!health_) {	// Already killed?
		return;
	}
	health_ = Math::Clamp(health_-drain, 0.0f, 1.0f);
	if (health_) {
		return;
	}

	// Was killed. Drop all wheels! :)
	kill_joints_tick_count_ = 2;
	wheel_expel_tick_count_ = 3;
	float real_time_ratio;
	v_get(real_time_ratio, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
	wheel_expel_tick_count_ = (int)(wheel_expel_tick_count_/real_time_ratio);
	if (GetPhysics()) {
		GetPhysics()->ClearEngines();
	}
	/*tbc::ChunkyClass* clazz = (tbc::ChunkyClass*)GetClass();
	size_t tag_count = clazz->GetTagCount();
	for (size_t x = 0; x < tag_count;) {
		clazz->RemoveTag(x);
		--tag_count;
	}
	DeleteEngineSounds();*/
}

float Cutie::GetHealth() const {
	return health_;
}

bool Cutie::QueryFlip() {
	if (!health_ || !IsUpsideDown()) {
		return false;
	}

	// Reset vehicle in the direction it was heading.
	const cure::ObjectPositionalData* original_position_data;
	if (UpdateFullPosition(original_position_data)) {
		cure::ObjectPositionalData position_data;
		position_data.CopyData(original_position_data);
		position_data.Stop();
		xform& transform = position_data.position_.transformation_;
		transform.SetPosition(GetPosition() + vec3(0, 0, 1.5f));
		vec3 euler_angles;
		GetOrientation().GetEulerAngles(euler_angles);
		transform.GetOrientation().SetEulerAngles(euler_angles.x, 0, 0);
		transform.GetOrientation() *= GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		SetFullPosition(position_data, 0);
		return true;
	}
	return false;
}

bool Cutie::IsUpsideDown() const {
	vec3 up(0, 0, 1);
	up = GetOrientation() * up;
	if (up.z > 0.4f ||
		GetVelocity().GetLengthSquared() > 0.1f ||
		GetAngularVelocity().GetLengthSquared() > 0.1f) {
		// Nope, still standing, or at least moving. Might be drunken style,
		// but at least not on it's head yet.
		return false;
	}
	return true;
}



void Cutie::OnTick() {
	Parent::OnTick();

	// If killed: check if we should expel wheels! :)
	if (health_) {
		return;
	}
	if (wheel_expel_tick_count_ <= 0) {
		return;
	}
	--kill_joints_tick_count_;
	--wheel_expel_tick_count_;
	if (kill_joints_tick_count_ <= 0) {
		kill_joints_tick_count_ = 0x7FFFFFFF;
		// Remove the joints, but don't allow collisions with body yet.
		const vec3 position = GetPosition();
		const int bone_count = GetPhysics()->GetBoneCount();
		for (int x = 0; x < bone_count; ++x) {
			tbc::ChunkyBoneGeometry* wheel = GetPhysics()->GetBoneGeometry(x);
			if (wheel->GetJointType() != tbc::ChunkyBoneGeometry::kJointExclude) {
				GetManager()->GetGameManager()->GetPhysicsManager()->DeleteJoint(wheel->GetJointId());
				wheel->ResetJointId();
				// Push the wheel away somewhat, not too much.
				const float push_factor = 200;
				const vec3 wheel_position = GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(wheel->GetBodyId());
				const vec3 force = (wheel_position-position).GetNormalized()*push_factor*wheel->GetMass();
				GetManager()->GetGameManager()->GetPhysicsManager()->AddForce(wheel->GetBodyId(), force);
			}
		}
	}
	if (wheel_expel_tick_count_ > 0) {
		return;
	}
	// Allow collisions with body.
	const vec3 position = GetPosition();
	const int bone_count = GetPhysics()->GetBoneCount();
	for (int x = 0; x < bone_count; ++x) {
		tbc::ChunkyBoneGeometry* wheel = GetPhysics()->GetBoneGeometry(x);
		if (wheel->GetJointType() == tbc::ChunkyBoneGeometry::kJointExclude ||
			!GetManager()->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListenerId(wheel->GetBodyId())) {
			continue;
		}
		vec3 wheel_position = GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(wheel->GetBodyId());
		bool far_away = (position.GetDistanceSquared(wheel_position) >= 5*5);
		if (!far_away) {
			const quat car_inverse = GetOrientation().GetInverse();
			wheel_position = car_inverse * (wheel_position - position);
			const float min_distance = wheel->GetShapeSize().x * 0.5f;
			if (::fabs(wheel_position.x) > ::fabs(GetPhysics()->GetOriginalBoneTransformation(x).GetPosition().x) + min_distance) {
				far_away = true;
			}
		}
		if (far_away) {
			GetManager()->RemovePhysicsBody(wheel->GetBodyId());
			GetManager()->GetGameManager()->GetPhysicsManager()->SetForceFeedbackListener(wheel->GetBodyId(), 0);
		} else {
			// Come back later for this one.
			wheel_expel_tick_count_ = 1;
		}
	}
}



loginstance(kGameContextCpp, Cutie);



}
