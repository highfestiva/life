
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "basemachine.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/health.h"
#include "../cure/include/gamemanager.h"
#include "../life/explosion.h"
#include "../life/launcher.h"



namespace Downwash {



BaseMachine::BaseMachine(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, life::Launcher* launcher):
	Parent(resource_manager, class_id, ui_manager, launcher) {
	cure::Health::Set(this, 1);
}

BaseMachine::~BaseMachine() {
}



void BaseMachine::OnDie() {
	if (is_detonated_) {
		//GetManager()->DelayKillObject(this, 3);
		return;
	}

	life::Explosion::FallApart(GetManager()->GetGameManager()->GetPhysicsManager(), this, true);
	Parent::OnDie();
	//CenterMeshes();
}



}
