
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "Game.h"



namespace GrenadeRun
{



class Game;



class VehicleAi: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	VehicleAi(Game* pGame);
	virtual ~VehicleAi();
	void Init();

private:
	enum Mode
	{
		MODE_FIND_BEST_PATH = 1,
		MODE_NORMAL,
		MODE_HEADING_BACK_ON_TRACK,
		MODE_BACKING_UP_DUE_TO_CRASH,
		MODE_CIRCUMVENTING_AFTER_BACKING,
		MODE_BACKING_ON_TRACK,
		MODE_REVERSING_DUE_TO_GRENADE,
		MODE_AT_GOAL,
	};

	virtual void OnTick();
	void SetMode(Mode pMode);
	float GetClosestPathDistance(const Vector3DF& pPosition, int pPath = -1) const;

	Game* mGame;
	Mode mMode;
	int mModeStartFrame;
	int mActivePath;

	LOG_CLASS_DECLARE();
};



}
