
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "HeliForceManager.h"


namespace HeliForce
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
