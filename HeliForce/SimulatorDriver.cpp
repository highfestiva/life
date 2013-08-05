
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "SimulatorDriver.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "../Lepra/Include/Random.h"
#include "Level.h"



namespace HeliForce
{



SimulatorDriver::SimulatorDriver(Cure::GameManager* pGame, Cure::GameObjectId pSimulatorId):
	Parent(pGame->GetResourceManager(), _T("SimulatorDriver")),
	mGame(pGame),
	mSimulatorId(pSimulatorId)
{
	pGame->GetContext()->AddLocalObject(this);
	pGame->GetContext()->AddAlarmCallback(this, 0, 1.0f, 0);
	pGame->GetContext()->AddAlarmCallback(this, 1, 1.7f, 0);
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
		float lForce = 0;
		const unsigned r = (Random::GetRandomNumber() % 21) + 1;
		const unsigned lMiddle = (pAlarmId == 0)? 11 : 14;
		if (r <= lMiddle-1) lForce = -1;
		if (r >= lMiddle+1) lForce = +1;
		lSimulator->GetPhysics()->SetEnginePower(pAlarmId, lForce);
	}
	//GetManager()->AddAlarmCallback(this, pAlarmId, Random::Uniform(0.5f, 1.5f), 0);
	GetManager()->AddAlarmCallback(this, pAlarmId, 1.5f, 0);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, SimulatorDriver);



}
