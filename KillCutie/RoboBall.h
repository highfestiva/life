
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"
#include "game.h"



namespace grenaderun {



class RoboBall: public UiCure::Machine {
public:
	typedef UiCure::Machine Parent;

	RoboBall(const Game* game, const str& class_id);
	virtual ~RoboBall();

private:
	void OnTick();

	const Game* game_;
	HiResTimer head_away_timer_;
	UiCure::UserSound3dResource* sound_;
	int bad_speed_counter_;

	logclass();
};



}
