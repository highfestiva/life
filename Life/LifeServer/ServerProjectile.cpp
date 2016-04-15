
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "ServerProjectile.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../Launcher.h"
#include "../ProjectileUtil.h"



namespace Life
{



ServerProjectile::ServerProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, float pMuzzleVelocity, Launcher* pLauncher):
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

	const Tbc::ChunkyClass::Tag* lTag = FindTag("ammo", 4, -1);
	deb_assert(lTag);
	mExplosiveEnergy = lTag->mFloatValueList[3];

	xform lTransform;
	vec3 lParentVelocity;
	ProjectileUtil::GetBarrel(this, lTransform, lParentVelocity);
	vec3 lVelocity = lTransform.GetOrientation() * vec3(0, 0, mMuzzleVelocity);
	lVelocity += lParentVelocity;
	lTransform.GetPosition() += lTransform.GetOrientation() * vec3(0, 0, +3);
	const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(lGeometry->GetBodyId(), lTransform);
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
}

void ServerProjectile::OnForceApplied(Cure::ContextObject* pOtherObject,
	Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
	const vec3& pForce, const vec3& pTorque,
	const vec3& pPosition, const vec3& pRelativeVelocity)
{
	(void)pOtherObject;
	(void)pOtherBodyId;
	(void)pOwnBodyId;
	(void)pForce;
	(void)pTorque;
	(void)pRelativeVelocity;

	ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, pPosition, pRelativeVelocity, vec3(), mExplosiveEnergy, 0);
}



loginstance(GAME_CONTEXT_CPP, ServerProjectile);



}
