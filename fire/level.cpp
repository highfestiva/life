
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "level.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/contextpath.h"
#include "../cure/include/gamemanager.h"



namespace Fire {



Level::Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, cure::ContextForceListener* gravel_emitter):
	Parent(resource_manager, class_id, ui_manager, gravel_emitter),
	path_(0),
	level_speed_(1) {
}

Level::~Level() {
}

void Level::OnLoaded() {
	Parent::OnLoaded();

	const tbc::ChunkyClass::Tag* speed_tag = FindTag("driver", 1, 0);
	if (speed_tag) {
		level_speed_ = speed_tag->float_value_list_[0];
	}
	const tbc::ChunkyClass::Tag* gravity_tag = FindTag("behavior", 3, 0);
	if (gravity_tag) {
		vec3 gravity(gravity_tag->float_value_list_[0], gravity_tag->float_value_list_[1], gravity_tag->float_value_list_[2]);
		manager_->GetGameManager()->GetPhysicsManager()->SetGravity(gravity);
	} else {
		deb_assert(false);
	}
}

cure::ContextPath* Level::QueryPath() {
	if (!path_) {
		path_ = new cure::ContextPath(GetResourceManager(), "ContextPath");
		GetManager()->AddLocalObject(path_);
	}
	return path_;
}

float Level::GetLevelSpeed() const {
	return level_speed_;
}



}
