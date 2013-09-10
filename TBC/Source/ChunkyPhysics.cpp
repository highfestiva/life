
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/ChunkyPhysics.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/ListUtil.h"
#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/PhysicsEngine.h"
#include "../Include/PhysicsSpawner.h"
#include "../Include/PhysicsTrigger.h"



namespace TBC
{



#define TBC_PHYSICS_RELOCATE_POINTERS(a)					\
{										\
	const size_t cnt = a.size();						\
	for (size_t x = 0; x < cnt; ++x)					\
	{									\
		a[x]->RelocatePointers(this, &pOriginal, *pOriginal.a[x]);	\
	}									\
}




ChunkyPhysics::ChunkyPhysics(TransformOperation pTransformOperation, PhysicsType pPhysicsType):
	mTransformOperation(pTransformOperation),
	mPhysicsType(pPhysicsType),
	mGuideMode(GUIDE_EXTERNAL),
	mUniqeGeometryIndex(0)
{
}

ChunkyPhysics::ChunkyPhysics(const ChunkyPhysics& pOriginal):
	Parent(pOriginal)
{
	mGuideMode = pOriginal.mGuideMode;
	mUniqeGeometryIndex = pOriginal.mUniqeGeometryIndex;
	VectorUtil<ChunkyBoneGeometry>::CloneListFactoryMethod(mGeometryArray, pOriginal.mGeometryArray);
	TBC_PHYSICS_RELOCATE_POINTERS(mGeometryArray);
	VectorUtil<PhysicsEngine>::CloneList(mEngineArray, pOriginal.mEngineArray);
	TBC_PHYSICS_RELOCATE_POINTERS(mEngineArray);
	VectorUtil<PhysicsTrigger>::CloneList(mTriggerArray, pOriginal.mTriggerArray);
	TBC_PHYSICS_RELOCATE_POINTERS(mTriggerArray);
	VectorUtil<PhysicsSpawner>::CloneList(mSpawnerArray, pOriginal.mSpawnerArray);
	TBC_PHYSICS_RELOCATE_POINTERS(mSpawnerArray);
	mTransformOperation = pOriginal.mTransformOperation;
	mPhysicsType = pOriginal.mPhysicsType;
	mGuideMode = pOriginal.mGuideMode;
	mUniqeGeometryIndex = pOriginal.mUniqeGeometryIndex;
}

ChunkyPhysics::~ChunkyPhysics()
{
	//deb_assert(mGeometryArray.empty());	// Ensure all resources has been released prior to delete.
	ClearAll(0);
}



void ChunkyPhysics::OnMicroTick(PhysicsManager* pPhysicsManager, float pFrameTime)
{
	EngineArray::iterator x = mEngineArray.begin();
	for (; x != mEngineArray.end(); ++x)
	{
		(*x)->OnMicroTick(pPhysicsManager, this, pFrameTime);
	}
}



void ChunkyPhysics::SetTransformOperation(TransformOperation pOperation)
{
	mTransformOperation = pOperation;
}

ChunkyPhysics::PhysicsType ChunkyPhysics::GetPhysicsType() const
{
	return (mPhysicsType);
}

void ChunkyPhysics::SetPhysicsType(PhysicsType pPhysicsType)
{
	// TODO: implement change in physics engine.
	mPhysicsType = pPhysicsType;
}

ChunkyPhysics::GuideMode ChunkyPhysics::GetGuideMode() const
{
	return mGuideMode;
}

void ChunkyPhysics::SetGuideMode(GuideMode pGuideMode)
{
	mGuideMode = pGuideMode;
}

float ChunkyPhysics::QueryTotalMass(PhysicsManager* pPhysicsManager) const
{
	float lTotalMass = 0;
	const int lBoneCount = GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = GetBoneGeometry(x);
		if (lGeometry->GetBodyId())
		{
			lTotalMass += pPhysicsManager->GetBodyMass(lGeometry->GetBodyId());
		}
	}
	if (lTotalMass < 0.01f)
	{
		lTotalMass = 0.1f;
	}
	return lTotalMass;
}



ChunkyBoneGeometry* ChunkyPhysics::GetBoneGeometry(int pBoneIndex) const
{
	deb_assert(pBoneIndex >= 0 && pBoneIndex < GetBoneCount());
	return ((pBoneIndex < (int)mGeometryArray.size())? mGeometryArray[pBoneIndex] : 0);
}

ChunkyBoneGeometry* ChunkyPhysics::GetBoneGeometry(PhysicsManager::BodyID pBodyId) const
{
	const int lBoneCount = GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		ChunkyBoneGeometry* lGeometry = GetBoneGeometry(x);
		if (lGeometry->GetBodyId() == pBodyId)
		{
			return (lGeometry);	// TRICKY: RAII.
		}
	}
	deb_assert(false);
	return (0);
}

