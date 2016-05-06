
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/elevator.h"
#include "game.h"



namespace grenaderun {


class Game;



class CutieElevator: public cure::Elevator {
	typedef cure::Elevator Parent;
public:
	CutieElevator(Game* game);
	virtual ~CutieElevator();

private:
	void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);

	Game* game_;

	logclass();
};



}
