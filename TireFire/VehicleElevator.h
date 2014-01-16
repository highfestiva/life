
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
	void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, TBC::PhysicsManager::BodyID pBodyId, const Vector3DF& pNormal);

	Game* mGame;

	LOG_CLASS_DECLARE();
};



}
