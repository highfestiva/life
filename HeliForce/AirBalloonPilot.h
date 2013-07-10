
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/CppContextObject.h"
#include "HeliForceManager.h"



namespace HeliForce
{



class HeliForceManager;



class AirBalloonPilot: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	AirBalloonPilot(HeliForceManager* pGame, Cure::GameObjectId pBalloonId);
	virtual ~AirBalloonPilot();

private:
	virtual void OnTick();
	void GetClosestPathDistance(const Vector3DF& pPosition, Vector3DF& pClosestPoint);

	HeliForceManager* mGame;
	Cure::GameObjectId mBalloonId;
	Cure::ContextPath::SplinePath* mPath;
	LOG_CLASS_DECLARE();
};



}
