
// Author: Jonas Byström
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

	if (mExplosiveEnergy)
	{
		Life::ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), Vector3DF(), mExplosiveEnergy);
	}
}



void FastProjectile::OnLoaded()
{
	Parent::OnLoaded();

	const TBC::ChunkyClass::Tag* lTag = FindTag(_T("ammo"), 4, 2);
	assert(lTag);

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
		if (Life::ProjectileUtil::GetBarrel(this, lParentTransform, lParentVelocity))
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
	Life::ProjectileUtil::StartBullet(this, pMuzzleVelocity, !lIsSynchronized);

	if (lIsSynchronized)
	{
		// Move mesh to muzzle and let it lerp towards object.
		TransformationF lTransform;
		Vector3DF lVelocity;
		Life::ProjectileUtil::GetBarrel(this, lTransform, lVelocity);
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

	//const TBC::ChunkyBoneGeometry* lRootGeometry = GetPhysics()->GetBoneGeometry(0);
	//TBC::PhysicsManager::BodyID lBody = lRootGeometry->GetTriggerId();
	//TransformationF lTransform;
	//GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lBody, lTransform);
	//static QuaternionF q;
	//static int lFrame = 0;
	//++lFrame;
	//if ((q - lTransform.GetOrientation()).GetNorm() >= 0.01f)
	//{
	//	mLog.Infof(_T("Fast projectile BEFORE bullet tick (ID %u, frame %i): (%f;%f;%f;%f)."), GetInstanceId(), lFrame, lTransform.GetOrientation().GetA(), lTransform.GetOrientation().GetB(), lTransform.GetOrientation().GetC(), lTransform.GetOrientation().GetD());
	//	q = lTransform.GetOrientation();
	//}
	Life::ProjectileUtil::BulletMicroTick(this, pFrameTime, mMaxVelocity, mAcceleration);
	//GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lBody, lTransform);
	//if ((q - lTransform.GetOrientation()).GetNorm() >= 0.01f)
	//{
	//	mLog.Infof(_T("Fast projectile AFTER bullet tick (ID %u, frame %i): (%f;%f;%f;%f)."), GetInstanceId(), lFrame, lTransform.GetOrientation().GetA(), lTransform.GetOrientation().GetB(), lTransform.GetOrientation().GetC(), lTransform.GetOrientation().GetD());
	//	q = lTransform.GetOrientation();
	//}
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
		Life::ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), pNormal, mExplosiveEnergy);
	}
	else
	{
		Life::ProjectileUtil::OnBulletHit(this, &mIsDetonated, mLauncher, pBody);
	}
}

void FastProjectile::LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource)
{
	assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), GetPosition(), GetVelocity());
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0.7f, 1.0);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, FastProjectile);



}
