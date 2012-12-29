
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Grenade.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../UiCure/Include/UiGameUiManager.h"



namespace Push
{



Grenade::Grenade(Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, float pMuzzleVelocity, Launcher* pLauncher):
	Parent(pResourceManager, _T("grenade"), pUiManager),
	mShreekSound(0),
	mLaunchSound(0),
	mTimeFrameCreated(-1),
	mMuzzleVelocity(pMuzzleVelocity),
	mLauncher(pLauncher),
	mIsLaunched(false),
	mIsExploded(false)
{
	SetForceLoadUnique(true);	// Needs to be unique as physics are reloaded often with shared IDs.
}

Grenade::~Grenade()
{
	delete mLaunchSound;
	mLaunchSound = 0;
	delete mShreekSound;
	mShreekSound = 0;
}



void Grenade::Launch()
{
	mShreekSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
	mShreekSound->Load(GetResourceManager(), _T("incoming.wav"),
		UiCure::UserSound3dResource::TypeLoadCallback(this, &Grenade::LoadPlaySound3d));

	mLaunchSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
	mLaunchSound->Load(GetResourceManager(), _T("launch.wav"),
		UiCure::UserSound3dResource::TypeLoadCallback(this, &Grenade::LoadPlaySound3d));

	TransformationF lTransform;
	Vector3DF lParentVelocity;
	mLauncher->GetBarrel(lTransform, lParentVelocity);
	Vector3DF lVelocity = lTransform.GetOrientation() * Vector3DF(0, mMuzzleVelocity, 0);
	lVelocity += lParentVelocity;
	lTransform.GetPosition() += lTransform.GetOrientation() * Vector3DF(0, 3, 0);
	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(lGeometry->GetBodyId(), lTransform);
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
}

void Grenade::OnTick()
{
	if (mShreekSound && mShreekSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		// Point the grenade in the velocity direction.
		const Vector3DF lPosition = GetPosition();
		Vector3DF lVelocity = GetVelocity();
		mUiManager->GetSoundManager()->SetSoundPosition(mShreekSound->GetData(), lPosition, lVelocity);
		if (lVelocity.GetLengthSquared() > 1*1)
		{
			TBC::PhysicsManager::BodyID lBodyId = mPhysics->GetBoneGeometry(0)->GetBodyId();
			const float l = lVelocity.GetLength();
			const float lPitch = -lVelocity.GetAngle(Vector3DF(0,0,l));
			Vector2DF lXY = Vector2DF(lVelocity.x, lVelocity.y);
			const float l2 = lXY.GetLength();
			if (l2)
			{
				const float lYaw = lXY.GetAngle(Vector2DF(0,l2));
				QuaternionF q;
				//q.SetEulerAngles(lYaw, lPitch, 0);
				q.RotateAroundWorldZ(lPosition.z/100);
				q.RotateAroundWorldX(lPitch);
				q.RotateAroundWorldZ(-lYaw);
				TransformationF t;
				t.GetOrientation() = q * mPhysics->GetOriginalBoneTransformation(0).GetOrientation();
				t.SetPosition(lPosition);
				GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(lBodyId, t);
			}
		}
	}
	if (mLaunchSound && mLaunchSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		// Update launch sound position.
		TransformationF lParentTransform;
		Vector3DF lParentVelocity;
		mLauncher->GetBarrel(lParentTransform, lParentVelocity);
		mUiManager->GetSoundManager()->SetSoundPosition(mLaunchSound->GetData(), lParentTransform.GetPosition(), lParentVelocity);
	}
	if (!mIsLaunched && IsLoaded())
	{
		mIsLaunched = true;
		Launch();
	}
	if (GetPosition().z <= -300)
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
	Parent::OnTick();
}

void Grenade::OnForceApplied(Cure::ContextObject* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	(void)pOwnBodyId;
	(void)pOtherBodyId;
	(void)pRelativeVelocity;

	if (mIsExploded)
	{
		return;
	}
	mIsExploded = true;

	GetManager()->PostKillObject(GetInstanceId());
	mLauncher->Detonate(pForce, pTorque, pPosition, this, pOtherObject, pOwnBodyId, pOtherBodyId);
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
