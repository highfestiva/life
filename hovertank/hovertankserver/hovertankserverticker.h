
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../life/lifeserver/gameserverticker.h"
#include "../hovertank.h"



namespace HoverTank {



class HoverTankServerTicker: public life::GameServerTicker {
	typedef life::GameServerTicker Parent;
public:
	HoverTankServerTicker(cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity);
	virtual ~HoverTankServerTicker();
};



}
