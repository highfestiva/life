
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerMessageProcessor.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "GameServerLogic.h"
#include "PushServerDelegate.h"



namespace Push
{



PushServerMessageProcessor::PushServerMessageProcessor(Life::GameServerManager* pGameServerManager, GameServerLogic* pLogic):
	Parent(pGameServerManager),
	mLogic(pLogic)
{
}

PushServerMessageProcessor::~PushServerMessageProcessor()
{
	mLogic = 0;
}



void PushServerMessageProcessor::ProcessNumber(Life::Client* pClient, Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat)
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



LOG_CLASS_DEFINE(GAME, PushServerMessageProcessor);



}
