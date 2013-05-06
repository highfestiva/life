
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "StuntTrigger.h"
#include <algorithm>
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../TBC/Include/ChunkyPhysics.h"
#include "../TBC/Include/PhysicsTrigger.h"



namespace Life
{



StuntTrigger::StuntTrigger(Cure::ContextManager* pManager, const str& pClassId):
	Cure::CppContextObject(pManager->GetGameManager()->GetResourceManager(), pClassId),
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

StuntTrigger::~StuntTrigger()
{
}



void StuntTrigger::FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger)
{
	std::vector<int> lTriggerIndexArray;
	const TBC::ChunkyPhysics* lPhysics = mParent->GetPhysics();
	const int lBoneCount = pTrigger->GetTriggerGeometryCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		const int lBoneIndex = lPhysics->GetIndex(pTrigger->GetTriggerGeometry(x));
		assert(lBoneIndex >= 0);
		lTriggerIndexArray.push_back(lBoneIndex);
	}
	const TBC::ChunkyClass::Tag* lTag = ((CppContextObject*)mParent)->FindTag(_T("stunt_trigger_data"), 5, 2, &lTriggerIndexArray);
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

void StuntTrigger::OnTick()
{
	if (!mLastFrameTriggered)
	{
		mIsTriggerTimerStarted = false;
	}
	mLastFrameTriggered = false;
	mTriggerTimer.UpdateTimer();
}

void StuntTrigger::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal)
{
	(void)pTriggerId;
	(void)pNormal;

	if (!mAllowBulletTime)
	{
		return;
	}

	mLastFrameTriggered = true;

	ContextObject* lObject = pBody;
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
	DidTrigger();
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, StuntTrigger);




}
