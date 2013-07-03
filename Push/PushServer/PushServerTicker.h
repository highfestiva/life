
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../Life/LifeServer/GameServerTicker.h"
#include "../Push.h"



namespace Push
{



class PushServerTicker: public Life::GameServerTicker
{
	typedef Life::GameServerTicker Parent;
public:
	PushServerTicker(Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~PushServerTicker();
};



}
