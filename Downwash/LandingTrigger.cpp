
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "landingtrigger.h"
#include "../cure/include/contextmanager.h"
#include "downwashmanager.h"



namespace Downwash {



LandingTrigger::LandingTrigger(cure::ContextManager* manager):
	Parent(manager, "LandingTrigger") {
}

LandingTrigger::~LandingTrigger() {
}



void LandingTrigger::DidTrigger(cure::ContextObject* body) {
	if (!strutil::StartsWith(body->GetClassId(), "helicopter")) {
		allow_bullet_time_ = true;
		return;
	}
	if (((DownwashManager*)GetManager()->GetGameManager())->DidFinishLevel()) {
		GetManager()->AddGameAlarmCallback(this, 0, bullet_time_duration_, 0);
	} else {
		allow_bullet_time_ = true;
	}
}

void LandingTrigger::OnAlarm(int alarm_id, void* extra_data) {
	Parent::OnAlarm(alarm_id, extra_data);
	((DownwashManager*)GetManager()->GetGameManager())->StepLevel(+1);
}



loginstance(kGameContextCpp, LandingTrigger);




}
