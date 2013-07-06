
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/ContextPath.h"
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
	void CheckStalledRotor(Cure::ContextObject* pChopper);
	float GetClosestPathDistance(const Vector3DF& pPosition, Vector3DF& pClosestPoint) const;

	typedef Cure::ContextPath::SplinePath Spline;

	HeliForceManager* mGame;
	StopWatch mStalledRotorTimer;
	float mClosestPathDistance;
	Vector3DF mLastAvatarPosition;
	Spline* mPath;
	LOG_CLASS_DECLARE();
};



}
