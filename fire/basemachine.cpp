
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "basemachine.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/health.h"
#include "../cure/include/gamemanager.h"
#include "../life/explosion.h"
#include "../life/launcher.h"
#include "../uicure/include/uiburnemitter.h"
#include "rtvar.h"

#define kSinkTime	4.0f
#define kStartSinkingId	10



namespace Fire {



BaseMachine::BaseMachine(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, life::Launcher* launcher):
	Parent(resource_manager, class_id, ui_manager, launcher),
	level_speed_(1),
	panic_level_(0),
	dangerousness_(0),
	did_get_to_town_(false) {
	cure::Health::Set(this, 1);
}

BaseMachine::~BaseMachine() {
}



void BaseMachine::AddPanic(float panic) {
	panic_level_ = Math::Clamp(panic_level_+panic, 0.0f, 1.1f);
}



void BaseMachine::OnAlarm(int alarm_id, void* extra_data) {
	Parent::OnAlarm(alarm_id, extra_data);

	if (alarm_id == kStartSinkingId) {
		life::Explosion::Freeze(manager_->GetGameManager()->GetPhysicsManager(), this);
		SetSinking(12/kSinkTime);
		manager_->DelayKillObject(this, kSinkTime);
	}
}

void BaseMachine::OnDie() {
	if (is_detonated_) {
		return;
	}

	if (GetBurnEmitter()) {
		GetBurnEmitter()->SetFreeFlow();
	}

	life::Explosion::FallApart(GetManager()->GetGameManager()->GetPhysicsManager(), this, true);
	ShrinkMeshBigOrientationThreshold(1e-3f);
	Parent::OnDie();
	float vehicle_remove_delay;
	v_get(vehicle_remove_delay, =(float), GetManager()->GetGameManager()->GetVariableScope(), kRtvarGameVehicleRemoveDelay, 9.0);
	manager_->AddGameAlarmCallback(this, kStartSinkingId, vehicle_remove_delay, 0);
}



}
