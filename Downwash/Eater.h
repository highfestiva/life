
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
	virtual void OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal);

	logclass();
};



}
