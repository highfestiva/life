
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerMessageProcessor.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "../PushBarrel.h"
#include "../Explosion.h"
#include "../Version.h"
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
			ServerGrenade* lGrenade = new ServerGrenade(mGameServerManager->GetResourceManager(), 200, this);
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



void PushServerMessageProcessor::GetBarrel(Cure::GameObjectId pOwnerId, TransformationF& pTransform, Vector3DF& pVelocity) const
{
	PushBarrel::GetInfo(mGameServerManager, pOwnerId, pTransform, pVelocity);
}

void PushServerMessageProcessor::Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pExplosive, Cure::ContextObject* pHitObject,
	TBC::PhysicsManager::BodyID pExplosiveBodyId, TBC::PhysicsManager::BodyID pHitBodyId)
{
	(void)pForce;
	(void)pTorque;
	(void)pExplosive;
	(void)pHitObject;
	(void)pExplosiveBodyId;
	(void)pHitBodyId;

	TBC::PhysicsManager* lPhysicsManager = mGameServerManager->GetPhysicsManager();
	Cure::ContextManager::ContextObjectTable lObjectTable = mGameServerManager->GetContext()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		const Cure::ContextObject* lObject = x->second;
		if (!lObject->IsLoaded())
		{
			continue;
		}
		const float lForce = Explosion::PushObject(lPhysicsManager, lObject, pPosition, 1.0f);
		if (lForce > 0 && lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			x->second->ForceSend();
		}
	}
}



LOG_CLASS_DEFINE(GAME, PushServerMessageProcessor);



}
