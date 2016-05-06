
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "racket.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/contextpath.h"
#include "game.h"



namespace bounce {



Racket::Racket(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager),
	path_(0) {
}

Racket::~Racket() {
	path_ = 0;
}



cure::ContextPath* Racket::QueryPath() {
	if (!path_) {
		path_ = new cure::ContextPath(GetResourceManager(), "ContextPath");
		GetManager()->AddLocalObject(path_);
	}
	return path_;
}



loginstance(kGameContextCpp, Racket);



}
