
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../HoverTank.h"



namespace Life
{
class Launcher;
}



namespace HoverTank
{



class BombPlane: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	BombPlane(Cure::ResourceManager* pResourceManager, const str& pClassId, Life::Launcher* pLauncher, const vec3& pTarget);
	virtual ~BombPlane();

private:
	virtual void OnLoaded();
	virtual void OnTick();

	Life::Launcher* mLauncher;
	vec3 mTarget;
	int mLastBombTick;
	float mBombingRadiusSquared;
	float mDropInterval;
	bool mIsDetonated;

	logclass();
};



}
