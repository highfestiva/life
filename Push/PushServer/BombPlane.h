
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../Push.h"



namespace Life
{
class Launcher;
}



namespace Push
{



class BombPlane: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	BombPlane(Cure::ResourceManager* pResourceManager, const str& pClassId, Life::Launcher* pLauncher, const Vector3DF& pTarget);
	virtual ~BombPlane();

private:
	virtual void OnLoaded();
	virtual void OnTick();

	Life::Launcher* mLauncher;
	Vector3DF mTarget;
	int mLastBombTick;
	float mBombingRadiusSquared;
	float mDropInterval;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
