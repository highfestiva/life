
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/CppContextObject.h"
#include "DownwashManager.h"



namespace Downwash
{



class AutoPathDriver: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	AutoPathDriver(DownwashManager* pGame, Cure::GameObjectId pVehicleId, const str& pPathName);
	virtual ~AutoPathDriver();

private:
	virtual void OnTick();
	void GetClosestPathDistance(const Vector3DF& pPosition, Vector3DF& pClosestPoint, float pWantedDistance);

	DownwashManager* mGame;
	Cure::GameObjectId mVehicleId;
	const str mPathName;
	Cure::ContextPath::SplinePath* mPath;
	StopWatch mStillTimer;
	LOG_CLASS_DECLARE();
};



}
