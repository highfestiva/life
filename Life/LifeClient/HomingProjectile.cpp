
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "homingprojectile.h"
#include "../../cure/include/contextmanager.h"



namespace life {



HomingProjectile::HomingProjectile(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, Launcher* launcher):
	Parent(resource_manager, class_id, ui_manager, launcher),
	target_(0) {
}

HomingProjectile::~HomingProjectile() {
}

void HomingProjectile::SetTarget(cure::GameObjectId target) {
	target_ = target;
}



void HomingProjectile::OnTick() {
	Parent::OnTick();

	cure::ContextObject* object = GetManager()->GetObject(target_);
	if (object && object->GetPhysics()->GetEngineCount() >= 1) {
		vec3 pos = GetPosition();
		vec3 vel = GetVelocity();
		vec3 delta = object->GetPosition() - pos;
		if (max_velocity_ > 0) {
			const float t = delta.GetLength() / max_velocity_;
			delta += object->GetVelocity() * t;
		}
		delta.Normalize();
		const float xy = delta.ProjectOntoPlane(vec3(0,0,1)).GetLength();
		quat q;
		q.SetEulerAngles(-::atan2(delta.x, delta.y), ::atan2(delta.z, xy), 0);
		const float v = vel.GetLength();
		const vec3 velocity = q * vec3(0, v, 0);
		SetRootOrientation(q);
		SetRootVelocity(velocity);
	}
}



loginstance(kGameContextCpp, HomingProjectile);



}
