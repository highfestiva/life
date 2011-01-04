
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Props.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Random.h"



namespace Life
{



Props::Props(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mParticleType(PARTICLE_NONE),
	mScale(1),
	mTime(0)
{
	SetPhysicsTypeOverride(PHYSICS_OVERRIDE_BONES);
}

Props::~Props()
{
}



void Props::StartParticle(ParticleType pParticleType, const Vector3DF& pStartVelocity, float pScale)
{
	assert(pStartVelocity.GetLengthSquared() < 1000*1000);
	mParticleType = pParticleType;
	mVelocity = pStartVelocity;
	mScale = pScale;
	GetManager()->AddAlarmCallback(this, 5, 2, 0);
}



void Props::DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource)
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
	if (mParticleType == PARTICLE_GAS)
	{
		for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
		{
			if (mMeshResourceArray[x]->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
			{
				mMeshResourceArray[x]->GetRamData()->GetBasicMaterialSettings().mAlpha = 0.01f;
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
		}
		break;
		case PARTICLE_GAS:
		{
			SetRootPosition(GetPosition() + mVelocity*lFrameTime);
			const float lLerp = Math::GetIterateLerpTime(0.08f, lFrameTime);
			mVelocity.x = Math::Lerp(mVelocity.x, 0.0f, lLerp);
			mVelocity.y = Math::Lerp(mVelocity.y, 0.0f, lLerp);
			mVelocity.z = Math::Lerp(mVelocity.z, 3.0f, lLerp);
			for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
			{
				if (mMeshResourceArray[x]->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
				{
					mMeshResourceArray[x]->GetRamData()->GetBasicMaterialSettings().mAlpha = 0.3f * sin(mTime*0.5f*PIF);
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


}
