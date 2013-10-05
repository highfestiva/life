
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Lepra/Include/GameTimer.h"
#include "DownwashManager.h"



namespace Downwash
{



class CanonDriver: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	CanonDriver(DownwashManager* pGame, Cure::GameObjectId pCanonId, int pAmmoType, float pShotsPerSecond);
	virtual ~CanonDriver();

private:
	virtual void OnTick();

	DownwashManager* mGame;
	Cure::GameObjectId mCanonId;
	int mAmmoType;
	float mDistance;
	bool mDistanceSet;
	float mShootPeriod;
	GameTimer mLastShot;
	float mJointStartAngle;
	LOG_CLASS_DECLARE();
};



}
