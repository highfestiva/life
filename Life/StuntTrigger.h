
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "../Lepra/Include/HiResTimer.h"
#include "Life.h"



namespace Life
{



class StuntTrigger: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	StuntTrigger(Cure::ContextManager* pManager, const str& pClassId);
	virtual ~StuntTrigger();

protected:
	virtual void FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger);
	virtual void OnTick();
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal);
	virtual void DidTrigger(Cure::ContextObject* pBody) = 0;

	bool mAllowBulletTime;

	bool mLastFrameTriggered;
	StopWatch mTriggerTimer;

	float mMinSpeed;
	float mMaxSpeed;
	float mMinTime;
	float mRealTimeRatio;
	float mBulletTimeDuration;
	str mClientStartCommand;
	str mClientStopCommand;

	LOG_CLASS_DECLARE();
};



}
