
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "../Lepra/Include/HiResTimer.h"
#include "Life.h"



namespace Life
{



class Elevator: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Elevator(Cure::ResourceManager* pResourceManager);
	virtual ~Elevator();

protected:
	virtual void OnTick(float pFrameTime);
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pBody);

	static void SetFunctionTarget(const str& pFunction, TBC::PhysicsEngine* pEngine);

private:
	const TBC::PhysicsTrigger* mActiveTrigger;
	HiResTimer mTrigTime;
	const double mExitDelay;

	LOG_CLASS_DECLARE();
};



}
