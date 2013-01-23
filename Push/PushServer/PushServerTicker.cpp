
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerTicker.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "PushServerDelegate.h"
#include "PushServerMessageProcessor.h"



namespace Push
{



PushServerTicker::PushServerTicker(Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity)
{
	PushServerDelegate* lDelegate = new PushServerDelegate(mGameManager);
	mGameManager->SetDelegate(lDelegate);
	mGameManager->SetMessageProcessor(new PushServerMessageProcessor(mGameManager, lDelegate));
}

PushServerTicker::~PushServerTicker()
{
}



}
