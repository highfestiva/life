
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "ServerFastProjectile.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Life/Launcher.h"
#include "../../Life/ProjectileUtil.h"



namespace Push
{



ServerFastProjectile::ServerFastProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, Life::Launcher* pLauncher):
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

	const TBC::ChunkyClass::Tag* lTag = FindTag(_T("ammo"), 4, 2);
	assert(lTag);
	const float lMuzzleVelocity = lTag->mFloatValueList[0];
	Life::ProjectileUtil::StartBullet(this, lMuzzleVelocity, true);
	mMaxVelocity = lTag->mFloatValueList[1];
	mAcceleration = lTag->mFloatValueList[2];
	mExplosiveEnergy = lTag->mFloatValueList[3];
}

void ServerFastProjectile::OnMicroTick(float pFrameTime)
{
	Parent::OnMicroTick(pFrameTime);

	Life::ProjectileUtil::BulletMicroTick(this, pFrameTime, mMaxVelocity, mAcceleration);
}

void ServerFastProjectile::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody)
{
	(void)pTriggerId;
	if (mExplosiveEnergy)
	{
		Life::ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), mExplosiveEnergy);
	}
	else
	{
		Life::ProjectileUtil::OnBulletHit(this, &mIsDetonated, mLauncher, pBody);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, ServerFastProjectile);



}
