
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "ServerProjectile.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Life/Launcher.h"
#include "../../Life/ProjectileUtil.h"



namespace Push
{



ServerProjectile::ServerProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, float pMuzzleVelocity, Life::Launcher* pLauncher):
	Parent(pResourceManager, pClassId),
	mMuzzleVelocity(pMuzzleVelocity),
	mLauncher(pLauncher),
	mIsDetonated(false),
	mExplosiveEnergy(1)
{
}

ServerProjectile::~ServerProjectile()
{
}



void ServerProjectile::OnLoaded()
{
	Parent::OnLoaded();

	const TBC::ChunkyClass::Tag* lTag = FindTag(_T("ammo"), 1, 2);
	assert(lTag);
	mExplosiveEnergy = lTag->mFloatValueList[0];

	TransformationF lTransform;
	Vector3DF lParentVelocity;
	Life::ProjectileUtil::GetBarrel(this, lTransform, lParentVelocity);
	Vector3DF lVelocity = lTransform.GetOrientation() * Vector3DF(0, 0, mMuzzleVelocity);
	lVelocity += lParentVelocity;
	lTransform.GetPosition() += lTransform.GetOrientation() * Vector3DF(0, 0, +3);
	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(lGeometry->GetBodyId(), lTransform);
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
}

void ServerProjectile::OnForceApplied(Cure::ContextObject* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	(void)pOtherObject;
	(void)pOtherBodyId;
	(void)pOwnBodyId;
	(void)pForce;
	(void)pTorque;
	(void)pRelativeVelocity;

	Life::ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, pPosition, mExplosiveEnergy);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, ServerProjectile);



}
