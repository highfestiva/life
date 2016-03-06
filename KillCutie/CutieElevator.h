
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
	void OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal);

	Game* mGame;

	logclass();
};



}
