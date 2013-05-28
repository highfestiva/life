
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiCollisionSoundManager.h"
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



#define MINIMUM_PLAYED_VOLUME_FACTOR	0.6f



CollisionSoundManager::CollisionSoundManager(Cure::GameManager* pGameManager, UiCure::GameUiManager* pUiManager):
	mGameManager(pGameManager),
	mUiManager(pUiManager)
{
	SetScale(50, 0.5f, 0.2f);
}

CollisionSoundManager::~CollisionSoundManager()
{
	mUiManager = 0;
	mGameManager = 0;
}

void CollisionSoundManager::SetScale(float pSmallMass, float pLightImpact, float pSoundCutoffDuration)
{
	mSmallMass = pSmallMass;
	mLightImpact = pLightImpact;
	mSoundCutoffDuration = pSoundCutoffDuration;
}

void CollisionSoundManager::AddSound(const str& pName, const SoundResourceInfo& pInfo)
{
	mSoundNameMap.insert(SoundNameMap::value_type(pName, pInfo));
}



void CollisionSoundManager::Tick(const Vector3DF& pCameraPosition)
{
	mCameraPosition = pCameraPosition;

	float lRealTimeRatio;
	CURE_RTVAR_GET(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);

	SoundMap::iterator x = mSoundMap.begin();
	while (x != mSoundMap.end())
	{
		SoundInfo* lSoundInfo = x->second;
		bool lIsPlaying = false;
		bool lOneIsPlayingOrLoading = false;
		if (lSoundInfo->mSound)
		{
			if (lSoundInfo->mSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
			{
				lIsPlaying = mUiManager->GetSoundManager()->IsPlaying(lSoundInfo->mSound->GetData());
				lOneIsPlayingOrLoading |= lIsPlaying;
			}
			else if (lSoundInfo->mSound->GetLoadState() == Cure::RESOURCE_LOAD_IN_PROGRESS)
			{
				lOneIsPlayingOrLoading = true;
			}
		}
		if (lOneIsPlayingOrLoading)
		{
			if (lIsPlaying && lRealTimeRatio != 1)
			{
				mUiManager->GetSoundManager()->SetPitch(lSoundInfo->mSound->GetData(), lSoundInfo->mPitch*lRealTimeRatio);
			}
			++x;
		}
		else
		{
			mSoundMap.erase(x++);
			delete lSoundInfo;
		}
	}
}

void CollisionSoundManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, float pMaxDistance, bool pIsLoud)
{
	if (!pObject1 || !pObject2)
	{
		return;
	}
	if (pPosition.GetDistanceSquared(mCameraPosition) > pMaxDistance*pMaxDistance)
	{
		return;
	}
	if (pObject1->GetVelocity().GetDistanceSquared(pObject2->GetVelocity()) < mLightImpact)
	{
		return;
	}
	float lImpact = pObject1->GetImpact(mGameManager->GetPhysicsManager()->GetGravity(), pForce, pTorque*0.01f, mSmallMass);
	if (lImpact < mLightImpact)
	{
		if (!pIsLoud)
		{
			return;
		}
		lImpact = mLightImpact;
	}
	const TBC::ChunkyBoneGeometry* lKey = pObject1->GetStructureGeometry(pBody1Id);
	OnCollision(lImpact, pPosition, lKey, lKey->GetMaterial());
}

void CollisionSoundManager::OnCollision(float pImpact, const Vector3DF& pPosition, const TBC::ChunkyBoneGeometry* pKey, const str& pSoundName)
{
	SoundInfo* lSoundInfo = GetPlayingSound(pKey);
	if (lSoundInfo)
	{
		const double lTime = (lSoundInfo->mSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)? mUiManager->GetSoundManager()->GetStreamTime(lSoundInfo->mSound->GetRamData()) : 0;
		if (lTime < mSoundCutoffDuration && strutil::StartsWith(lSoundInfo->mSound->GetName(), _T("collision_")+pSoundName))
		{
			if (pImpact > lSoundInfo->mBaseImpact * 1.2f)
			{
				// We are louder! Use our impact instead!
				lSoundInfo->mBaseImpact = pImpact;
				UpdateSound(lSoundInfo);
			}
		}
		else
		{
			// We are newer or different.
			if (pImpact > lSoundInfo->mBaseImpact * 0.7f)
			{
				// ... and we almost as load! Play us instead!
				StopSound(pKey);
				PlaySound(pKey, pSoundName, pPosition, pImpact);
			}
			else
			{
				// We must play both sounds at once. This hack (using key+1) will allow us to fire-and-forget.
				OnCollision(pImpact, pPosition, pKey+1, pSoundName);
			}
		}
	}
	else
	{
		PlaySound(pKey, pSoundName, pPosition, pImpact);
	}
}



CollisionSoundManager::SoundInfo* CollisionSoundManager::GetPlayingSound(const TBC::ChunkyBoneGeometry* pGeometryKey) const
{
	return HashUtil::FindMapObject(mSoundMap, pGeometryKey);
}

