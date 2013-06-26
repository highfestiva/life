
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "HoverTankServerTicker.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "HoverTankServerDelegate.h"
#include "HoverTankServerMessageProcessor.h"



namespace HoverTank
{



HoverTankServerTicker::HoverTankServerTicker(Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity)
{
	HoverTankServerDelegate* lDelegate = new HoverTankServerDelegate(mGameManager);
	mGameManager->SetDelegate(lDelegate);
	mGameManager->SetMessageProcessor(new HoverTankServerMessageProcessor(mGameManager, lDelegate));
}

HoverTankServerTicker::~HoverTankServerTicker()
{
}



}
