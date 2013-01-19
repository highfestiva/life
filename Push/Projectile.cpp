
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Projectile.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiSoundReleaser.h"
#include "../Life/Launcher.h"
#include "../Life/ProjectileUtil.h"



namespace Push
{



Projectile::Projectile(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager),
	mShreekSound(0),
	mLauncher(pLauncher),
	mTickCounter(0),
	mIsDetonated(false)
{
	DisableRootShadow();
}

Projectile::~Projectile()
{
	delete mShreekSound;
	mShreekSound = 0;
	Life::ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition());
}



void Projectile::OnLoaded()
{
	Parent::OnLoaded();

#ifdef LEPRA_DEBUG
	//Vector3DF lPos = GetPosition();
	//log_volatile(mLog.Infof(_T("Projectile loaded at pos (%f;%f;%f)."), lPos.x, lPos.y, lPos.z));
#endif // Debug

	mShreekSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
	mShreekSound->Load(GetResourceManager(), _T("incoming.wav"),
		UiCure::UserSound3dResource::TypeLoadCallback(this, &Projectile::LoadPlaySound3d));

	UiCure::UserSound3dResource* lLaunchSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
	lLaunchSound->Load(GetResourceManager(), _T("launch.wav"),
		UiCure::UserSound3dResource::TypeLoadCallback(this, &Projectile::LoadPlaySound3d));
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
	(void)pRelativeVelocity;

	if (mTickCounter < 3 && pOtherObject->GetInstanceId() == GetOwnerInstanceId())	// Don't detonate on oneself immediately.
	{
		return;
	}

	Life::ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, pPosition);
}

void Projectile::LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource)
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



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Projectile);



}
