
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "simulatordriver.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/health.h"
#include "../lepra/include/random.h"
#include "level.h"



namespace Downwash {



SimulatorDriver::SimulatorDriver(cure::GameManager* game, cure::GameObjectId simulator_id):
	Parent(game->GetResourceManager(), "SimulatorDriver"),
	game_(game),
	simulator_id_(simulator_id) {
	game->GetContext()->AddLocalObject(this);
	game->GetContext()->AddGameAlarmCallback(this, 0, 5.0f, 0);
	game->GetContext()->AddGameAlarmCallback(this, 1, 5.7f, 0);
}

SimulatorDriver::~SimulatorDriver() {
}



void SimulatorDriver::OnAlarm(int alarm_id, void* extra_data) {
	Parent::OnAlarm(alarm_id, extra_data);

	cure::ContextObject* simulator = manager_->GetObject(simulator_id_, true);
	if (!simulator) {
		manager_->PostKillObject(GetInstanceId());
		return;
	}
	if (simulator->IsLoaded() && simulator->GetPhysics()->GetEngineCount() >= 2) {
		float force = 0;
		const unsigned r = (Random::GetRandomNumber() % 21) + 1;
		const unsigned middle = (alarm_id == 0)? 11 : 14;
		if (r <= middle-1) force = -1;
		if (r >= middle+1) force = +1;
		simulator->GetPhysics()->SetEnginePower(alarm_id, force);
	}
	//GetManager()->AddGameAlarmCallback(this, alarm_id, Random::Uniform(0.5f, 1.5f), 0);
	GetManager()->AddGameAlarmCallback(this, alarm_id, 1.5f, 0);
}



loginstance(kGameContextCpp, SimulatorDriver);



}
