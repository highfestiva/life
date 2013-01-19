
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "ServerFastProjectile.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Life/Launcher.h"
#include "../../Life/ProjectileUtil.h"



namespace Push
{



ServerFastProjectile::ServerFastProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, float pMuzzleVelocity, Life::Launcher* pLauncher):
	Parent(pResourceManager, pClassId),
	mMuzzleVelocity(pMuzzleVelocity),
	mLauncher(pLauncher),
	mIsDetonated(false)
{
}

ServerFastProjectile::~ServerFastProjectile()
{
}



void ServerFastProjectile::OnLoaded()
{
	Parent::OnLoaded();

	Life::ProjectileUtil::StartBullet(this, mMuzzleVelocity);
}

void ServerFastProjectile::OnMicroTick(float pFrameTime)
{
	Parent::OnMicroTick(pFrameTime);

	Life::ProjectileUtil::BulletMicroTick(this, pFrameTime);
}

void ServerFastProjectile::OnTick()
{
	Parent::OnTick();

	if (IsLoaded())
	{
		const Vector3DF lPosition = GetPosition();
		if (lPosition.GetLengthSquared() > 3000*3000)
		{
			GetManager()->PostKillObject(GetInstanceId());
		}
	}
}

void ServerFastProjectile::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody)
{
	(void)pTriggerId;

	Life::ProjectileUtil::OnBulletHit(this, &mIsDetonated, mLauncher, pBody);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, ServerFastProjectile);



}
