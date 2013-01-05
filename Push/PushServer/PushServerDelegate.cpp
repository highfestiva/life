
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerDelegate.h"
#include "../../Cure/Include/FloatAttribute.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "../PushBarrel.h"
#include "../Explosion.h"
#include "../Version.h"



namespace Push
{



PushServerDelegate::PushServerDelegate(Life::GameServerManager* pGameServerManager):
	Parent(pGameServerManager)
{
}

PushServerDelegate::~PushServerDelegate()
{
}



void PushServerDelegate::OnLoadAvatar(Life::Client* pClient, Cure::ContextObject* pAvatar)
{
	(void)pClient;
	Cure::FloatAttribute* lHealth = new Cure::FloatAttribute(pAvatar, _T("float_health"), 1);
	lHealth->SetNetworkType(Cure::FloatAttribute::TYPE_SERVER_BROADCAST);
}

void PushServerDelegate::OnLoadObject(Cure::ContextObject* pObject)
{
	(void)pObject;
}

void PushServerDelegate::GetBarrel(Cure::GameObjectId pOwnerId, TransformationF& pTransform, Vector3DF& pVelocity) const
{
	PushBarrel::GetInfo(mGameServerManager, pOwnerId, pTransform, pVelocity);
}

void PushServerDelegate::Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
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
			if (mGameServerManager->IsAvatarObject(lObject))
			{
				Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)lObject->GetAttribute(_T("float_health"));
				assert(lHealth);
				const float lRemainingHealth = lHealth->GetValue() - lForce*0.4f;
				if (lRemainingHealth >= 0.005f)
				{
					lHealth->SetValue(lRemainingHealth);
				}
				else
				{
					mGameServerManager->GetContext()->PostKillObject(lObject->GetInstanceId());
				}
			}
			x->second->ForceSend();
		}
	}
}



}
