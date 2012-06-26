
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/Elevator.h"



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
	void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pBody);

	Game* mGame;

	LOG_CLASS_DECLARE();
};



}
