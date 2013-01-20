
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#include "FastProjectile.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiSoundReleaser.h"
#include "../Life/Launcher.h"
#include "../Life/ProjectileUtil.h"



namespace Push
{



FastProjectile::FastProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager),
	mShreekSound(0),
	mLauncher(pLauncher),
	mIsDetonated(false)
{
	DisableRootShadow();
	EnableMeshSlide(false);
}

FastProjectile::~FastProjectile()
{
	delete mShreekSound;
	mShreekSound = 0;
}



void FastProjectile::OnLoaded()
{
	Parent::OnLoaded();

	const UiTbc::ChunkyClass::Tag* lTag = FindTag(_T("ammo"), 3, 2);
	assert(lTag);

	const float lMuzzleVelocity = lTag->mFloatValueList[0];
	Life::ProjectileUtil::StartBullet(this, lMuzzleVelocity);
	//const float lMaxVelocity = lTag->mFloatValueList[1];
	//const float lAcceleration = lTag->mFloatValueList[2];

	const str lLaunchSoundName = lTag->mStringValueList[0];
	const str lShreekSoundName = lTag->mStringValueList[1];
	if (!lLaunchSoundName.empty())
	{
		UiCure::UserSound3dResource* lLaunchSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
		lLaunchSound->Load(GetResourceManager(), lLaunchSoundName,
			UiCure::UserSound3dResource::TypeLoadCallback(this, &FastProjectile::LoadPlaySound3d));
	}
	if (!lShreekSoundName.empty())
	{
		mShreekSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
		mShreekSound->Load(GetResourceManager(), lShreekSoundName,
			UiCure::UserSound3dResource::TypeLoadCallback(this, &FastProjectile::LoadPlaySound3d));
	}
}

void FastProjectile::OnMicroTick(float pFrameTime)
{
	Parent::OnMicroTick(pFrameTime);

	Life::ProjectileUtil::BulletMicroTick(this, pFrameTime);
}

void FastProjectile::OnTick()
{
	Parent::OnTick();

	const Vector3DF lPosition = GetPosition();
	if (mShreekSound && mShreekSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		const Vector3DF lVelocity = GetVelocity();
		mUiManager->GetSoundManager()->SetSoundPosition(mShreekSound->GetData(), lPosition, lVelocity);
	}
	/*if (lPosition.GetLengthSquared() > 3000*3000)
	{
		GetManager()->PostKillObject(GetInstanceId());
	}*/
}

void FastProjectile::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody)
{
	(void)pTriggerId;
	Life::ProjectileUtil::OnBulletHit(this, &mIsDetonated, mLauncher, pBody);
}

void FastProjectile::LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource)
{
	assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		if (pSoundResource == mShreekSound)
		{
			mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), GetPosition(), GetVelocity());
			mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0.7f, 1.0);
		}
		else
		{
			TransformationF lParentTransform;
			Vector3DF lParentVelocity;
			Life::ProjectileUtil::GetBarrel(this, lParentTransform, lParentVelocity);
			mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), lParentTransform.GetPosition(), lParentVelocity);
			mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 5.0f, 1.0);
			new UiCure::SoundReleaser(mUiManager, GetManager(), pSoundResource);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, FastProjectile);



}
