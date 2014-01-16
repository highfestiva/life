
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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

	void SetStopDelay(double pStopDelay);
	Vector3DF GetPosition() const;
	Vector3DF GetVelocity() const;
	float GetRadius() const;

protected:
	virtual void OnTick();
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, TBC::PhysicsManager::BodyID pBodyId, const Vector3DF& pNormal);
	void Trig(const TBC::PhysicsTrigger* pTrigger);
	TBC::ChunkyBoneGeometry* GetFirstBody() const;

	float GetSignedMaxSpeedSquare() const;
	void HaltActiveEngines(bool pStop);

	void SetFunctionTarget(const str& pFunction, TBC::PhysicsEngine* pEngine);

private:
	const TBC::PhysicsTrigger* mActiveTrigger;
	HiResTimer mTrigTime;
	StopWatch mStopTimer;
	double mExitDelay;
	double mStopDelay;
	bool mElevatorHasBeenMoving;
	bool mElevatorIsActive;
	float mEngineActivity;

	LOG_CLASS_DECLARE();
};



}
