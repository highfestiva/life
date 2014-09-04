
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "Game.h"



namespace GrenadeRun
{



class Game;



class LauncherAi: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	LauncherAi(Game* pGame);
	virtual ~LauncherAi();
	void Init();

private:
	virtual void OnTick();

	Game* mGame;
	HiResTimer mLastShot;
	vec3 mTargetOffset;
	bool mDidShoot;
	int mShotCount;

	logclass();
};



}
