
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "level.h"



namespace life {



Level::Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, cure::ContextForceListener* gravel_emitter):
	Parent(resource_manager, class_id, ui_manager),
	gravel_emitter_(gravel_emitter) {
}

Level::~Level() {
	delete gravel_emitter_;
	gravel_emitter_ = 0;
}

void Level::OnLoaded() {
	Parent::OnLoaded();

	const tbc::ChunkyClass::Tag* tag = FindTag("mass_objects", -1, -1);
	if (tag) {
		deb_assert(tag->string_value_list_.size() == tag->float_value_list_.size());
		deb_assert(tag->string_value_list_.size() == tag->body_index_list_.size());
		const size_t count = tag->body_index_list_.size();
		for (size_t x = 0; x < count; ++x) {
			MassObjectInfo info;
			info.class_id_ = tag->string_value_list_[x];
			info.ground_body_index_ = tag->body_index_list_[x];
			info.count_ = (int)tag->float_value_list_[x];
			mass_objects_.push_back(info);
		}
	}
}



Level::MassObjectList Level::GetMassObjects() const {
	return mass_objects_;
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
