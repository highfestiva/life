
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerMessageProcessor.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "PushServerDelegate.h"
#include "ServerGrenade.h"



namespace Push
{



PushServerMessageProcessor::PushServerMessageProcessor(Life::GameServerManager* pGameServerManager):
	Parent(pGameServerManager)
{
}

PushServerMessageProcessor::~PushServerMessageProcessor()
{
}



void PushServerMessageProcessor::ProcessNumber(Life::Client* pClient, Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat)
{
	switch (pType)
	{
		case Cure::MessageNumber::INFO_APPLICATION_0:
		{
			Cure::ContextObject* lAvatar = mGameServerManager->GetContext()->GetObject(pClient->GetAvatarId());
			if (!lAvatar)
			{
				return;
			}
			PushServerDelegate* lDelegate = (PushServerDelegate*)mGameServerManager->GetDelegate();
			ServerGrenade* lGrenade = new ServerGrenade(mGameServerManager->GetResourceManager(), 200, lDelegate);
			mGameServerManager->AddContextObject(lGrenade, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
			bool lOk = (lGrenade != 0);
			assert(lOk);
			if (lOk)
			{
				log_volatile(mLog.Debugf(_T("Shooting grenade with ID %i!"), (int)lGrenade->GetInstanceId()));
				lGrenade->SetOwnerId(lAvatar->GetInstanceId());
				TransformationF t(lAvatar->GetOrientation(), lAvatar->GetPosition()+Vector3DF(0, 0, +5.0f));
				lGrenade->SetInitialTransform(t);
				lGrenade->StartLoading();
			}
		}
		return;
	}
	Parent::ProcessNumber(pClient, pType, pInteger, pFloat);
}



LOG_CLASS_DEFINE(GAME, PushServerMessageProcessor);



}
