
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerTicker.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "PushServerDelegate.h"
#include "PushServerMessageProcessor.h"



namespace Push
{



PushServerTicker::PushServerTicker(Cure::ResourceManager* pResourceManager, InteractiveConsoleLogListener* pConsoleLogger, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pResourceManager, pConsoleLogger, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity)
{
	mGameManager->SetLevelName(_T("level_02"));
	mGameManager->SetDelegate(new PushServerDelegate(mGameManager));
	mGameManager->SetMessageProcessor(new PushServerMessageProcessor(mGameManager));
}

PushServerTicker::~PushServerTicker()
{
}



}
