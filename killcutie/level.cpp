
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "level.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/contextpath.h"
#include "game.h"



namespace grenaderun {



Level::Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, cure::ContextForceListener* gravel_emitter):
	Parent(resource_manager, class_id, ui_manager),
	path_(0),
	gravel_emitter_(gravel_emitter) {
}

Level::~Level() {
	delete gravel_emitter_;
	gravel_emitter_ = 0;
	path_ = 0;
}



cure::ContextPath* Level::QueryPath() {
	if (!path_) {
		path_ = new cure::ContextPath(GetResourceManager(), "ContextPath");
		GetManager()->AddLocalObject(path_);
	}
	return path_;
}



void Level::OnForceApplied(cure::ContextObject* other_object,
	tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
	const vec3& force, const vec3& torque,
	const vec3& position, const vec3& relative_velocity) {
	Parent::OnForceApplied(other_object, own_body_id, other_body_id, force, torque, position, relative_velocity);

	gravel_emitter_->OnForceApplied(this, other_object, own_body_id, other_body_id, force, torque, position, relative_velocity);
}



loginstance(kGameContextCpp, Level);



}
