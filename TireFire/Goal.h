
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, TBC::PhysicsManager::BodyID pBodyId, const Vector3DF& pNormal);

	const TBC::PhysicsTrigger* mTrigger;
	bool mIsTriggered;

	LOG_CLASS_DECLARE();
};



}
