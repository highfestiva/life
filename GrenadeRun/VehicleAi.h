
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
		MODE_BACKING_UP,
		MODE_AT_GOAL,
	};

	virtual void OnTick();
	bool AvoidGrenade(const Vector3DF& pPosition, const Vector3DF& pVelocity, float pCaution);
	void SetMode(Mode pMode);
	bool IsCloseToTarget(const Vector3DF& pPosition) const;
	float GetClosestPathDistance(const Vector3DF& pPosition, int pPath = -1) const;

	Game* mGame;
	Mode mMode;
	int mModeStartFrame;
	int mStoppedFrame;
	int mActivePath;

	LOG_CLASS_DECLARE();
};



}
