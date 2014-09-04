
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Game.h"



namespace GrenadeRun
{



class RoboBall: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	RoboBall(const Game* pGame, const str& pClassId);
	virtual ~RoboBall();

private:
	void OnTick();

	const Game* mGame;
	HiResTimer mHeadAwayTimer;
	UiCure::UserSound3dResource* mSound;
	int mTimeFrameBlown;
	int mBadSpeedCounter;

	logclass();
};



}
