
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "FastProjectile.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiSoundReleaser.h"
#include "../Launcher.h"
#include "../ProjectileUtil.h"



namespace Life
{



FastProjectile::FastProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager),
	mShreekSound(0),
	mLauncher(pLauncher),
	mMaxVelocity(0),
	mAcceleration(0),
	mExplosiveEnergy(0),
	mTickCount(0),
	mIsDetonated(false)
{
	DisableRootShadow();
	EnableMeshSlide(false);
}

FastProjectile::~FastProjectile()
{
	delete mShreekSound;
	mShreekSound = 0;

	if (mExplosiveEnergy && GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
	{
		ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), Vector3DF(), mExplosiveEnergy, 0);
	}
}



void FastProjectile::OnLoaded()
{
	Parent::OnLoaded();

	const TBC::ChunkyClass::Tag* lTag = FindTag(_T("ammo"), 4, 2);
	deb_assert(lTag);

	const float lMuzzleVelocity = lTag->mFloatValueList[0];
	StartBullet(lMuzzleVelocity);
	mMaxVelocity = lTag->mFloatValueList[1];
	mAcceleration = lTag->mFloatValueList[2];
	mExplosiveEnergy = lTag->mFloatValueList[3];

	const str lLaunchSoundName = lTag->mStringValueList[0];
	const str lShreekSoundName = lTag->mStringValueList[1];
	if (!lLaunchSoundName.empty())
	{
		TransformationF lParentTransform;
		Vector3DF lParentVelocity;
		if (ProjectileUtil::GetBarrel(this, lParentTransform, lParentVelocity))
		{
			UiCure::UserSound3dResource* lLaunchSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
			new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetManager(), lLaunchSoundName, lLaunchSound, lParentTransform.GetPosition(), lParentVelocity, 5.0f, 1.0f);
		}
	}
	if (!lShreekSoundName.empty())
	{
		mShreekSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
		mShreekSound->Load(GetResourceManager(), lShreekSoundName,
			UiCure::UserSound3dResource::TypeLoadCallback(this, &FastProjectile::LoadPlaySound3d));
	}
}

void FastProjectile::StartBullet(float pMuzzleVelocity)
{
	const bool lIsSynchronized = !GetManager()->IsLocalGameObjectId(GetInstanceId());
	const bool lHasBarrel = (GetOwnerInstanceId() != 0);
	ProjectileUtil::StartBullet(this, pMuzzleVelocity, !lIsSynchronized && lHasBarrel);

	if (lIsSynchronized && lHasBarrel)
	{
		// Move mesh to muzzle and let it lerp towards object.
		TransformationF lTransform;
		Vector3DF lVelocity;
		ProjectileUtil::GetBarrel(this, lTransform, lVelocity);
		for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
		{
			UiCure::UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
			TBC::GeometryBase* lGfxGeometry = lResource->GetRamData();
			lGfxGeometry->SetTransformation(lTransform);
		}
		EnableMeshSlide(true);
		ActivateLerp();
	}
}

void FastProjectile::OnMicroTick(float pFrameTime)
{
	Parent::OnMicroTick(pFrameTime);

	ProjectileUtil::BulletMicroTick(this, pFrameTime, mMaxVelocity, mAcceleration);
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

void FastProjectile::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal)
{
	(void)pTriggerId;

	if (++mTickCount < 10 && pBody->GetInstanceId() == GetOwnerInstanceId())	// Disallow self-hit during the first few frames.
	{
		return;
	}

	if (mExplosiveEnergy)
	{
		ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), pNormal, mExplosiveEnergy, 0);
	}
	else
	{
		ProjectileUtil::OnBulletHit(this, &mIsDetonated, mLauncher, pBody);
	}
}

void FastProjectile::LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource)
{
	deb_assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), GetPosition(), GetVelocity());
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0.7f, 1.0);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, FastProjectile);



}
