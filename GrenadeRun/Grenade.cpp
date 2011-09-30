
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Grenade.h"
#include "../Cure/Include/ContextManager.h"
#include "../TBC/Include/ChunkyBoneGeometry.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiProps.h"
#include "Game.h"



namespace GrenadeRun
{



Grenade::Grenade(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mShreekSound(0),
	mLaunchSound(0),
	mExploded(false)
{
}

Grenade::~Grenade()
{
	delete mLaunchSound;
	mLaunchSound = 0;
	delete mShreekSound;
	mShreekSound = 0;
}



void Grenade::Start()
{
	GetManager()->AddAlarmCallback(this, 1, 1.1f, 0);
}



void Grenade::OnTick()
{
	if (mShreekSound && mShreekSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		const Vector3DF lPosition = GetPosition();
		Vector3DF lVelocity = GetVelocity();
		mUiManager->GetSoundManager()->SetSoundPosition(mShreekSound->GetData(), lPosition, lVelocity);
		if (lVelocity.GetLengthSquared() > 10*10)
		{
			TBC::PhysicsManager::BodyID lBodyId = mPhysics->GetBoneGeometry(0)->GetBodyId();
			const float l = lVelocity.GetLength();
			const float lPitch = -lVelocity.GetAngle(Vector3DF(0,0,l));
			Vector3DF lXY = Vector3DF(lVelocity.x, lVelocity.y, 0);
			const float l2 = lXY.GetLength();
			if (l2)
			{
				const float lYaw = lXY.GetAngle(Vector3DF(0,l2,0));
				QuaternionF q;
				//q.SetEulerAngles(lYaw, lPitch, 0);
				q.RotateAroundWorldZ(lPosition.z/100);
				q.RotateAroundWorldX(lPitch);
				q.RotateAroundWorldZ(lYaw);
				TransformationF t;
				t.GetOrientation() = q * mPhysics->GetOriginalBoneTransformation(0).GetOrientation();
				t.SetPosition(lPosition);
				GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(lBodyId, t);
			}
		}
	}
	if (mLaunchSound && mLaunchSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		Vector3DF lPosition;
		Vector3DF lVelocity;
		((Game*)GetManager()->GetGameManager())->GetVehicleMotion(lPosition, lVelocity);
		mUiManager->GetSoundManager()->SetSoundPosition(mLaunchSound->GetData(), lPosition, lVelocity);
	}
	Parent::OnTick();
}

void Grenade::OnAlarm(int pAlarmId, void*)
{
	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
	{
		switch (pAlarmId)
		{
			case 1:	// Has started falling, now stop grenade in launcher pipe and wait some time before firing.
			{
				GetManager()->AddAlarmCallback(this, 2, 1.5f, 0);
				GetManager()->GetGameManager()->GetPhysicsManager()->DeactivateGravity(lGeometry->GetBodyId());
				Vector3DF lVelocity;
				GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
			}
			break;
			case 2:	// Grenade has been stopped some in the pipe to simulate it falling all the way to the bottom, now fire it!
			{
				mShreekSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
				mShreekSound->Load(GetResourceManager(), _T("incoming.wav"),
					UiCure::UserSound3dResource::TypeLoadCallback(this, &Grenade::LoadPlaySound3d));

				mLaunchSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
				mLaunchSound->Load(GetResourceManager(), _T("launch.wav"),
					UiCure::UserSound3dResource::TypeLoadCallback(this, &Grenade::LoadPlaySound3d));

				GetManager()->GetGameManager()->GetPhysicsManager()->ActivateGravity(lGeometry->GetBodyId());
				Vector3DF lVelocity = GetOrientation() * Vector3DF(0, 0, 40);
				GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetBodyId(), lVelocity);

				GetManager()->AddAlarmCallback(this, 3, 0.5f, 0);
			}
			break;
			case 3:
			{
				((Game*)GetManager()->GetGameManager())->OnPostLaunchGrenade();
			}
			break;
		}
	}
}



void Grenade::OnForceApplied(TBC::PhysicsManager::ForceFeedbackListener* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	pOtherObject;
	pOwnBodyId;
	pOtherBodyId;
	pRelativeVelocity;

	if (mExploded)
	{
		return;
	}
	mExploded = true;

	GetManager()->PostKillObject(GetInstanceId());
	((Game*)GetManager()->GetGameManager())->Blast(pForce, pTorque, pPosition, this);
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
			Vector3DF lPosition;
			Vector3DF lVelocity;
			((Game*)GetManager()->GetGameManager())->GetVehicleMotion(lPosition, lVelocity);
			mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), lPosition, lVelocity);
			mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 5.0f, 1.0);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Grenade);



}
