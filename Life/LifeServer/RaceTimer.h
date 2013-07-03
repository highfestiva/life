
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include <hash_map>
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
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal);

	typedef std::hash_map<Cure::GameObjectId, int> DoneMap;

	str mAttributeName;
	unsigned mTriggerCount;
	DoneMap mDoneMap;

	LOG_CLASS_DECLARE();
};



}
