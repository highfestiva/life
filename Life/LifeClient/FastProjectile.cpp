
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
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
	EnableRootShadow(false);
	EnableMeshSlide(false);
}

FastProjectile::~FastProjectile()
{
	delete mShreekSound;
	mShreekSound = 0;

	if (mExplosiveEnergy && GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
	{
		ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), vec3(), mExplosiveEnergy, 0);
	}
}



void FastProjectile::OnLoaded()
{
	Parent::OnLoaded();

	const Tbc::ChunkyClass::Tag* lTag = FindTag("ammo", 4, -1);
	deb_assert(lTag);

	const float lMuzzleVelocity = lTag->mFloatValueList[0];
	StartBullet(lMuzzleVelocity);
	mMaxVelocity = lTag->mFloatValueList[1];
	mAcceleration = lTag->mFloatValueList[2];
	mExplosiveEnergy = lTag->mFloatValueList[3];

	str lLaunchSoundName;
	str lShreekSoundName;
	const float lPitch = ProjectileUtil::GetShotSounds(GetManager(), lTag->mStringValueList, lLaunchSoundName, lShreekSoundName);
	if (!lLaunchSoundName.empty())
	{
		xform lParentTransform;
		vec3 lParentVelocity;
		if (ProjectileUtil::GetBarrel(this, lParentTransform, lParentVelocity))
		{
			UiCure::UserSound3dResource* lLaunchSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
			new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetManager(), lLaunchSoundName, lLaunchSound, lParentTransform.GetPosition(), lParentVelocity, 5.0f, lPitch);
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
		xform lTransform;
		vec3 lVelocity;
		ProjectileUtil::GetBarrel(this, lTransform, lVelocity);
		for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
		{
			UiCure::UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
			Tbc::GeometryBase* lGfxGeometry = lResource->GetRamData();
			lGfxGeometry->SetTransformation(lTransform);
		}
		EnableMeshSlide(true);
		ActivateLerp();
	}
}

void FastProjectile::OnTick()
{
	Parent::OnTick();

	const vec3 lPosition = GetPosition();
	if (mShreekSound && mShreekSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		const vec3 lVelocity = GetVelocity();
		mUiManager->GetSoundManager()->SetSoundPosition(mShreekSound->GetData(), lPosition, lVelocity);
	}
	/*if (lPosition.GetLengthSquared() > 3000*3000)
	{
		GetManager()->PostKillObject(GetInstanceId());
	}*/
}

void FastProjectile::OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal)
{
	(void)pTriggerId;
	(void)pBodyId;
	(void)pPosition;

	if (++mTickCount < 10 && pOtherObject->GetInstanceId() == GetOwnerInstanceId())	// Disallow self-hit during the first few frames.
	{
		return;
	}

	if (mExplosiveEnergy)
	{
		ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), pNormal, mExplosiveEnergy, 0);
	}
	else
	{
		ProjectileUtil::OnBulletHit(this, &mIsDetonated, mLauncher, pOtherObject);
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



loginstance(GAME_CONTEXT_CPP, FastProjectile);



}
