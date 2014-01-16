
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "DownwashManager.h"


namespace Downwash
{



class Eater: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Eater(Cure::ContextManager* pManager);
	virtual ~Eater();

protected:
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, TBC::PhysicsManager::BodyID pBodyId, const Vector3DF& pNormal);

	LOG_CLASS_DECLARE();
};



}
