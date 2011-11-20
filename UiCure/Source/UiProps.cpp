
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/UiProps.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Random.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



Props::Props(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mParticleType(PARTICLE_NONE),
	mScale(1),
	mTime(0),
	mLifeTime(2),
	mFadeOutTime(0.6f),
	mOpacity(1),
	mIsFadingOut(false)
{
	EnablePixelShader(false);
	SetPhysicsTypeOverride(PHYSICS_OVERRIDE_BONES);
}

Props::~Props()
{
}



void Props::SetOpacity(float pOpacity)
{
	mOpacity = pOpacity;
}

void Props::StartParticle(ParticleType pParticleType, const Vector3DF& pStartVelocity, float pScale, float pAngularRange, float pTime)
{
	//assert(pStartVelocity.GetLengthSquared() < 1000*1000);
	mParticleType = pParticleType;
	mVelocity = pStartVelocity;
	mScale = pScale;
	mAngularVelocity.Set((float)Random::Uniform(-pAngularRange, pAngularRange),
		(float)Random::Uniform(-pAngularRange, pAngularRange),
		(float)Random::Uniform(-pAngularRange*0.1f, pAngularRange*0.1f));
	mLifeTime = pTime;
	GetManager()->AddAlarmCallback(this, 5, mLifeTime, 0);
}

void Props::SetFadeOutTime(float pTime)
{
	mFadeOutTime = pTime;
}


void Props::DispatchOnLoadMesh(UserGeometryReferenceResource* pMeshResource)
{
	Parent::DispatchOnLoadMesh(pMeshResource);
	if (mParticleType != PARTICLE_NONE && pMeshResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		pMeshResource->GetRamData()->SetIsSimpleObject();
		pMeshResource->GetRamData()->SetScale((float)Random::Uniform(0.4, 1.6) * mScale);
	}
}

void Props::TryAddTexture()
{
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		if (mMeshResourceArray[x]->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
		{
			if (mParticleType == PARTICLE_GAS)
			{
				mMeshResourceArray[x]->GetRamData()->GetBasicMaterialSettings().mAlpha = 0.01f;
			}
			else if (mParticleType == PARTICLE_SOLID)
			{
				mMeshResourceArray[x]->GetRamData()->GetBasicMaterialSettings().mAlpha = mOpacity;
			}
		}
	}
	Parent::TryAddTexture();
}

void Props::OnTick()
{
	const float lFrameTime = GetManager()->GetGameManager()->GetTimeManager()->GetNormalFrameTime();
	mTime += lFrameTime;
	switch (mParticleType)
	{
		case PARTICLE_SOLID:
		{
			SetRootPosition(GetPosition() + mVelocity*lFrameTime);
			mVelocity.z -= 9.82f*lFrameTime;

			QuaternionF lOrientation = GetOrientation();
			lOrientation.RotateAroundOwnX(lFrameTime * mAngularVelocity.x * mVelocity.x);
			lOrientation.RotateAroundOwnY(lFrameTime * mAngularVelocity.y * mVelocity.y);
			lOrientation.RotateAroundOwnZ(lFrameTime * mAngularVelocity.z * mVelocity.z);
			SetRootOrientation(lOrientation);

			const float lTimeLeft = mLifeTime-mTime;
			if (lTimeLeft < mFadeOutTime)
			{
				const float lFadeOutOpacity = mOpacity * lTimeLeft / mFadeOutTime;
				for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
				{
					UserGeometryReferenceResource* lMesh = mMeshResourceArray[x];
					if (!mIsFadingOut && mOpacity >= 1)
					{
						mUiManager->GetRenderer()->ChangeMaterial(lMesh->GetData(), UiTbc::Renderer::MAT_SINGLE_COLOR_BLENDED);
					}
					lMesh->GetRamData()->GetBasicMaterialSettings().mAlpha = lFadeOutOpacity;
				}
				mIsFadingOut = true;
			}
		}
		break;
		case PARTICLE_GAS:
		{
			const float lLerp = Math::GetIterateLerpTime(0.08f, lFrameTime);
			mVelocity.x = Math::Lerp(mVelocity.x, 0.0f, lLerp);
			mVelocity.y = Math::Lerp(mVelocity.y, 0.0f, lLerp);
			mVelocity.z = Math::Lerp(mVelocity.z, 3.0f, lLerp);
			SetRootPosition(GetPosition() + mVelocity*lFrameTime);

			QuaternionF lOrientation = GetOrientation();
			lOrientation.RotateAroundOwnX(lFrameTime * mAngularVelocity.x * mVelocity.x);
			lOrientation.RotateAroundOwnY(lFrameTime * mAngularVelocity.y * mVelocity.y);
			lOrientation.RotateAroundOwnZ(lFrameTime * mAngularVelocity.z * mVelocity.z);
			SetRootOrientation(lOrientation);

			const float lGasOpacity = mOpacity * sin(mTime/mLifeTime*PIF);
			for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
			{
				if (mMeshResourceArray[x]->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
				{
					mMeshResourceArray[x]->GetRamData()->GetBasicMaterialSettings().mAlpha = lGasOpacity;
				}
			}
		}
		break;
	}

	Parent::OnTick();	// TRICKY: not a vehicle in this sense.
}

void Props::OnAlarm(int pAlarmId, void* /*pExtraData*/)
{
	if (pAlarmId == 5)
	{
		assert(mParticleType != PARTICLE_NONE);
		GetManager()->PostKillObject(GetInstanceId());
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Props);



}
