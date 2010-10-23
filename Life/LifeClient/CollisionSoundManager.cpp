
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "CollisionSoundManager.h"
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../Cure/Include/GameManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"



namespace Life
{



#define MINIMUM_PLAYED_VOLUME	0.3f



CollisionSoundManager::CollisionSoundManager(Cure::GameManager* pGameManager, UiCure::GameUiManager* pUiManager):
	mGameManager(pGameManager),
	mUiManager(pUiManager)
{
	mSoundNameMap.insert(SoundNameMap::value_type(_T("small_metal"),	SoundResourceInfo(0.2f, 0.4f)));
	mSoundNameMap.insert(SoundNameMap::value_type(_T("big_metal"),		SoundResourceInfo(1.5f, 0.4f)));
	mSoundNameMap.insert(SoundNameMap::value_type(_T("plastic"),		SoundResourceInfo(1.0f, 0.4f)));
	mSoundNameMap.insert(SoundNameMap::value_type(_T("rubber"),		SoundResourceInfo(1.0f, 0.5f)));
	mSoundNameMap.insert(SoundNameMap::value_type(_T("wood"),		SoundResourceInfo(1.0f, 0.5f)));
}

CollisionSoundManager::~CollisionSoundManager()
{
	mUiManager = 0;
	mGameManager = 0;
}



void CollisionSoundManager::Tick(const Vector3DF& pCameraPosition)
{
	mCameraPosition = pCameraPosition;

	SoundMap::iterator x = mSoundMap.begin();
	while (x != mSoundMap.end())
	{
		SoundInfo* lSoundInfo = x->second;
		bool lOneIsPlaying = false;
		//for (unsigned y = 0; !lOneIsPlaying && y < 2; ++y)
		{
			if (lSoundInfo->mSound)
			{
				if (lSoundInfo->mSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
				{
					lOneIsPlaying |= mUiManager->GetSoundManager()->IsPlaying(lSoundInfo->mSound->GetData());
				}
				else if (lSoundInfo->mSound->GetLoadState() == Cure::RESOURCE_LOAD_IN_PROGRESS)
				{
					lOneIsPlaying = true;
				}
			}
		}
		if (lOneIsPlaying)
		{
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
	TBC::PhysicsManager::BodyID pBody1Id)
{
	if (pPosition.GetDistanceSquared(mCameraPosition) > 200*200)
	{
		return;
	}
	//Vector3DF lVelocity1;
	//Vector3DF lVelocity2;
	//mGameManager->GetPhysicsManager()->GetBodyVelocity(pBody1Id, lVelocity1);
	//mGameManager->GetPhysicsManager()->GetBodyVelocity(pBody2Id, lVelocity2);
	//float lVelocitySquare = lVelocity1.GetDistanceSquared(lVelocity2);
	//if (lVelocitySquare < 0.2f)
	if (pObject1->GetVelocity().GetDistanceSquared(pObject2->GetVelocity()) < 0.2f)
	{
		return;
	}
	const float lImpact = pObject1->GetImpact(mGameManager->GetPhysicsManager()->GetGravity(), pForce, pTorque*0.01f, 50, 0.01f);
	if (lImpact < 0.5f)
	{
		return;
	}

	const TBC::ChunkyBoneGeometry* lKey = pObject1->GetStructureGeometry(pBody1Id);
	SoundInfo* lSoundInfo = GetPlayingSound(lKey);
	if (lSoundInfo)
	{
		if (lImpact > lSoundInfo->mBaseImpact*1.8)
		{
			// We are louder! Play us instead!
			lSoundInfo->mBaseImpact = lImpact;
			UpdateSound(lSoundInfo);
		}
	}
	else
	{
		PlaySound(lKey, pPosition, lImpact);
	}
}



CollisionSoundManager::SoundInfo* CollisionSoundManager::GetPlayingSound(const TBC::ChunkyBoneGeometry* pGeometryKey) const
{
	return HashUtil::FindMapObject(mSoundMap, pGeometryKey);
}

void CollisionSoundManager::PlaySound(const TBC::ChunkyBoneGeometry* pGeometryKey, const Vector3DF& pPosition, float pImpact)
{
	const str& lMaterial = pGeometryKey->GetMaterial();
	SoundResourceInfo lResource;
	bool lGotSound = HashUtil::TryFindMapObject(mSoundNameMap, lMaterial, lResource);
	lGotSound &= (SoundInfo::GetVolume(pImpact, lResource) >= MINIMUM_PLAYED_VOLUME);
	if (!lGotSound)
	{
		return;
	}

	SoundInfo* lSoundInfo = new SoundInfo(lResource);
	lSoundInfo->mPosition = pPosition;
	lSoundInfo->mBaseImpact = pImpact;
	lSoundInfo->mSound = new CollisionSoundResource(mUiManager, lSoundInfo);
	mSoundMap.insert(SoundMap::value_type(pGeometryKey, lSoundInfo));
	lSoundInfo->mSound->Load(mGameManager->GetResourceManager(), _T("Data/collision_")+lMaterial+_T(".wav"),
		UiCure::UserSound3dResource::TypeLoadCallback(this, &CollisionSoundManager::OnSoundLoaded));
}

void CollisionSoundManager::OnSoundLoaded(UiCure::UserSound3dResource* pSoundResource)
{
	SoundInfo* lSoundInfo = ((CollisionSoundResource*)pSoundResource)->mSoundInfo;
	assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(),
			lSoundInfo->mPosition, Vector3DF());
		lSoundInfo->UpdateImpact();
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), lSoundInfo->mVolume, lSoundInfo->mPitch);
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
		pSoundInfo->UpdateImpact();
		mUiManager->GetSoundManager()->SetVolume(pSoundInfo->mSound->GetData(), pSoundInfo->mVolume);
		mUiManager->GetSoundManager()->SetPitch(pSoundInfo->mSound->GetData(), pSoundInfo->mPitch);
	}
}



/*CollisionSoundManager::GeometryCombination::GeometryCombination(TBC::ChunkyBoneGeometry* pGeometry):
	mGeometry(pGeometry)
{
}

bool CollisionSoundManager::GeometryCombination::operator==(const GeometryCombination& pOther) const
{
	return (mGeometry == pOther.mGeometry);
}*/



CollisionSoundManager::SoundResourceInfo::SoundResourceInfo():
	mStrength(0),
	mMinimumClamp(0)
{
}

CollisionSoundManager::SoundResourceInfo::SoundResourceInfo(float pStrength, float pMinimumClamp):
	mStrength(pStrength),
	mMinimumClamp(pMinimumClamp)
{
}



CollisionSoundManager::SoundInfo::SoundInfo(const SoundResourceInfo& pResourceInfo):
	mBaseImpact(0),
	mResourceInfo(pResourceInfo)
{
	::memset(&mVolume, 0, sizeof(mVolume));
	::memset(&mPitch, 0, sizeof(mPitch));
	::memset(&mSound, 0, sizeof(mSound));
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
	mPitch = 1;
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
