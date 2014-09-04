
// Author: Jonas Byström
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
	virtual void FinalizeTrigger(const Tbc::PhysicsTrigger* pTrigger);
	virtual void OnTick();
	virtual void OnTrigger(Tbc::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pNormal);

	typedef std::unordered_map<Cure::GameObjectId, int> DoneMap;

	str mAttributeName;
	unsigned mTriggerCount;
	DoneMap mDoneMap;

	logclass();
};



}
