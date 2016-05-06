
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "bullettime.h"
#include "../../cure/include/runtimevariable.h"
#include "gameservermanager.h"



namespace life {



BulletTime::BulletTime(cure::ContextManager* manager):
	Parent(manager, "BulletTime") {
}

BulletTime::~BulletTime() {
}



void BulletTime::OnAlarm(int alarm_id, void* extra_data) {
	Parent::OnAlarm(alarm_id, extra_data);
	switch (alarm_id) {
		case 0:
			v_set(cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
			((GameServerManager*)GetManager()->GetGameManager())->BroadcastStatusMessage(
				cure::MessageStatus::kInfoCommand,
				client_stop_command_);
			break;
		case 1:
			allow_bullet_time_ = true;
			break;
	}
}

void BulletTime::DidTrigger(cure::ContextObject* body) {
	(void)body;
	v_set(cure::GetSettings(), kRtvarPhysicsRtr, real_time_ratio_);
	((GameServerManager*)GetManager()->GetGameManager())->BroadcastStatusMessage(
		cure::MessageStatus::kInfoCommand,
		client_start_command_);

	GetManager()->AddGameAlarmCallback(this, 0, bullet_time_duration_, 0);
	GetManager()->AddGameAlarmCallback(this, 1, std::max(bullet_time_duration_*11, 60.0f), 0);
}



loginstance(kGameContextCpp, BulletTime);




}
