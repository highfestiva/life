
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "BulletTime.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "GameServerManager.h"



namespace Life
{



BulletTime::BulletTime(Cure::ContextManager* pManager):
	Parent(pManager, _T("BulletTime"))
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
			CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
			((GameServerManager*)GetManager()->GetGameManager())->BroadcastStatusMessage(
				Cure::MessageStatus::INFO_COMMAND,
				wstrutil::Encode(mClientStopCommand));
			break;
		case 1:
			mAllowBulletTime = true;
			break;
	}
}

void BulletTime::DidTrigger(Cure::ContextObject* pBody)
{
	(void)pBody;
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_PHYSICS_RTR, mRealTimeRatio);
	((GameServerManager*)GetManager()->GetGameManager())->BroadcastStatusMessage(
		Cure::MessageStatus::INFO_COMMAND,
		wstrutil::Encode(mClientStartCommand));

	GetManager()->AddGameAlarmCallback(this, 0, mBulletTimeDuration, 0);
	GetManager()->AddGameAlarmCallback(this, 1, std::max(mBulletTimeDuration*11, 60.0f), 0);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, BulletTime);




}
