
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/physicsmanager.h"



namespace tbc {



PhysicsManager::PhysicsManager():
	trigger_callback_(0),
	force_feedback_callback_(0) {
}

PhysicsManager::~PhysicsManager() {
}



void PhysicsManager::SetTriggerListener(TriggerListener* trigger_callback) {
	trigger_callback_ = trigger_callback;
}

void PhysicsManager::SetForceFeedbackListener(ForceFeedbackListener* force_feedback_callback) {
	force_feedback_callback_ = force_feedback_callback;
}



int PhysicsManager::QueryRayCollisionAgainst(const xform& ray_transform, float length, BodyID body,
	vec3* collision_points, int max_collision_count) {
	vec3 dir = ray_transform.GetOrientation() * vec3(0, 0, 1);
	return QueryRayCollisionAgainst(ray_transform.GetPosition(), dir, length, body, collision_points, max_collision_count);
}



}
