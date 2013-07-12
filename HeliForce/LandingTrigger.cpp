
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "LandingTrigger.h"
#include "../Cure/Include/ContextManager.h"
#include "HeliForceManager.h"



namespace HeliForce
{



LandingTrigger::LandingTrigger(Cure::ContextManager* pManager):
	Parent(pManager, _T("LandingTrigger"))
{
}

LandingTrigger::~LandingTrigger()
{
}



void LandingTrigger::DidTrigger(Cure::ContextObject* pBody)
{
	if (!strutil::StartsWith(pBody->GetClassId(), _T("helicopter")))
	{
		mAllowBulletTime = true;
		return;
	}
	if (((HeliForceManager*)GetManager()->GetGameManager())->DidFinishLevel())
	{
		GetManager()->AddAlarmCallback(this, 0, mBulletTimeDuration, 0);
	}
}

void LandingTrigger::OnAlarm(int pAlarmId, void* pExtraData)
{
	Parent::OnAlarm(pAlarmId, pExtraData);
	((HeliForceManager*)GetManager()->GetGameManager())->StepLevel(+1);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, LandingTrigger);




}
