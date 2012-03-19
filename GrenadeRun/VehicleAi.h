
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "Game.h"



namespace Cure
{
class Elevator;
}



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
		MODE_STOPPING_AT_GOAL,
		MODE_AT_GOAL,
		MODE_WAITING_FOR_ELEVATOR,
		MODE_GET_ON_ELEVATOR,
		MODE_ON_ELEVATOR,
	};

	virtual void OnTick();
	bool AvoidGrenade(const Vector3DF& pPosition, const Vector3DF& pVelocity, float pCaution);
	void SetMode(Mode pMode);
	bool IsCloseToTarget(const Vector3DF& pPosition, float pDistance) const;
	float GetClosestPathDistance(const Vector3DF& pPosition, int pPath = -1, float* pLikeliness = 0) const;
	const Cure::Elevator* GetClosestElevator(const Vector3DF& pPosition) const;

	Game* mGame;
	Mode mMode;
	int mModeStartFrame;
	int mStoppedFrame;
	int mActivePath;

	LOG_CLASS_DECLARE();
};



}
