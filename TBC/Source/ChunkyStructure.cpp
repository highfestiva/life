
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyStructure.h"
#include "../Include/StructureEngine.h"



namespace TBC
{



ChunkyStructure::ChunkyStructure(PhysicsType pPhysicsType):
	BoneHierarchy(),
	mPhysicsType(pPhysicsType),
	mUniqeGeometryIndex(0)
{
}

ChunkyStructure::~ChunkyStructure()
{
	assert(mGeometryArray.empty());	// Ensure all resources has been released prior to delete.
	ClearStructureEngines();
}



void ChunkyStructure::OnTick(PhysicsEngine* pPhysicsManager, float pFrameTime)
{
	EngineArray::iterator x = mEngineArray.begin();
	for (; x != mEngineArray.end(); ++x)
	{
		(*x)->OnTick(pPhysicsManager, pFrameTime);
	}
}



void ChunkyStructure::SetPhysicsType(PhysicsType pPhysicsType)
{
	// TODO: implement change in physics engine.
	mPhysicsType = pPhysicsType;
}

ChunkyStructure::PhysicsType ChunkyStructure::GetPhysicsType() const
{
	return (mPhysicsType);
}

ChunkyBoneGeometry* ChunkyStructure::GetBoneGeometry(int pBoneIndex) const
{
	assert(pBoneIndex < GetBoneCount());
	return (mGeometryArray[pBoneIndex]);
}

ChunkyBoneGeometry* ChunkyStructure::GetBoneGeometry(PhysicsEngine::BodyID pBodyId) const
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
	assert(false);
	return (0);
}

void ChunkyStructure::AddBoneGeometry(ChunkyBoneGeometry* pGeometry)
{
	assert((int)mGeometryArray.size() < GetBoneCount());
	mGeometryArray.push_back(pGeometry);
}

void ChunkyStructure::AddBoneGeometry(const Lepra::TransformationF& pTransformation, ChunkyBoneGeometry* pGeometry)
{
	SetOriginalBoneTransformation((int)mGeometryArray.size(), pTransformation);
	AddBoneGeometry(pGeometry);
}

int ChunkyStructure::GetIndex(const ChunkyBoneGeometry* pGeometry) const
{
	const int lBoneCount = GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		if (pGeometry == GetBoneGeometry(x))
		{
			return (x);
		}
	}
	assert(false);
	return (0);
}

const Lepra::TransformationF& ChunkyStructure::GetTransformation(const ChunkyBoneGeometry* pGeometry) const
{
	return (GetOriginalBoneTransformation(GetIndex(pGeometry)));
}

void ChunkyStructure::ClearBoneGeometries(PhysicsEngine* pPhysics)
{
	for (int x = 0; x < GetBoneCount(); ++x)
	{
		mGeometryArray[x]->RemovePhysics(pPhysics);
		delete (mGeometryArray[x]);
		mGeometryArray[x] = 0;
	}
	mGeometryArray.clear();
	mUniqeGeometryIndex = 0;
}



int ChunkyStructure::GetStructureEngineCount() const
{
	return ((int)mEngineArray.size());
}

StructureEngine* ChunkyStructure::GetStructureEngine(int pBoneIndex) const
{
	assert((size_t)pBoneIndex < mEngineArray.size());
	return (mEngineArray[pBoneIndex]);
}

void ChunkyStructure::AddStructureEngine(StructureEngine* pEngine)
{
	mEngineArray.push_back(pEngine);
}

void ChunkyStructure::SetEnginePower(unsigned pAspect, float pPower, float pAngle)
{
	EngineArray::iterator x = mEngineArray.begin();
	for (; x != mEngineArray.end(); ++x)
	{
		(*x)->SetValue(pAspect, pPower, pAngle);
	}
}

void ChunkyStructure::ClearStructureEngines()
{
	EngineArray::iterator x = mEngineArray.begin();
	for (; x != mEngineArray.end(); ++x)
	{
		delete (*x);
	}
	mEngineArray.clear();
}

void ChunkyStructure::ClearAll(PhysicsEngine* pPhysics)
{
	ClearBoneGeometries(pPhysics);
	BoneHierarchy::ClearAll(pPhysics);
	ClearStructureEngines();
}

void ChunkyStructure::SetBoneCount(int pBoneCount)
{
	BoneHierarchy::SetBoneCount(pBoneCount);

	assert(mGeometryArray.empty());
	mGeometryArray.clear();

	mUniqeGeometryIndex = GetBoneCount();
}

bool ChunkyStructure::FinalizeInit(PhysicsEngine* pPhysics, PhysicsEngine::TriggerListener* pTrigListener,
	PhysicsEngine::ForceFeedbackListener* pForceListener)
{
	// TODO: add to physics engine depending on mPhysicsType.
	bool lOk = ((int)mGeometryArray.size() == GetBoneCount());
	assert(lOk);
	if (lOk)
	{
		lOk = BoneHierarchy::FinalizeInit();
	}
	if (lOk)
	{
		const int lBoneCount = GetBoneCount();
		for (int x = 0; lOk && x < lBoneCount; ++x)
		{
			ChunkyBoneGeometry* lGeometry = GetBoneGeometry(x);
			if (mPhysicsType == DYNAMIC || mPhysicsType == STATIC)
			{
				PhysicsEngine::BodyType lBodyType = (mPhysicsType == DYNAMIC)? PhysicsEngine::DYNAMIC : PhysicsEngine::STATIC;
				if (lGeometry->GetParent() && lGeometry->GetJointType() == ChunkyBoneGeometry::TYPE_EXCLUDE)
				{
					lBodyType = PhysicsEngine::STATIC;
				}
				lOk = lGeometry->CreateBody(pPhysics, x == 0, pTrigListener, pForceListener,
					lBodyType, GetOriginalBoneTransformation(x));
			}
			else if (mPhysicsType == COLLISION_DETECT_ONLY)
			{
				lOk = lGeometry->CreateTrigger(pPhysics, pTrigListener, GetOriginalBoneTransformation(x));
			}
			else
			{
				assert(false);
			}
		}
		for (int z = 0; lOk && z < lBoneCount; ++z)
		{
			ChunkyBoneGeometry* lGeometry = GetBoneGeometry(z);
			lOk = lGeometry->CreateJoint(this, pPhysics);
		}
	}
	assert(lOk);
	return (lOk);
}



unsigned ChunkyStructure::GetNextGeometryIndex()
{
	return (++mUniqeGeometryIndex);
}



}
