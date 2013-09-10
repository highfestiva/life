
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
	CanonDriver(HeliForceManager* pGame, Cure::GameObjectId pCanonId, int pAmmoType, float pShotsPerSecond);
	virtual ~CanonDriver();

private:
	virtual void OnTick();

	HeliForceManager* mGame;
	Cure::GameObjectId mCanonId;
	int mAmmoType;
	float mShootPeriod;
	HiResTimer mLastShot;
	float mJointStartAngle;
	LOG_CLASS_DECLARE();
};



}
