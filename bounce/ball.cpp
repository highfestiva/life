
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "ball.h"
#include "../cure/include/contextmanager.h"



namespace bounce {



Ball::Ball(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager) {
}

Ball::~Ball() {
}



void Ball::OnMicroTick(float frame_time) {
	Parent::OnMicroTick(frame_time);

	if (!IsLoaded()) {
		return;
	}
	tbc::PhysicsManager* physics_manager = GetManager()->GetGameManager()->GetPhysicsManager();
	vec3 velocity;
	physics_manager->GetBodyVelocity(
		GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
		velocity);
	float air_force = velocity.GetLength();
	air_force *= air_force * -0.002f;
	velocity.x *= air_force;
	velocity.y *= air_force;
	velocity.z *= air_force * 0.09f;
	physics_manager->AddForce(
		GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
		velocity);
}

void Ball::OnLoaded() {
	GetManager()->EnableMicroTickCallback(this);
	Parent::OnLoaded();
}



loginstance(kGameContextCpp, Ball);



}
