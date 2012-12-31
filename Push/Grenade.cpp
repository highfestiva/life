
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Grenade.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "Launcher.h"



namespace Push
{



Grenade::Grenade(Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, Launcher* pLauncher):
	Parent(pResourceManager, _T("grenade"), pUiManager),
	mShreekSound(0),
	mLaunchSound(0),
	mLauncher(pLauncher),
	mDetonated(false)
{
	SetForceLoadUnique(true);	// Needs to be unique as physics are reloaded often with shared IDs.
}

Grenade::~Grenade()
{
	delete mLaunchSound;
	mLaunchSound = 0;
	delete mShreekSound;
	mShreekSound = 0;
	Detonate();
}



void Grenade::Detonate()
{
	if (mDetonated)
	{
		return;
	}
	mDetonated = true;

	TBC::ChunkyPhysics* lPhysics = GetPhysics();
	if (lPhysics)
	{
		mLauncher->Detonate(this, lPhysics->GetBoneGeometry(0));
	}
}

void Grenade::OnLoaded()
{
	Parent::OnLoaded();

	mShreekSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
	mShreekSound->Load(GetResourceManager(), _T("incoming.wav"),
		UiCure::UserSound3dResource::TypeLoadCallback(this, &Grenade::LoadPlaySound3d));

	mLaunchSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
	mLaunchSound->Load(GetResourceManager(), _T("launch.wav"),
		UiCure::UserSound3dResource::TypeLoadCallback(this, &Grenade::LoadPlaySound3d));
}

void Grenade::OnTick()
{
	if (mShreekSound && mShreekSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		// Point the grenade in the velocity direction.
		const Vector3DF lPosition = GetPosition();
		Vector3DF lVelocity = GetVelocity();
		mUiManager->GetSoundManager()->SetSoundPosition(mShreekSound->GetData(), lPosition, lVelocity);
	}
	if (mLaunchSound && mLaunchSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		// Update launch sound position.
		TransformationF lParentTransform;
		Vector3DF lParentVelocity;
		mLauncher->GetBarrel(lParentTransform, lParentVelocity);
		mUiManager->GetSoundManager()->SetSoundPosition(mLaunchSound->GetData(), lParentTransform.GetPosition(), lParentVelocity);
	}
	Parent::OnTick();
}

void Grenade::OnForceApplied(ContextObject* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	(void)pOtherObject;
	(void)pOwnBodyId;
	(void)pOtherBodyId;
	(void)pForce;
	(void)pTorque;
	(void)pPosition;
	(void)pRelativeVelocity;
	Detonate();
	GetManager()->PostKillObject(GetInstanceId());
}

void Grenade::LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource)
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
			mLauncher->GetBarrel(lParentTransform, lParentVelocity);
			mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), lParentTransform.GetPosition(), lParentVelocity);
			mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 5.0f, 1.0);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Grenade);



}
