
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "mine.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/health.h"
#include "../projectileutil.h"



namespace life {



Mine::Mine(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, Launcher* launcher):
	Parent(resource_manager, class_id, ui_manager, launcher),
	enable_delete_detonation_(true) {
	cure::Health::Set(this, 1);
}

Mine::~Mine() {
	if (enable_delete_detonation_) {
		OnDie();
	}
}

void Mine::EnableDeleteDetonation(bool enable) {
	enable_delete_detonation_ = enable;
}

void Mine::OnDie() {
	Parent::OnDie();
	GetManager()->PostKillObject(GetInstanceId());
}



loginstance(kGameContextCpp, Mine);



}
