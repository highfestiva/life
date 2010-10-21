
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "CollisionSoundManager.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../Cure/Include/GameManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"



namespace Life
{



CollisionSoundManager::CollisionSoundManager(Cure::GameManager* pGameManager, UiCure::GameUiManager* pUiManager):
	mGameManager(pGameManager),
	mUiManager(pUiManager)
{
}

CollisionSoundManager::~CollisionSoundManager()
{
	mUiManager = 0;
	mGameManager = 0;
}



void CollisionSoundManager::Tick()
{
	SoundMap::iterator x = mSoundMap.begin();
	while (x != mSoundMap.end())
	{
		SoundInfo* lSoundInfo = x->second;
		if (lSoundInfo->mSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
		{
			if (!mUiManager->GetSoundManager()->IsPlaying(lSoundInfo->mSound->GetData()))
			{
				mSoundMap.erase(x++);
				delete lSoundInfo;
			}
			else
			{
				++x;
			}
		}
		else
		{
			++x;
		}
	}
}

void CollisionSoundManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	assert(pObject1 != pObject2);

	/*Vector3DF lVelocity1;
	Vector3DF lVelocity2;
	mGameManager->GetPhysicsManager()->GetBodyVelocity(pBody1Id, lVelocity1);
	mGameManager->GetPhysicsManager()->GetBodyVelocity(pBody2Id, lVelocity2);
	lVelocity1.Sub(lVelocity2);
	float lVelocitySquare = lVelocity1.GetDistanceSquared();
	if (lVelocitySquare < 2.0f)
	{
		return;
	}*/
	const float lImpact = pObject1->GetImpact(mGameManager->GetPhysicsManager()->GetGravity(), pForce, pTorque*0.001f);
	if (lImpact < 0.2f)
	{
		return;
	}
	/*lImpact *= lVelocitySquare;
	if (lImpact < 10.0f)
	{
		return;
	}*/

	GeometryCombination lKey(pObject1->GetStructureGeometry(pBody1Id), pObject2->GetStructureGeometry(pBody2Id));
	SoundInfo* lSoundInfo = GetPlayingSound(lKey);
	if (lSoundInfo)
	{
		if (lImpact > lSoundInfo->mImpact*1.8)
		{
			// We are louder! Play us instead!
			lSoundInfo->mImpact = lImpact;
			UpdateSound(lSoundInfo);
		}
	}
	else
	{
		PlaySound(lKey, pPosition, lImpact);
	}
}



CollisionSoundManager::SoundInfo* CollisionSoundManager::GetPlayingSound(const GeometryCombination& pGeometryKey) const
{
	return HashUtil::FindMapObject(mSoundMap, pGeometryKey);
	/*SoundMap::iterator x = mSoundMap.find(pGeometryKey);
	if (x != mSoundMap.end())
	{
		return x->second;
	}
	return 0;*/
}

void CollisionSoundManager::PlaySound(const GeometryCombination& pGeometryKey, const Vector3DF& pPosition, float pImpact)
{
	SoundInfo* lSoundData = new SoundInfo(pGeometryKey);
	lSoundData->mPosition = pPosition;
	lSoundData->mImpact = pImpact;
	lSoundData->mSound = new CollisionSoundResource(mUiManager, lSoundData);
	mSoundMap.insert(SoundMap::value_type(pGeometryKey, lSoundData));
	str lSoundName = _T("collision.wav");
	SoundNameMap::iterator x = mSoundNameMap.find(pGeometryKey);
	if (x != mSoundNameMap.end())
	{
		lSoundName = x->second;
	}
	lSoundData->mSound->Load(mGameManager->GetResourceManager(), _T("Data/")+lSoundName,
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
		lSoundInfo->UpdateParams();
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), lSoundInfo->mVolume, lSoundInfo->mPitch);
	}
	else
	{
		mSoundMap.erase(lSoundInfo->mKey);
	}
}

void CollisionSoundManager::UpdateSound(SoundInfo* pSoundInfo)
{
	if (pSoundInfo->mSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		pSoundInfo->UpdateParams();
		mUiManager->GetSoundManager()->SetVolume(pSoundInfo->mSound->GetData(), pSoundInfo->mVolume);
		mUiManager->GetSoundManager()->SetPitch(pSoundInfo->mSound->GetData(), pSoundInfo->mPitch);
	}
}



CollisionSoundManager::GeometryCombination::GeometryCombination(TBC::ChunkyBoneGeometry* pGeometryA,
	TBC::ChunkyBoneGeometry* pGeometryB)
{
	if (pGeometryA < pGeometryB)
	{
		mGeometry1 = pGeometryA;
		mGeometry2 = pGeometryB;
	}
	else
	{
		mGeometry1 = pGeometryB;
		mGeometry2 = pGeometryA;
	}
}

bool CollisionSoundManager::GeometryCombination::operator==(const GeometryCombination& pOther) const
{
	return (mGeometry1 == pOther.mGeometry1 && mGeometry2 == pOther.mGeometry2);
}



CollisionSoundManager::SoundInfo::SoundInfo(const GeometryCombination& pKey):
	mKey(pKey),
	mVolume(1),
	mPitch(1)
{
}

CollisionSoundManager::SoundInfo::~SoundInfo()
{
	delete mSound;
	mSound = 0;
}

void CollisionSoundManager::SoundInfo::UpdateParams()
{
	mVolume = Math::Clamp(mImpact, 0.2f, 2.0f);
	mPitch = 1;
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
