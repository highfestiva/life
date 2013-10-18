
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/CppContextObject.h"
#include "DownwashManager.h"



namespace Downwash
{



class DownwashManager;



class Autopilot: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Autopilot(DownwashManager* pGame);
	virtual ~Autopilot();
	void Reset();
	Vector3DF GetSteering();
	void AttemptCloserPathDistance();
	float GetClosestPathDistance() const;
	Vector3DF GetClosestPathVector() const;
	Vector3DF GetLastAvatarPosition() const;

private:
	void CheckStalledRotor(Cure::ContextObject* pChopper);
	float GetClosestPathDistance(const Vector3DF& pPosition, Vector3DF& pClosestPoint) const;

	typedef Cure::ContextPath::SplinePath Spline;

	DownwashManager* mGame;
	StopWatch mStalledRotorTimer;
	float mClosestPathDistance;
	Vector3DF mClosestPathPosition;
	Vector3DF mLastAvatarPosition;
	Spline* mPath;
	LOG_CLASS_DECLARE();
};



}
