
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "StuntTrigger.h"
#include <algorithm>
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../Tbc/Include/ChunkyPhysics.h"
#include "../Tbc/Include/PhysicsTrigger.h"



namespace Life
{



StuntTrigger::StuntTrigger(Cure::ContextManager* pManager, const str& pClassId):
	Cure::CppContextObject(pManager->GetGameManager()->GetResourceManager(), pClassId),
	mAllowBulletTime(true),
	mLastFrameTriggered(false),
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



void StuntTrigger::FinalizeTrigger(const Tbc::PhysicsTrigger* pTrigger)
{
	std::vector<int> lTriggerIndexArray;
	const Tbc::ChunkyPhysics* lPhysics = mParent->GetPhysics();
	const int lBoneCount = pTrigger->GetTriggerGeometryCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		const int lBoneIndex = lPhysics->GetIndex(pTrigger->GetTriggerGeometry(x));
		deb_assert(lBoneIndex >= 0);
		lTriggerIndexArray.push_back(lBoneIndex);
	}
	const Tbc::ChunkyClass::Tag* lTag = ((CppContextObject*)mParent)->FindTag("stunt_trigger_data", 5, 2, &lTriggerIndexArray);
	deb_assert(lTag);
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
		mTriggerTimer.Stop();
	}
	mLastFrameTriggered = false;
}

void StuntTrigger::OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal)
{
	(void)pTriggerId;
	(void)pBodyId;
	(void)pPosition;
	(void)pNormal;

	if (!mAllowBulletTime)
	{
		return;
	}

	mLastFrameTriggered = true;

	ContextObject* lObject = pOtherObject;
	const float lSpeed = lObject->GetVelocity().GetLength();
	if (lSpeed < mMinSpeed || lSpeed > mMaxSpeed)
	{
		mTriggerTimer.Stop();
		return;
	}
	mTriggerTimer.TryStart();
	if (mTriggerTimer.QueryTimeDiff() < mMinTime)
	{
		return;
	}

	mAllowBulletTime = false;
	DidTrigger(lObject);
}



loginstance(GAME_CONTEXT_CPP, StuntTrigger);




}
