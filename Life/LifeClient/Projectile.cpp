
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Projectile.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiSoundReleaser.h"
#include "../Launcher.h"
#include "../ProjectileUtil.h"



namespace Life
{



Projectile::Projectile(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager),
	mShreekSound(0),
	mLauncher(pLauncher),
	mTickCounter(0),
	mIsDetonated(false),
	mExplosiveEnergy(1)
{
	EnableRootShadow(false);
}

Projectile::~Projectile()
{
	delete mShreekSound;
	mShreekSound = 0;
	ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), vec3(), mExplosiveEnergy, -1);
}



void Projectile::OnLoaded()
{
	Parent::OnLoaded();

	const Tbc::ChunkyClass::Tag* lTag = FindTag("ammo", 4, -1);
	deb_assert(lTag);
	mExplosiveEnergy = lTag->mFloatValueList[3];

	str lLaunchSoundName;
	str lShreekSoundName;
	const float lPitch = ProjectileUtil::GetShotSounds(GetManager(), lTag->mStringValueList, lLaunchSoundName, lShreekSoundName);
	if (!lLaunchSoundName.empty())
	{
		xform lParentTransform;
		vec3 lParentVelocity;
		if (!ProjectileUtil::GetBarrel(this, lParentTransform, lParentVelocity))
		{
			lParentTransform.SetPosition(GetPosition());
			lParentVelocity = GetVelocity();
		}
		UiCure::UserSound3dResource* lLaunchSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
		new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetManager(), lLaunchSoundName, lLaunchSound, lParentTransform.GetPosition(), lParentVelocity, 5.0f, lPitch);
	}
	if (!lShreekSoundName.empty())
	{
		mShreekSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
		mShreekSound->Load(GetResourceManager(), lShreekSoundName,
			UiCure::UserSound3dResource::TypeLoadCallback(this, &Projectile::LoadPlaySound3d));
	}
}

void Projectile::OnTick()
{
	++mTickCounter;
	if (mShreekSound && mShreekSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		// Point the projectile in the velocity direction.
		const vec3 lPosition = GetPosition();
		vec3 lVelocity = GetVelocity();
		mUiManager->GetSoundManager()->SetSoundPosition(mShreekSound->GetData(), lPosition, lVelocity);
	}
	Parent::OnTick();
}

void Projectile::OnForceApplied(ContextObject* pOtherObject,
	Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
	const vec3& pForce, const vec3& pTorque,
	const vec3& pPosition, const vec3& pRelativeVelocity)
{
	(void)pOwnBodyId;
	(void)pOtherBodyId;
	(void)pForce;
	(void)pTorque;
	(void)pPosition;

	if (mTickCounter < 3 && pOtherObject->GetInstanceId() == GetOwnerInstanceId())	// Don't detonate on oneself immediately.
	{
		return;
	}

	ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, pPosition, pRelativeVelocity, vec3(), mExplosiveEnergy, 0);
}

void Projectile::LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource)
{
	deb_assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), GetPosition(), GetVelocity());
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0.7f, 1.0);
	}
}



loginstance(GAME_CONTEXT_CPP, Projectile);



}
