
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



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
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pListener);

	typedef std::hash_map<Cure::GameObjectId, int> DoneMap;

	str mAttributeName;
	unsigned mTriggerCount;
	DoneMap mDoneMap;

	LOG_CLASS_DECLARE();
};



}
