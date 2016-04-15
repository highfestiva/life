
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "BulletTime.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "GameServerManager.h"



namespace Life
{



BulletTime::BulletTime(Cure::ContextManager* pManager):
	Parent(pManager, "BulletTime")
{
}

BulletTime::~BulletTime()
{
}



void BulletTime::OnAlarm(int pAlarmId, void* pExtraData)
{
	Parent::OnAlarm(pAlarmId, pExtraData);
	switch (pAlarmId)
	{
		case 0:
			v_set(Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
			((GameServerManager*)GetManager()->GetGameManager())->BroadcastStatusMessage(
				Cure::MessageStatus::INFO_COMMAND,
				mClientStopCommand);
			break;
		case 1:
			mAllowBulletTime = true;
			break;
	}
}

void BulletTime::DidTrigger(Cure::ContextObject* pBody)
{
	(void)pBody;
	v_set(Cure::GetSettings(), RTVAR_PHYSICS_RTR, mRealTimeRatio);
	((GameServerManager*)GetManager()->GetGameManager())->BroadcastStatusMessage(
		Cure::MessageStatus::INFO_COMMAND,
		mClientStartCommand);

	GetManager()->AddGameAlarmCallback(this, 0, mBulletTimeDuration, 0);
	GetManager()->AddGameAlarmCallback(this, 1, std::max(mBulletTimeDuration*11, 60.0f), 0);
}



loginstance(GAME_CONTEXT_CPP, BulletTime);




}
