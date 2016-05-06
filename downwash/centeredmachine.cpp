
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "centeredmachine.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/gamemanager.h"



namespace Downwash {



CenteredMachine::CenteredMachine(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, life::Launcher* launcher):
	Parent(resource_manager, class_id, ui_manager, launcher) {
}

CenteredMachine::~CenteredMachine() {
}


bool CenteredMachine::TryComplete() {
	if (Parent::TryComplete()) {
		GetManager()->EnableMicroTickCallback(this);
		return true;
	}
	return false;
}

void CenteredMachine::OnMicroTick(float frame_time) {
	Parent::OnMicroTick(frame_time);

	if (!is_detonated_) {
		tbc::PhysicsManager* physics_manager = GetManager()->GetGameManager()->GetPhysicsManager();
		tbc::PhysicsManager::BodyID body_id = GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		vec3 v = physics_manager->GetBodyPosition(body_id);
		v.y = 0;
		physics_manager->SetBodyPosition(body_id, v);
		physics_manager->GetBodyVelocity(body_id, v);
		v.y = 0;
		physics_manager->SetBodyVelocity(body_id, v);
	}
}



loginstance(kGameContextCpp, CenteredMachine);



}
