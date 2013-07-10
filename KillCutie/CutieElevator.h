
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/Elevator.h"
#include "Game.h"



namespace GrenadeRun
{


class Game;



class CutieElevator: public Cure::Elevator
{
	typedef Cure::Elevator Parent;
public:
	CutieElevator(Game* pGame);
	virtual ~CutieElevator();

private:
	void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal);

	Game* mGame;

	LOG_CLASS_DECLARE();
};



}
