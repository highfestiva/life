
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/cppcontextobject.h"
#include "game.h"



namespace grenaderun {



class Game;



class LauncherAi: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	LauncherAi(Game* game);
	virtual ~LauncherAi();
	void Init();

private:
	virtual void OnTick();

	Game* game_;
	HiResTimer last_shot_;
	vec3 target_offset_;
	bool did_shoot_;
	int shot_count_;

	logclass();
};



}
