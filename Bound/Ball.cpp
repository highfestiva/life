
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "ball.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/gamemanager.h"



namespace Bound {



Ball::Ball(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager),
	average_speed_(0) {
}

Ball::~Ball() {
}



void Ball::OnTick() {
	Parent::OnTick();

	vec3 velocity = GetVelocity();
	bool normalize = false;
	float speed = velocity.GetLength();
	average_speed_ = Math::Lerp(average_speed_, speed, 0.1f);
	if (speed > 3.1f) {
		speed = 2.9f;
		normalize = true;
	} else if (average_speed_ < 1.0f) {
		speed = 2.9f;
		average_speed_ = 2.9f;
		normalize = true;
	}
	if (normalize) {
		velocity.Normalize(speed);
		const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
		manager_->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(geometry->GetBodyId(), velocity);
	}
}



}
