
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "level.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/contextpath.h"
#include "game.h"



namespace tirefire {



Level::Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager),
	path_(0) {
}

Level::~Level() {
	path_ = 0;
}



cure::ContextPath* Level::QueryPath() {
	if (!path_) {
		path_ = new cure::ContextPath(GetResourceManager(), "ContextPath");
		GetManager()->AddLocalObject(path_);
	}
	return path_;
}



loginstance(kGameContextCpp, Level);



}
