
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Life/LifeServer/GameServerTicker.h"
#include "../HoverTank.h"



namespace HoverTank
{



class HoverTankServerTicker: public Life::GameServerTicker
{
	typedef Life::GameServerTicker Parent;
public:
	HoverTankServerTicker(Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~HoverTankServerTicker();
};



}
