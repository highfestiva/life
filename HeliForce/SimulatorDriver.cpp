
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "SimulatorDriver.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "Level.h"



namespace HeliForce
{



SimulatorDriver::SimulatorDriver(Cure::GameManager* pGame, Cure::GameObjectId pSimulatorId):
	Parent(pGame->GetResourceManager(), _T("SimulatorDriver")),
	mGame(pGame),
	mSimulatorId(pSimulatorId)
{
	pGame->GetContext()->AddLocalObject(this);
	pGame->GetContext()->AddAlarmCallback(this, 0, 5.0f, 0);
	pGame->GetContext()->AddAlarmCallback(this, 1, 10.0f, 0);
}

SimulatorDriver::~SimulatorDriver()
{
}



void SimulatorDriver::OnAlarm(int pAlarmId, void* pExtraData)
{
	Parent::OnAlarm(pAlarmId, pExtraData);

	Cure::ContextObject* lSimulator = mManager->GetObject(mSimulatorId, true);
	if (!lSimulator)
	{
		mManager->PostKillObject(GetInstanceId());
		return;
	}
	if (lSimulator->IsLoaded() && lSimulator->GetPhysics()->GetEngineCount() >= 2)
	{
		lSimulator->GetPhysics()->GetEngine(pAlarmId)->SetValue(pAlarmId, 1.0f);
	}
	GetManager()->AddAlarmCallback(this, pAlarmId, 3.0f, 0);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, SimulatorDriver);



}
