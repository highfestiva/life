
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "HeliForceManager.h"


namespace HeliForce
{



class StoneEater: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	StoneEater(Cure::ContextManager* pManager);
	virtual ~StoneEater();

protected:
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal);

	LOG_CLASS_DECLARE();
};



}
