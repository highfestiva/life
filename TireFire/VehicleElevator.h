
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/Elevator.h"
#include "Game.h"



namespace TireFire
{


class Game;



class VehicleElevator: public Cure::Elevator
{
	typedef Cure::Elevator Parent;
public:
	VehicleElevator(Game* pGame);
	virtual ~VehicleElevator();

private:
	void OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal);

	Game* mGame;

	logclass();
};



}
