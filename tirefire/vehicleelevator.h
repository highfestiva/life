
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/elevator.h"
#include "game.h"



namespace tirefire {


class Game;



class VehicleElevator: public cure::Elevator {
	typedef cure::Elevator Parent;
public:
	VehicleElevator(Game* game);
	virtual ~VehicleElevator();

private:
	void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);

	Game* game_;

	logclass();
};



}
