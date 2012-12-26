
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Life/LifeServer/GameServerTicker.h"
#include "../Push.h"



namespace Push
{



class PushServerTicker: public Life::GameServerTicker
{
	typedef Life::GameServerTicker Parent;
public:
	PushServerTicker(Cure::ResourceManager* pResourceManager, InteractiveConsoleLogListener* pConsoleLogger, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~PushServerTicker();

};



}
