
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "BaseMachine.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "../Cure/Include/GameManager.h"
#include "../Life/Explosion.h"
#include "../Life/Launcher.h"
#include "../UiCure/Include/UiBurnEmitter.h"

#define DIE_SINK_DELAY		9.0f
#define SINK_TIME		4.0f
#define START_SINKING_ID	10



namespace Fire
{



BaseMachine::BaseMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager, pLauncher),
	mLevelSpeed(1),
	mPanicLevel(0)
{
	Cure::Health::Set(this, 1);
}

BaseMachine::~BaseMachine()
{
}



void BaseMachine::OnAlarm(int pAlarmId, void* pExtraData)
{
	Parent::OnAlarm(pAlarmId, pExtraData);

	if (pAlarmId == START_SINKING_ID)
	{
		Life::Explosion::Freeze(mManager->GetGameManager()->GetPhysicsManager(), this);
		SetSinking(12/SINK_TIME);
		mManager->GetGameManager()->DeleteContextObjectDelay(this, SINK_TIME);
	}
}

void BaseMachine::OnDie()
{
	if (mIsDetonated)
	{
		return;
	}

	if (GetBurnEmitter())
	{
		GetBurnEmitter()->SetFreeFlow();
	}

	Life::Explosion::FallApart(GetManager()->GetGameManager()->GetPhysicsManager(), this);
	Parent::OnDie();
	mManager->AddAlarmCallback(this, START_SINKING_ID, DIE_SINK_DELAY, 0);
}



}
