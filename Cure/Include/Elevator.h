
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Lepra/Include/HiResTimer.h"
#include "CppContextObject.h"
#include "Cure.h"



namespace Cure
{



class Elevator: public CppContextObject
{
	typedef CppContextObject Parent;
public:
	Elevator(ContextManager* pManager);
	virtual ~Elevator();

protected:
	virtual void OnTick();
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pBody);
	void Trig(const TBC::PhysicsTrigger* pTrigger);

	float GetActiveMaxSpeedSquare() const;
	void HaltActiveEngines(bool pStop);

	void SetFunctionTarget(const str& pFunction, TBC::PhysicsEngine* pEngine);

private:
	const TBC::PhysicsTrigger* mActiveTrigger;
	HiResTimer mTrigTime;
	StopWatch mStopTimer;
	double mStoppedTime;
	const double mExitDelay;
	const double mStopDelay;
	bool mElevatorHasBeenMoving;
	bool mElevatorIsActive;
	float mEngineActivity;

	LOG_CLASS_DECLARE();
};



}