void ChunkyPhysics::AddBoneGeometry(ChunkyBoneGeometry* pGeometry)
{
	deb_assert((int)mGeometryArray.size() < GetBoneCount());
	mGeometryArray.push_back(pGeometry);
}

void ChunkyPhysics::AddBoneGeometry(const TransformationF& pTransformation,
	ChunkyBoneGeometry* pGeometry, const ChunkyBoneGeometry* pParent)
{
	const int lChildIndex = (int)mGeometryArray.size();
	SetOriginalBoneTransformation(lChildIndex, pTransformation);
	AddBoneGeometry(pGeometry);
	if (pParent)
	{
		int lParentIndex = GetIndex(pParent);
		AddChild(lParentIndex, lChildIndex);
	}
}

PhysicsManager::BodyType ChunkyPhysics::GetBodyType(const ChunkyBoneGeometry* pGeometry) const
{
	PhysicsManager::BodyType lBodyType = (mPhysicsType == DYNAMIC)? PhysicsManager::DYNAMIC : PhysicsManager::STATIC;
	if (pGeometry->GetParent())
	{
		if (pGeometry->GetJointType() == ChunkyBoneGeometry::JOINT_EXCLUDE)
		{
			lBodyType = PhysicsManager::STATIC;
		}
		else
		{
			lBodyType = PhysicsManager::DYNAMIC;
		}
	}
	return (lBodyType);
}

int ChunkyPhysics::GetIndex(const ChunkyBoneGeometry* pGeometry) const
{
	const int lBoneCount = GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		if (pGeometry == GetBoneGeometry(x))
		{
			return (x);
		}
	}
	deb_assert(false);
	mLog.AError("Trying to get uncontained geometry!");
	return (-1);
}

const TransformationF& ChunkyPhysics::GetTransformation(const ChunkyBoneGeometry* pGeometry) const
{
	return (GetBoneTransformation(GetIndex(pGeometry)));
}

void ChunkyPhysics::ClearBoneGeometries(PhysicsManager* pPhysics)
{
	for (size_t x = 0; x < mGeometryArray.size(); ++x)
	{
		ChunkyBoneGeometry* lGeometry = mGeometryArray[x];
		if (lGeometry)
		{
			if (pPhysics)
			{
				lGeometry->RemovePhysics(pPhysics);
			}
			delete (lGeometry);
			mGeometryArray[x] = 0;
		}
	}
	mGeometryArray.clear();
	mUniqeGeometryIndex = 0;
}

void ChunkyPhysics::EnableGravity(PhysicsManager* pPhysicsManager, bool pEnable)
{
	for (size_t x = 0; x < mGeometryArray.size(); ++x)
	{
		ChunkyBoneGeometry* lGeometry = mGeometryArray[x];
		if (!lGeometry || GetBodyType(lGeometry) == PhysicsManager::STATIC)
		{
			continue;
		}
		PhysicsManager::BodyID lBodyId = lGeometry->GetBodyId();
		pPhysicsManager->EnableGravity(lBodyId, pEnable);
	}
}


int ChunkyPhysics::GetEngineCount() const
{
	return ((int)mEngineArray.size());
}

PhysicsEngine* ChunkyPhysics::GetEngine(int pEngineIndex) const
{
	deb_assert((size_t)pEngineIndex < mEngineArray.size());
	return (mEngineArray[pEngineIndex]);
}

int ChunkyPhysics::GetEngineIndexFromControllerIndex(int pStartEngineIndex, int pEngineStep, unsigned pControllerIndex) const
{
	for (int x = pStartEngineIndex; x >= 0 && x < (int)mEngineArray.size(); x += pEngineStep)
	{
		if (mEngineArray[x]->GetControllerIndex() == pControllerIndex)
		{
			return x;
		}
	}
	return -1;
}

int ChunkyPhysics::GetEngineIndex(const PhysicsEngine* pEngine) const
{
	for (size_t x = 0; x < mEngineArray.size(); ++x)
	{
		if (mEngineArray[x] == pEngine)
		{
			return (int)x;
		}
	}
	return -1;
}

void ChunkyPhysics::AddEngine(PhysicsEngine* pEngine)
{
	mEngineArray.push_back(pEngine);
}

bool ChunkyPhysics::SetEnginePower(unsigned pAspect, float pPower)
{
	bool lOk = false;
	EngineArray::iterator x = mEngineArray.begin();
	for (; x != mEngineArray.end(); ++x)
	{
		lOk |= (*x)->SetValue(pAspect, pPower);
	}
	return lOk;
}

void ChunkyPhysics::ClearEngines()
{
	EngineArray::iterator x = mEngineArray.begin();
	for (; x != mEngineArray.end(); ++x)
	{
		delete (*x);
	}
	mEngineArray.clear();
}



