
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	void Reset();
	Vector3DF GetSteering();
	float GetClosestPathDistance() const;
	Vector3DF GetLastAvatarPosition() const;

private:
	float GetClosestPathDistance(const Vector3DF& pPosition, Vector3DF& pClosestPoint) const;

	HeliForceManager* mGame;
	float mClosestPathDistance;
	Vector3DF mLastAvatarPosition;
	LOG_CLASS_DECLARE();
};



}
