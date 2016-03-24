
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Grenade.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/TimeManager.h"
#include "../Tbc/Include/ChunkyBoneGeometry.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiProps.h"
#include "Game.h"



namespace GrenadeRun
{



Grenade::Grenade(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, float pMuzzleVelocity):
	Parent(pResourceManager, pClassId, pUiManager),
	mShreekSound(0),
	mLaunchSound(0),
	mTimeFrameCreated(-1),
	mMuzzleVelocity(pMuzzleVelocity),
	mIsLaunched(false),
	mExploded(false),
	mIsUserFired(false)
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



bool Grenade::IsUserFired() const
{
	return mIsUserFired;
}



void Grenade::Launch()
{
	mIsLaunched = true;

	mShreekSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
	mShreekSound->Load(GetResourceManager(), _T("incoming.wav"),
		UiCure::UserSound3dResource::TypeLoadCallback(this, &Grenade::LoadPlaySound3d));

	mLaunchSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
	mLaunchSound->Load(GetResourceManager(), _T("launch.wav"),
		UiCure::UserSound3dResource::TypeLoadCallback(this, &Grenade::LoadPlaySound3d));

	const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	GetManager()->GetGameManager()->GetPhysicsManager()->EnableGravity(lGeometry->GetBodyId(), true);
	vec3 lVelocity = GetOrientation() * vec3(0, 0, mMuzzleVelocity);
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetBodyId(), lVelocity);

	((Game*)GetManager()->GetGameManager())->FreeLauncherBarrel();
	mIsUserFired = (((Game*)GetManager()->GetGameManager())->GetComputerIndex() != 1);
}

void Grenade::OnTick()
{
	if (mShreekSound && mShreekSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		const vec3 lPosition = GetPosition();
		vec3 lVelocity = GetVelocity();
		mUiManager->GetSoundManager()->SetSoundPosition(mShreekSound->GetData(), lPosition, lVelocity);
		if (lVelocity.GetLengthSquared() > 1*1)
		{
			Tbc::PhysicsManager::BodyID lBodyId = mPhysics->GetBoneGeometry(0)->GetBodyId();
			const float l = lVelocity.GetLength();
			const float lPitch = -lVelocity.GetAngle(vec3(0,0,l));
			vec2 lXY = vec2(lVelocity.x, lVelocity.y);
			const float l2 = lXY.GetLength();
			if (l2)
			{
				const float lYaw = lXY.GetAngle(vec2(0,l2));
				quat q;
				//q.SetEulerAngles(lYaw, lPitch, 0);
				q.RotateAroundWorldZ(lPosition.z/100);
				q.RotateAroundWorldX(lPitch);
				q.RotateAroundWorldZ(-lYaw);
				xform t;
				t.GetOrientation() = q * mPhysics->GetOriginalBoneTransformation(0).GetOrientation();
				t.SetPosition(lPosition);
				GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(lBodyId, t);
			}
		}
	}
	if (mLaunchSound && mLaunchSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		vec3 lPosition;
		vec3 lVelocity;
		((Game*)GetManager()->GetGameManager())->GetVehicleMotion(lPosition, lVelocity);
		mUiManager->GetSoundManager()->SetSoundPosition(mLaunchSound->GetData(), lPosition, lVelocity);
	}
	if (!mIsLaunched && mTimeFrameCreated >= 0)
	{
		xform lTransform;
		((Game*)GetManager()->GetGameManager())->GetLauncherTransform(lTransform);
		const Cure::TimeManager* lTimeManager = GetManager()->GetGameManager()->GetTimeManager();
		const float lTime = lTimeManager->ConvertPhysicsFramesToSeconds(lTimeManager->GetCurrentPhysicsFrameDelta(mTimeFrameCreated));
		const float lLauncherLength = 1.1f;
		float lRealTimeRatio;
		v_get(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
		float h = lLauncherLength/2+3 - lTime*lTime*4.0f*lRealTimeRatio;
		h = std::max(-lLauncherLength/2, h);
		const vec3 lFalling = lTransform.GetOrientation() * vec3(0, 0, h);
		lTransform.GetPosition() += lFalling;
		lTransform.GetOrientation() *= GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation().GetInverse();
		GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(GetPhysics()->GetBoneGeometry(0)->GetBodyId(), lTransform);
		if (h <= -lLauncherLength/2+0.1f)
		{
			Launch();
		}
	}
	if (GetPosition().z <= -300)
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
	Parent::OnTick();
}

bool Grenade::TryComplete()
{
	bool lOk = Parent::TryComplete();
	if (mIsLoaded)
	{
		GetPhysics()->EnableGravity(GetManager()->GetGameManager()->GetPhysicsManager(), false);
		mTimeFrameCreated = GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrame();
	}
	return lOk;
}

void Grenade::OnForceApplied(Cure::ContextObject* pOtherObject,
	Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
	const vec3& pForce, const vec3& pTorque,
	const vec3& pPosition, const vec3& pRelativeVelocity)
{
	(void)pOwnBodyId;
	(void)pOtherBodyId;
	(void)pRelativeVelocity;

	if (mExploded)
	{
		return;
	}
	mExploded = true;

	GetManager()->PostKillObject(GetInstanceId());
	((Game*)GetManager()->GetGameManager())->Detonate(pForce, pTorque, pPosition,
		this, (Cure::ContextObject*)pOtherObject, pOwnBodyId, pOtherBodyId);
}

void Grenade::LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource)
{
	deb_assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		if (pSoundResource == mShreekSound)
		{
			mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), GetPosition(), GetVelocity());
			mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0.7f, 1.0);
		}
		else
		{
			vec3 lPosition;
			vec3 lVelocity;
			((Game*)GetManager()->GetGameManager())->GetVehicleMotion(lPosition, lVelocity);
			mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), lPosition, lVelocity);
			mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 5.0f, 1.0);
		}
	}
}



loginstance(GAME_CONTEXT_CPP, Grenade);



}
