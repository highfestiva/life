
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerTicker.h"
#include "../../Life/LifeServer/GameServerManager.h"



namespace Push
{



PushServerTicker::PushServerTicker(Cure::ResourceManager* pResourceManager, InteractiveConsoleLogListener* pConsoleLogger, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pResourceManager, pConsoleLogger, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity)
{
	mGameManager->SetLevelName(_T("level_01"));
}

PushServerTicker::~PushServerTicker()
{
}



}
