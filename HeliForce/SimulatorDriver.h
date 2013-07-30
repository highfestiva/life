
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "HeliForceManager.h"



namespace HeliForce
{



class SimulatorDriver: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	SimulatorDriver(Cure::GameManager* pGame, Cure::GameObjectId pSimulatorId);
	virtual ~SimulatorDriver();

	virtual void OnAlarm(int pAlarmId, void* pExtraData);

private:
	Cure::GameManager* mGame;
	Cure::GameObjectId mSimulatorId;
	LOG_CLASS_DECLARE();
};



}
