
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerMessageProcessor.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "../PushBarrel.h"
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
	(void)pPosition;
	(void)pExplosive;
	(void)pHitObject;
	(void)pExplosiveBodyId;
	(void)pHitBodyId;

	float lLevelShootEasyness = 12.5f;
	Cure::ContextManager::ContextObjectTable lObjectTable = mGameServerManager->GetContext()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		const Cure::ContextObject* lObject = x->second;
		TBC::ChunkyPhysics* lPhysics = lObject->ContextObject::GetPhysics();
		if (!lObject->IsLoaded() || !lPhysics)
		{
			continue;
		}
		// Dynamics only get hit in the main body, while statics gets all their dynamic sub-bodies hit.
		const Vector3DF lEpicenter = pPosition + Vector3DF(0, 0, -0.75f);
		const int lBoneStart = (lPhysics->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC)? 0 : 1;
		const int lBoneCount = (lPhysics->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC)? 1 : lPhysics->GetBoneCount();
		for (int x = lBoneStart; x < lBoneCount; ++x)
		{
			const TBC::ChunkyBoneGeometry* lGeometry = lPhysics->GetBoneGeometry(x);
			if (lGeometry->GetBoneType() != TBC::ChunkyBoneGeometry::BONE_BODY
				|| (x != 0 && lGeometry->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_EXCLUDE))
			{
				continue;
			}
			const Vector3DF lBodyCenter = mGameServerManager->GetPhysicsManager()->GetBodyPosition(lGeometry->GetBodyId());
			Vector3DF f = lBodyCenter - lEpicenter;
			float d = f.GetLength();
			if (d > 80*VISUAL_SCALE_FACTOR)
			{
				continue;
			}
			d = 1/d;
			f *= d;
			d *= lLevelShootEasyness;
			d = d*d*d;
			d = std::min(1.0f, d);
			const float lMaxForceFactor = 1200.0f;
			const float ff = lMaxForceFactor * lObject->GetMass() * d;
			if (f.z <= 0.1f)
			{
				f.z += 0.3f;
			}
			f *= ff;
			mGameServerManager->GetPhysicsManager()->AddForce(lGeometry->GetBodyId(), f);
		}
	}
}



LOG_CLASS_DEFINE(GAME, PushServerMessageProcessor);



}
