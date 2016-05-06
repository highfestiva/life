
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "goal.h"
#include "../cure/include/cppcontextobject.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/runtimevariable.h"
#include "../tbc/include/physicstrigger.h"
#include "../uicure/include/uicppcontextobject.h"



namespace tirefire {



Goal::Goal(cure::ContextManager* manager):
	Parent(manager->GetGameManager()->GetResourceManager(), "Goal"),
	trigger_(0),
	is_triggered_(false) {
	manager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

Goal::~Goal() {
}



bool Goal::IsTriggered() const {
	return is_triggered_;
}

vec3 Goal::GetPosition() const {
	const tbc::ChunkyBoneGeometry* goal_geometry = trigger_->GetTriggerGeometry(0);
	return GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(goal_geometry->GetBodyId());
}

void Goal::FinalizeTrigger(const tbc::PhysicsTrigger* trigger) {
	trigger_ = trigger;
}

void Goal::OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	(void)trigger_id;
	(void)other_object;
	(void)body_id;
	(void)normal;
	is_triggered_ = true;
}



loginstance(kGameContextCpp, Goal);




}
