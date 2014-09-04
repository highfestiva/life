
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "ServerFastProjectile.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../Launcher.h"
#include "../ProjectileUtil.h"



namespace Life
{



ServerFastProjectile::ServerFastProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, Launcher* pLauncher):
	Parent(pResourceManager, pClassId),
	mLauncher(pLauncher),
	mMaxVelocity(0),
	mAcceleration(0),
	mExplosiveEnergy(0),
	mIsDetonated(false)
{
}

ServerFastProjectile::~ServerFastProjectile()
{
}



void ServerFastProjectile::OnLoaded()
{
	Parent::OnLoaded();

	const Tbc::ChunkyClass::Tag* lTag = FindTag(_T("ammo"), 4, 2);
	deb_assert(lTag);
	const float lMuzzleVelocity = lTag->mFloatValueList[0];
	ProjectileUtil::StartBullet(this, lMuzzleVelocity, true);
	mMaxVelocity = lTag->mFloatValueList[1];
	mAcceleration = lTag->mFloatValueList[2];
	mExplosiveEnergy = lTag->mFloatValueList[3];
}

void ServerFastProjectile::OnMicroTick(float pFrameTime)
{
	Parent::OnMicroTick(pFrameTime);

	ProjectileUtil::BulletMicroTick(this, pFrameTime, mMaxVelocity, mAcceleration);
}

void ServerFastProjectile::OnTrigger(Tbc::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pNormal)
{
	(void)pTriggerId;
	(void)pBodyId;
	if (mExplosiveEnergy)
	{
		ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), pNormal, mExplosiveEnergy, 0);
	}
	else
	{
		ProjectileUtil::OnBulletHit(this, &mIsDetonated, mLauncher, pOtherObject);
	}
}



loginstance(GAME_CONTEXT_CPP, ServerFastProjectile);



}
