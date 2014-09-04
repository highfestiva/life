
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
	vec3 GetSteering();
	void AttemptCloserPathDistance();
	float GetClosestPathDistance() const;
	vec3 GetClosestPathVector() const;
	vec3 GetLastAvatarPosition() const;
	float GetRotorSpeed(const Cure::ContextObject* pChopper) const;

private:
	void CheckStalledRotor(Cure::ContextObject* pChopper);
	float GetClosestPathDistance(const vec3& pPosition, vec3& pClosestPoint) const;

	typedef Cure::ContextPath::SplinePath Spline;

	DownwashManager* mGame;
	GameTimer mStalledRotorTimer;
	float mClosestPathDistance;
	vec3 mClosestPathPosition;
	vec3 mLastAvatarPosition;
	Spline* mPath;
	logclass();
};



}
