
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
	vec3 GetPosition() const;
	vec3 GetVelocity() const;
	float GetRadius() const;

protected:
	virtual void OnTick();
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	virtual void OnTrigger(Tbc::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pNormal);
	void Trig(const Tbc::PhysicsTrigger* pTrigger);
	Tbc::ChunkyBoneGeometry* GetFirstBody() const;

	float GetSignedMaxSpeedSquare() const;
	void HaltActiveEngines(bool pStop);

	void SetFunctionTarget(const str& pFunction, Tbc::PhysicsEngine* pEngine);

private:
	const Tbc::PhysicsTrigger* mActiveTrigger;
	HiResTimer mTrigTime;
	StopWatch mStopTimer;
	double mExitDelay;
	double mStopDelay;
	bool mElevatorHasBeenMoving;
	bool mElevatorIsActive;
	float mEngineActivity;

	logclass();
};



}
