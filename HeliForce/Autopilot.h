
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "HeliForceManager.h"



namespace HeliForce
{



class HeliForceManager;



class Autopilot: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Autopilot(HeliForceManager* pGame);
	virtual ~Autopilot();
	Vector3DF GetSteering();

private:
	float GetClosestPathDistance(const Vector3DF& pPosition) const;

	HeliForceManager* mGame;
	LOG_CLASS_DECLARE();
};



}
