
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

	vec3 GetPosition() const;
	bool IsTriggered() const;

private:
	virtual void FinalizeTrigger(const Tbc::PhysicsTrigger* pTrigger);
	virtual void OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal);

	const Tbc::PhysicsTrigger* mTrigger;
	bool mIsTriggered;

	logclass();
};



}
