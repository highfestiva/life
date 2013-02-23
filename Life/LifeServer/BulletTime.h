
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../../Lepra/Include/Timer.h"
#include "../Life.h"



namespace Life
{



class BulletTime: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	BulletTime(Cure::ContextManager* pManager);
	virtual ~BulletTime();

private:
	virtual void FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger);
	virtual void OnTick();
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal);

	bool mAllowBulletTime;

	bool mLastFrameTriggered;
	bool mIsTriggerTimerStarted;
	Timer mTriggerTimer;

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
