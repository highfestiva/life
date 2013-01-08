
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "ServerGrenade.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "ServerLauncher.h"



namespace Push
{



ServerGrenade::ServerGrenade(Cure::ResourceManager* pResourceManager, float pMuzzleVelocity, ServerLauncher* pLauncher):
	Parent(pResourceManager, _T("grenade")),
	mMuzzleVelocity(pMuzzleVelocity),
	mLauncher(pLauncher),
	mIsLaunched(false),
	mIsExploded(false),
	mOwnerId(0)
{
	SetForceLoadUnique(true);	// Needs to be unique as physics are reloaded often with shared IDs.
}

ServerGrenade::~ServerGrenade()
{
}



Cure::GameObjectId ServerGrenade::GetOwnerId() const
{
	return mOwnerId;
}

void ServerGrenade::SetOwnerId(Cure::GameObjectId pOwnerId)
{
	mOwnerId = pOwnerId;
}



void ServerGrenade::OnLoaded()
{
	Parent::OnLoaded();

	TransformationF lTransform;
	Vector3DF lParentVelocity;
	mLauncher->GetBarrel(mOwnerId, lTransform, lParentVelocity);
	Vector3DF lVelocity = lTransform.GetOrientation() * Vector3DF(0, 0, mMuzzleVelocity);
	lVelocity += lParentVelocity;
	lTransform.GetPosition() += lTransform.GetOrientation() * Vector3DF(0, 0, +3);
	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(lGeometry->GetBodyId(), lTransform);
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetBodyId(), lVelocity);

#ifdef LEPRA_DEBUG
	Vector3DF lPos = GetPosition();
	log_volatile(mLog.Debugf(_T("Server grenade started at pos (%f;%f;%f)."), lPos.x, lPos.y, lPos.z));
#endif // Debug
}

void ServerGrenade::OnTick()
{
	Vector3DF p = GetPosition();
	if (p.x > +3000 || p.x <= -3000 ||
		p.y > +3000 || p.y <= -3000 ||
		p.z > +3000 || p.z <= -300)
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
	Parent::OnTick();
}

void ServerGrenade::OnForceApplied(Cure::ContextObject* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	(void)pOwnBodyId;
	(void)pRelativeVelocity;

	if (pOtherObject->GetInstanceId() == mOwnerId)
	{
		return;
	}
	if (mIsExploded)
	{
		return;
	}
	mIsExploded = true;

	GetManager()->PostKillObject(GetInstanceId());
	mLauncher->Detonate(pForce, pTorque, pPosition, this, pOtherObject, pOwnBodyId, pOtherBodyId);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, ServerGrenade);



}
