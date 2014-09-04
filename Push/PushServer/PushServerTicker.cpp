
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "PushServerTicker.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "../../Life/LifeServer/ServerMessageProcessor.h"
#include "PushServerDelegate.h"



namespace Push
{



PushServerTicker::PushServerTicker(Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity)
{
	PushServerDelegate* lDelegate = new PushServerDelegate(mGameManager);
	mGameManager->SetDelegate(lDelegate);
	mGameManager->SetMessageProcessor(new Life::ServerMessageProcessor(mGameManager));
}

PushServerTicker::~PushServerTicker()
{
}



}
