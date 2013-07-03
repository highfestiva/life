
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	DisableRootShadow();
}

Projectile::~Projectile()
{
	delete mShreekSound;
	mShreekSound = 0;
	ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), Vector3DF(), mExplosiveEnergy);
}



void Projectile::OnLoaded()
{
	Parent::OnLoaded();

	const TBC::ChunkyClass::Tag* lTag = FindTag(_T("ammo"), 1, 2);
	deb_assert(lTag);
	mExplosiveEnergy = lTag->mFloatValueList[0];

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
			UiCure::UserSound3dResource::TypeLoadCallback(this, &Projectile::LoadPlaySound3d));
	}
}

void Projectile::OnTick()
{
	++mTickCounter;
	if (mShreekSound && mShreekSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		// Point the projectile in the velocity direction.
		const Vector3DF lPosition = GetPosition();
		Vector3DF lVelocity = GetVelocity();
		mUiManager->GetSoundManager()->SetSoundPosition(mShreekSound->GetData(), lPosition, lVelocity);
	}
	Parent::OnTick();
}

void Projectile::OnForceApplied(ContextObject* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
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

	ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, pPosition, pRelativeVelocity, Vector3DF(), mExplosiveEnergy);
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



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Projectile);



}
