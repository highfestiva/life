
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
	Elevator(Cure::ResourceManager* pResourceManager, Cure::ContextObject* pParent);
	virtual ~Elevator();

protected:
	virtual void OnTick(float pFrameTime);
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pBody);
	void Trig(const TBC::PhysicsTrigger* pTrigger);

	float GetActiveMaxSpeedSquare() const;
	void HaltActiveEngines();

	void SetFunctionTarget(const str& pFunction, TBC::PhysicsEngine* pEngine);

private:
	Cure::ContextObject* mParent;
	const TBC::PhysicsTrigger* mActiveTrigger;
	HiResTimer mTrigTime;
	const double mExitDelay;
	bool mAreEnginesActive;
	float mEngineActivity;

	LOG_CLASS_DECLARE();
};



}
