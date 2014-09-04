
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/CppContextObject.h"
#include "DownwashManager.h"



namespace Downwash
{



class DownwashManager;



class AirBalloonPilot: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	AirBalloonPilot(DownwashManager* pGame, Cure::GameObjectId pBalloonId);
	virtual ~AirBalloonPilot();

private:
	virtual void OnTick();
	void GetClosestPathDistance(const vec3& pPosition, vec3& pClosestPoint);

	DownwashManager* mGame;
	Cure::GameObjectId mBalloonId;
	Cure::ContextPath::SplinePath* mPath;
	logclass();
};



}
