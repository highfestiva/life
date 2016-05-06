
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "level.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/contextpath.h"



namespace Downwash {



Level::Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, cure::ContextForceListener* gravel_emitter):
	Parent(resource_manager, class_id, ui_manager, gravel_emitter),
	path_(0) {
}

Level::~Level() {
}

void Level::OnLoaded() {
	Parent::OnLoaded();

	const tbc::ChunkyClass::Tag* tag = FindTag("textures", 0, 1);
	if (tag) {
		background_name_ = tag->string_value_list_[0];
	}
}

const str& Level::GetBackgroundName() const {
	return background_name_;
}

cure::ContextPath* Level::QueryPath() {
	if (!path_) {
		path_ = new cure::ContextPath(GetResourceManager(), "ContextPath");
		GetManager()->AddLocalObject(path_);
	}
	return path_;
}



}
