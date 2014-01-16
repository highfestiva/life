
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../../Lepra/Include/Unordered.h"
#include "../../Lepra/Include/Timer.h"
#include "../Life.h"



namespace Life
{



class RaceTimer: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	RaceTimer(Cure::ContextManager* pManager);
	virtual ~RaceTimer();

private:
	virtual void FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger);
	virtual void OnTick();
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, TBC::PhysicsManager::BodyID pBodyId, const Vector3DF& pNormal);

	typedef std::unordered_map<Cure::GameObjectId, int> DoneMap;

	str mAttributeName;
	unsigned mTriggerCount;
	DoneMap mDoneMap;

	LOG_CLASS_DECLARE();
};



}
