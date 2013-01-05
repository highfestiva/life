
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Life/LifeServer/ServerDelegate.h"
#include "../Push.h"
#include "ServerLauncher.h"



namespace Push
{



class PushServerDelegate: public Life::ServerDelegate, public ServerLauncher
{
	typedef Life::ServerDelegate Parent;
public:
	PushServerDelegate(Life::GameServerManager* pGameServerManager);
	virtual ~PushServerDelegate();

	virtual void OnLoadAvatar(Life::Client* pClient, Cure::ContextObject* pAvatar);
	virtual void OnLoadObject(Cure::ContextObject* pObject);

private:
	virtual void GetBarrel(Cure::GameObjectId pOwnerId, TransformationF& pTransform, Vector3DF& pVelocity) const;
	virtual void Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pExplosive, Cure::ContextObject* pHitObject,
		TBC::PhysicsManager::BodyID pExplosiveBodyId, TBC::PhysicsManager::BodyID pHitBodyId);

	LOG_CLASS_DECLARE();
};



}
