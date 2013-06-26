
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "HoverTankServerMessageProcessor.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "GameServerLogic.h"
#include "HoverTankServerDelegate.h"



namespace HoverTank
{



HoverTankServerMessageProcessor::HoverTankServerMessageProcessor(Life::GameServerManager* pGameServerManager, GameServerLogic* pLogic):
	Parent(pGameServerManager),
	mLogic(pLogic)
{
}

HoverTankServerMessageProcessor::~HoverTankServerMessageProcessor()
{
	mLogic = 0;
}



void HoverTankServerMessageProcessor::ProcessNumber(Life::Client* pClient, Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat)
{
	switch (pType)
	{
		case Cure::MessageNumber::INFO_TOOL_0:
		{
			Cure::ContextObject* lAvatar = mGameServerManager->GetContext()->GetObject(pClient->GetAvatarId());
			if (lAvatar)
			{
				mLogic->Shoot(lAvatar, (int)pFloat);
			}
		}
		return;
	}
	Parent::ProcessNumber(pClient, pType, pInteger, pFloat);
}



LOG_CLASS_DEFINE(GAME, HoverTankServerMessageProcessor);



}