int ChunkyPhysics::GetTriggerCount() const
{
	return ((int)mTriggerArray.size());
}

const PhysicsTrigger* ChunkyPhysics::GetTrigger(int pTriggerIndex) const
{
	deb_assert((size_t)pTriggerIndex < mTriggerArray.size());
	return (mTriggerArray[pTriggerIndex]);
}

void ChunkyPhysics::AddTrigger(PhysicsTrigger* pTrigger)
{
	mTriggerArray.push_back(pTrigger);
}

void ChunkyPhysics::ClearTriggers()
{
	TriggerArray::iterator x = mTriggerArray.begin();
	for (; x != mTriggerArray.end(); ++x)
	{
		delete (*x);
	}
	mTriggerArray.clear();
}



int ChunkyPhysics::GetSpawnerCount() const
{
	return (int)mSpawnerArray.size();
}

const PhysicsSpawner* ChunkyPhysics::GetSpawner(int pSpawnerIndex) const
{
	deb_assert((size_t)pSpawnerIndex < mSpawnerArray.size());
	return mSpawnerArray[pSpawnerIndex];
}

void ChunkyPhysics::AddSpawner(PhysicsSpawner* pSpawner)
{
	mSpawnerArray.push_back(pSpawner);
}

void ChunkyPhysics::ClearSpawners()
{
	SpawnerArray::iterator x = mSpawnerArray.begin();
	for (; x != mSpawnerArray.end(); ++x)
	{
		delete (*x);
	}
	mSpawnerArray.clear();
}



void ChunkyPhysics::ClearAll(PhysicsManager* pPhysics)
{
	ClearBoneGeometries(pPhysics);
	BoneHierarchy::ClearAll(pPhysics);
	ClearEngines();
	ClearTriggers();
	ClearSpawners();
}

void ChunkyPhysics::SetBoneCount(int pBoneCount)
{
	BoneHierarchy::SetBoneCount(pBoneCount);

	deb_assert(mGeometryArray.empty());
	mGeometryArray.clear();

	mUniqeGeometryIndex = GetBoneCount();
}

bool ChunkyPhysics::FinalizeInit(PhysicsManager* pPhysics, unsigned pPhysicsFps, const TransformationF* pTransform,
	int pTrigListenerId, int pForceListenerId)
{
	bool lOk = ((int)mGeometryArray.size() == GetBoneCount());
	deb_assert(lOk);
	if (lOk)
	{
		if (pTransform)
		{
			const int lRoot = 0;
			TransformationF lTransformation = GetOriginalBoneTransformation(lRoot);
			lTransformation = *pTransform * lTransformation;
			SetOriginalBoneTransformation(lRoot, lTransformation);
		}
		lOk = Parent::FinalizeInit(mTransformOperation);
	}
	if (lOk && pPhysics)
	{
		const int lBoneCount = GetBoneCount();
		for (int x = 0; lOk && x < lBoneCount; ++x)
		{
			ChunkyBoneGeometry* lGeometry = GetBoneGeometry(x);
			switch (lGeometry->GetBoneType())
			{
				case ChunkyBoneGeometry::BONE_BODY:
				{
					const PhysicsManager::BodyType lBodyType = GetBodyType(lGeometry);
					const TransformationF& lBone = GetBoneTransformation(x);
					lOk = lGeometry->CreateBody(pPhysics, x == 0, pForceListenerId, lBodyType, lBone);
					if (lOk)
					{
						pPhysics->EnableGravity(lGeometry->GetBodyId(), lGeometry->IsAffectedByGravity());
					}
					if (lOk)
					{
						pPhysics->EnableCollideWithSelf(lGeometry->GetBodyId(), lGeometry->IsCollideWithSelf());
					}
				}
				break;
				case ChunkyBoneGeometry::BONE_TRIGGER:
				{
					lOk = lGeometry->CreateTrigger(pPhysics, pTrigListenerId, GetBoneTransformation(x));
					if (lOk)
					{
						pPhysics->EnableTriggerBySelf(lGeometry->GetTriggerId(), lGeometry->IsCollideWithSelf());
					}
				}
				break;
			}
		}
		for (int x = 0; lOk && x < lBoneCount; ++x)
		{
			ChunkyBoneGeometry* lGeometry = GetBoneGeometry(x);
			lOk = lGeometry->CreateJoint(this, pPhysics, pPhysicsFps);
		}
	}
	deb_assert(lOk);
	return (lOk);
}



unsigned ChunkyPhysics::GetNextGeometryIndex()
{
	return (++mUniqeGeometryIndex);
}



LOG_CLASS_DEFINE(PHYSICS, ChunkyPhysics);



}
