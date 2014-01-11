
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "FireManager.h"


namespace Fire
{



class Eater: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Eater(Cure::ContextManager* pManager);
	virtual ~Eater();

protected:
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal);

	LOG_CLASS_DECLARE();
};



}
