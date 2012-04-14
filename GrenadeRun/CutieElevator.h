
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/Elevator.h"



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
	void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pBody);

	Game* mGame;

	LOG_CLASS_DECLARE();
};



}
