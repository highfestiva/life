
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../life/lifeserver/gameserverticker.h"
#include "../push.h"



namespace Push {



class PushServerTicker: public life::GameServerTicker {
	typedef life::GameServerTicker Parent;
public:
	PushServerTicker(cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity);
	virtual ~PushServerTicker();
};



}
