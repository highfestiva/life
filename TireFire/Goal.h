
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "../Lepra/Include/HiResTimer.h"
#include "Game.h"



namespace TireFire
{



class Goal: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Goal(Cure::ContextManager* pManager);
	virtual ~Goal();

	Vector3DF GetPosition() const;
	bool IsTriggered() const;

private:
	virtual void FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger);
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody);

	const TBC::PhysicsTrigger* mTrigger;
	bool mIsTriggered;

	LOG_CLASS_DECLARE();
};



}