void CollisionSoundManager::PlaySound(const TBC::ChunkyBoneGeometry* pGeometryKey, const str& pSoundName, const Vector3DF& pPosition, float pImpact)
{
	SoundResourceInfo lResource;
	bool lGotSound = HashUtil::TryFindMapObject(mSoundNameMap, pSoundName, lResource);
	lGotSound &= (SoundInfo::GetVolume(pImpact, lResource) >= mLightImpact*MINIMUM_PLAYED_VOLUME_FACTOR);
	if (!lGotSound)
	{
		//mLog.Warningf(_T("Unable to play sound %s."), pSoundName.c_str());
		return;
	}

	SoundInfo* lSoundInfo = new SoundInfo(lResource);
	lSoundInfo->mPosition = pPosition;
	lSoundInfo->mBaseImpact = pImpact;
	lSoundInfo->mSound = new CollisionSoundResource(mUiManager, lSoundInfo);
	mSoundMap.insert(SoundMap::value_type(pGeometryKey, lSoundInfo));
	lSoundInfo->mSound->Load(mGameManager->GetResourceManager(), _T("collision_")+pSoundName+_T(".wav"),
		UiCure::UserSound3dResource::TypeLoadCallback(this, &CollisionSoundManager::OnSoundLoaded));
}

void CollisionSoundManager::OnSoundLoaded(UiCure::UserSound3dResource* pSoundResource)
{
	SoundInfo* lSoundInfo = ((CollisionSoundResource*)pSoundResource)->mSoundInfo;
	assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		float lRealTimeRatio;
		CURE_RTVAR_GET(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
		mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(),
			lSoundInfo->mPosition, Vector3DF());
		lSoundInfo->UpdateImpact();
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), lSoundInfo->mVolume, lSoundInfo->mPitch * lRealTimeRatio);
	}
}

void CollisionSoundManager::UpdateSound(SoundInfo* pSoundInfo)
{
	//for (unsigned x = 0; x < LEPRA_ARRAY_COUNT(pSoundInfo->mSound); ++x)
	{
		if (!pSoundInfo->mSound || pSoundInfo->mSound->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			return;
		}
		float lRealTimeRatio;
		CURE_RTVAR_GET(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
		pSoundInfo->UpdateImpact();
		mUiManager->GetSoundManager()->SetVolume(pSoundInfo->mSound->GetData(), pSoundInfo->mVolume);
		mUiManager->GetSoundManager()->SetPitch(pSoundInfo->mSound->GetData(), pSoundInfo->mPitch * lRealTimeRatio);
	}
}

void CollisionSoundManager::StopSound(const TBC::ChunkyBoneGeometry* pGeometryKey)
{
	SoundMap::iterator x = mSoundMap.find(pGeometryKey);
	if (x == mSoundMap.end())
	{
		return;
	}
	SoundInfo* lSoundInfo = x->second;
	mSoundMap.erase(x);
	mUiManager->GetSoundManager()->Stop(lSoundInfo->mSound->GetData());
	delete lSoundInfo;
}



CollisionSoundManager::SoundResourceInfo::SoundResourceInfo():
	mStrength(0),
	mMinimumClamp(0)
{
}

CollisionSoundManager::SoundResourceInfo::SoundResourceInfo(float pStrength, float pMinimumClamp, float pPitchFactor):
	mStrength(pStrength),
	mMinimumClamp(pMinimumClamp),
	mPitchFactor(pPitchFactor)
{
}



CollisionSoundManager::SoundInfo::SoundInfo(const SoundResourceInfo& pResourceInfo):
	mBaseImpact(0),
	mResourceInfo(pResourceInfo),
	mVolume(0),
	mPitch(0),
	mSound(0)
{
}

CollisionSoundManager::SoundInfo::~SoundInfo()
{
	//for (unsigned x = 0; x < LEPRA_ARRAY_COUNT(mSound); ++x)
	{
		delete mSound;
		mSound = 0;
	}
}

void CollisionSoundManager::SoundInfo::UpdateImpact()
{
	mVolume = GetVolume(mBaseImpact, mResourceInfo);
	if (!mResourceInfo.mPitchFactor)
	{
		mPitch = 1;
	}
	else
	{
		const float lTargetPitch = Math::Clamp(mBaseImpact, mResourceInfo.mMinimumClamp, 1.0f);
		mPitch = Math::Lerp(1.0f, lTargetPitch, mResourceInfo.mPitchFactor);
	}
}

float CollisionSoundManager::SoundInfo::GetVolume(float pBaseImpact, const SoundResourceInfo& pResourceInfo)
{
	return std::max(pBaseImpact * pResourceInfo.mStrength, pResourceInfo.mMinimumClamp);
}

void CollisionSoundManager::SoundInfo::operator=(const SoundInfo&)
{
	assert(false);
}



CollisionSoundManager::CollisionSoundResource::CollisionSoundResource(UiCure::GameUiManager* pUiManager,
	SoundInfo* pSoundInfo):
	Parent(pUiManager, UiLepra::SoundManager::LOOP_NONE),
	mSoundInfo(pSoundInfo)
{
}

void CollisionSoundManager::CollisionSoundResource::operator=(const CollisionSoundResource&)
{
	assert(false);
}



LOG_CLASS_DEFINE(GAME, CollisionSoundManager);



}
