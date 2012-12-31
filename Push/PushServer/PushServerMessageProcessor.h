
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Life/LifeServer/ServerMessageProcessor.h"
#include "../Push.h"
#include "ServerLauncher.h"



namespace Push
{



class PushServerMessageProcessor: public Life::ServerMessageProcessor, public ServerLauncher	// TODO: don't use *this* for a grenade launcher!
{
	typedef Life::ServerMessageProcessor Parent;
public:
	PushServerMessageProcessor(Life::GameServerManager* pGameServerManager);
	virtual ~PushServerMessageProcessor();

private:
	virtual void ProcessNumber(Life::Client* pClient, Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat);
	virtual void GetBarrel(Cure::GameObjectId pOwnerId, TransformationF& pTransform, Vector3DF& pVelocity) const;
	virtual void PushServerMessageProcessor::Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pExplosive, Cure::ContextObject* pHitObject,
		TBC::PhysicsManager::BodyID pExplosiveBodyId, TBC::PhysicsManager::BodyID pHitBodyId);

	LOG_CLASS_DECLARE();
};



}
