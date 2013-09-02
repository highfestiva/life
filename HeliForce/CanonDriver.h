
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/CppContextObject.h"
#include "HeliForceManager.h"



namespace HeliForce
{



class CanonDriver: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	CanonDriver(HeliForceManager* pGame, Cure::GameObjectId pCanonId);
	virtual ~CanonDriver();

private:
	virtual void OnTick();

	HeliForceManager* mGame;
	Cure::GameObjectId mCanonId;
	HiResTimer mLastShot;
	LOG_CLASS_DECLARE();
};



}
