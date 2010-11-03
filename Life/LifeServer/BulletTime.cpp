
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "BulletTime.h"
#include <algorithm>
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsTrigger.h"
#include "../LifeClient/RtVar.h"
#include "GameServerManager.h"



namespace Life
{



BulletTime::BulletTime(Cure::ContextManager* pManager):
	Cure::CppContextObject(pManager->GetGameManager()->GetResourceManager(), _T("BulletTime")),
	mAllowBulletTime(true),
	mLastFrameTriggered(false),
	mIsTriggerTimerStarted(false),
	mMinSpeed(0),
	mMaxSpeed(-1),
	mMinTime(-1),
	mRealTimeRatio(-1),
	mBulletTimeDuration(0)
{
	pManager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

BulletTime::~BulletTime()
{
}



void BulletTime::FinalizeTriggers()
{
	std::vector<int> lTriggerIndexArray;
	TriggerMap::iterator x = mTriggerMap.begin();
	for (; x != mTriggerMap.end(); ++x)
	{
		const TBC::ChunkyPhysics* lPhysics = mParent->GetPhysics();
		const int lTriggerCount = lPhysics->GetTriggerCount();
		for (int y = 0; y < lTriggerCount; ++y)
		{
			const TBC::PhysicsTrigger* lTrigger = lPhysics->GetTrigger(y);
			if (lTrigger->GetPhysicsTriggerId() == x->first)
			{
				lTriggerIndexArray.push_back(y);
				break;
			}
		}
	}
	std::sort(lTriggerIndexArray.begin(), lTriggerIndexArray.end());
	const TBC::ChunkyClass::Tag* lTag = ((CppContextObject*)mParent)->FindTag(_T("trigger_data"), 5, 2, lTriggerIndexArray);
	assert(lTag);
	if (lTag)
	{
		mMinSpeed		= lTag->mFloatValueList[0];
		mMaxSpeed		= lTag->mFloatValueList[1];
		mMinTime		= lTag->mFloatValueList[2];
		mRealTimeRatio		= lTag->mFloatValueList[3];
		mBulletTimeDuration	= lTag->mFloatValueList[4];
		mClientStartCommand	= lTag->mStringValueList[0];
		mClientStopCommand	= lTag->mStringValueList[1];
	}
}

void BulletTime::OnTick(float)
{
	if (!mLastFrameTriggered)
	{
		mIsTriggerTimerStarted = false;
	}
	mLastFrameTriggered = false;
	mTriggerTimer.UpdateTimer();
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

void BulletTime::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pListener)
{
	if (!mAllowBulletTime)
	{
		return;
	}

	mLastFrameTriggered = true;

	ContextObject* lObject = (ContextObject*)pListener;
	const float lSpeed = lObject->GetVelocity().GetLength();
	if (lSpeed < mMinSpeed || lSpeed > mMaxSpeed)
	{
		mIsTriggerTimerStarted = false;
		return;
	}
	if (!mIsTriggerTimerStarted)
	{
		mIsTriggerTimerStarted = true;
		mTriggerTimer.PopTimeDiff();
	}
	if (mTriggerTimer.GetTimeDiff() < mMinTime)
	{
		return;
	}

	mAllowBulletTime = false;
	const TBC::PhysicsTrigger* lTrigger = (const TBC::PhysicsTrigger*)GetTrigger(pTriggerId);
	assert(lTrigger);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_PHYSICS_RTR, mRealTimeRatio);
	((GameServerManager*)GetManager()->GetGameManager())->BroadcastStatusMessage(
		Cure::MessageStatus::INFO_COMMAND,
		wstrutil::Encode(mClientStartCommand));

	GetManager()->AddAlarmCallback(this, 0, mBulletTimeDuration, 0);
	GetManager()->AddAlarmCallback(this, 1, std::max(mBulletTimeDuration*11, 60.0f), 0);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, BulletTime);




}
