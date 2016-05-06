
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/contextpath.h"
#include "../cure/include/cppcontextobject.h"
#include "../lepra/include/gametimer.h"
#include "downwashmanager.h"



namespace Downwash {



class CanonDriver: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	CanonDriver(DownwashManager* game, cure::GameObjectId canon_id, int ammo_type);
	virtual ~CanonDriver();

private:
	virtual void OnTick();

	DownwashManager* game_;
	cure::GameObjectId canon_id_;
	int ammo_type_;
	float distance_;
	float shoot_period_;
	bool tag_set_;
	GameTimer last_shot_;
	float joint_start_angle_;
	logclass();
};



}
