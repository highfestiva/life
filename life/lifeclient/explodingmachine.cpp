
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "explodingmachine.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/health.h"
#include "../projectileutil.h"



namespace life {



ExplodingMachine::ExplodingMachine(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, Launcher* launcher):
	Parent(resource_manager, class_id, ui_manager),
	launcher_(launcher),
	trigger_death_frame_(-1),
	death_frame_delay_(1),
	disappear_after_death_delay_(-1),
	is_detonated_(false),
	explosive_strength_(2) {
}

ExplodingMachine::~ExplodingMachine() {
}



void ExplodingMachine::SetExplosiveStrength(float explosive_strength) {
	explosive_strength_ = explosive_strength;
}

void ExplodingMachine::SetDeathFrameDelay(int death_frame_delay) {
	death_frame_delay_ = death_frame_delay;
}

void ExplodingMachine::SetDisappearAfterDeathDelay(float disappear_delay) {
	disappear_after_death_delay_ = disappear_delay;
}

void ExplodingMachine::OnTick() {
	Parent::OnTick();

	if (cure::Health::Get(this, 1) <= 0 && trigger_death_frame_ < 0) {
		trigger_death_frame_ = 0;
	}
	if (trigger_death_frame_ >= 0) {
		if (++trigger_death_frame_ > death_frame_delay_) {
			OnDie();
		}
	}
}

void ExplodingMachine::OnDie() {
	ProjectileUtil::Detonate(this, &is_detonated_, launcher_, GetPosition(), GetVelocity(), vec3(), explosive_strength_, disappear_after_death_delay_);
}



loginstance(kGameContextCpp, ExplodingMachine);



}
