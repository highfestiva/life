
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyPhysics.h"
#include "../Include/PhysicsEngine.h"



namespace TBC
{



ChunkyPhysics::ChunkyPhysics(TransformOperation pTransformOperation, PhysicsType pPhysicsType):
	BoneHierarchy(),
	mTransformOperation(pTransformOperation),
	mPhysicsType(pPhysicsType),
	mUniqeGeometryIndex(0)
{
}

ChunkyPhysics::~ChunkyPhysics()
{
	assert(mGeometryArray.empty());	// Ensure all resources has been released prior to delete.
	ClearEngines();
}



void ChunkyPhysics::OnTick(PhysicsManager* pPhysicsManager, float pFrameTime)
{
	EngineArray::iterator x = mEngineArray.begin();
	for (; x != mEngineArray.end(); ++x)
	{
		(*x)->OnTick(pPhysicsManager, this, pFrameTime);
	}
}



void ChunkyPhysics::SetPhysicsType(PhysicsType pPhysicsType)
{
	// TODO: implement change in physics engine.
	mPhysicsType = pPhysicsType;
}

ChunkyPhysics::PhysicsType ChunkyPhysics::GetPhysicsType() const
{
	return (mPhysicsType);
}

ChunkyBoneGeometry* ChunkyPhysics::GetBoneGeometry(int pBoneIndex) const
{
	assert(pBoneIndex < GetBoneCount());
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
	assert(false);
	return (0);
}

void ChunkyPhysics::AddBoneGeometry(ChunkyBoneGeometry* pGeometry)
{
	assert((int)mGeometryArray.size() < GetBoneCount());
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
	assert(false);
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
			lGeometry->RemovePhysics(pPhysics);
			delete (lGeometry);
			mGeometryArray[x] = 0;
		}
	}
	mGeometryArray.clear();
	mUniqeGeometryIndex = 0;
}



int ChunkyPhysics::GetEngineCount() const
{
	return ((int)mEngineArray.size());
}

PhysicsEngine* ChunkyPhysics::GetEngine(int pEngineIndex) const
{
	assert((size_t)pEngineIndex < mEngineArray.size());
	return (mEngineArray[pEngineIndex]);
}

void ChunkyPhysics::AddEngine(PhysicsEngine* pEngine)
{
	mEngineArray.push_back(pEngine);
}

void ChunkyPhysics::SetEnginePower(unsigned pAspect, float pPower, float pAngle)
{
	EngineArray::iterator x = mEngineArray.begin();
	for (; x != mEngineArray.end(); ++x)
	{
		(*x)->SetValue(pAspect, pPower, pAngle);
	}
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

void ChunkyPhysics::ClearAll(PhysicsManager* pPhysics)
{
	ClearBoneGeometries(pPhysics);
	BoneHierarchy::ClearAll(pPhysics);
	ClearEngines();
}

void ChunkyPhysics::SetBoneCount(int pBoneCount)
{
	BoneHierarchy::SetBoneCount(pBoneCount);

	assert(mGeometryArray.empty());
	mGeometryArray.clear();

	mUniqeGeometryIndex = GetBoneCount();
}

bool ChunkyPhysics::FinalizeInit(PhysicsManager* pPhysics, unsigned pPhysicsFps, TransformationF* pTransform,
	PhysicsManager::TriggerListener* pTrigListener, PhysicsManager::ForceFeedbackListener* pForceListener)
{
	bool lOk = ((int)mGeometryArray.size() == GetBoneCount());
	assert(lOk);
	if (lOk)
	{
		if (pTransform)
		{
			const int lRoot = 0;
			SetOriginalBoneTransformation(lRoot, (*pTransform)*GetOriginalBoneTransformation(lRoot));
		}
		lOk = Parent::FinalizeInit(mTransformOperation);
	}
	if (lOk)
	{
		const int lBoneCount = GetBoneCount();
		for (int x = 0; lOk && x < lBoneCount; ++x)
		{
			ChunkyBoneGeometry* lGeometry = GetBoneGeometry(x);
			if (mPhysicsType == DYNAMIC || mPhysicsType == STATIC)
			{
				PhysicsManager::BodyType lBodyType = (mPhysicsType == DYNAMIC)? PhysicsManager::DYNAMIC : PhysicsManager::STATIC;
				if (lGeometry->GetParent())
				{
					if (lGeometry->GetJointType() == ChunkyBoneGeometry::JOINT_EXCLUDE)
					{
						lBodyType = PhysicsManager::STATIC;
					}
					else
					{
						lBodyType = PhysicsManager::DYNAMIC;
					}
				}
				const TransformationF& lBone = GetBoneTransformation(x);
				//QuaternionF q = lBone.GetOrientation();
				////q.Normalize();
				//RotationMatrixF rm = q.GetAsRotationMatrix();
				////rm.Reorthogonalize();
				//const Vector3DF lArrow0(0, 1, 0);
				//Vector3DF lArrow1 = rm*lArrow0;
				//lArrow1.x = 0;	// Project onto YZ.
				//mLog.Tracef(_T("Creating bone %i that has v1=(%f; %f; %f) from q=(%f; %f; %f; %f)."),
				//	x, lArrow1.x, lArrow1.y, lArrow1.z,
				//	q.GetA(), q.GetB(), q.GetC(), q.GetD());
				//const float lXAngle = ::acos(lArrow0*lArrow1) * 180/PIF;
				//const int lParentIndex = lGeometry->GetParent()? GetIndex(lGeometry->GetParent()) : -1;
				//mLog.Headlinef(_T("Creating bone %i (with parent %i) at (%f; %f; %f) with world x angle %f."),
				//	x, lParentIndex,
				//	lBone.GetPosition().x, lBone.GetPosition().y, lBone.GetPosition().z,
				//	lXAngle);
				//{	// TODO: remove entire scope!
				//	const TransformationF& lBone = GetOriginalBoneTransformation(x);
				//	QuaternionF q = lBone.GetOrientation();
				//	//q.Normalize();
				//	RotationMatrixF rm = q.GetAsRotationMatrix();
				//	const Vector3DF lArrow0(0, 1, 0);
				//	Vector3DF lArrow1 = rm*lArrow0;
				//	lArrow1.x = 0;	// Project onto YZ.
				//	const float lXAngle = ::acos(lArrow0*lArrow1) * 180/PIF;
				//	mLog.Headlinef(_T("Creating bone %i with local x angle %f from q=(%f; %f; %f; %f)."),
				//		x, lXAngle,
				//		q.GetA(), q.GetB(), q.GetC(), q.GetD());
				//}
				lOk = lGeometry->CreateBody(pPhysics, x == 0, pTrigListener, pForceListener,
					lBodyType, lBone);
			}
			else if (mPhysicsType == COLLISION_DETECT_ONLY)
			{
				lOk = lGeometry->CreateTrigger(pPhysics, pTrigListener, GetBoneTransformation(x));
			}
			else
			{
				assert(false);
			}
		}
		for (int x = 0; lOk && x < lBoneCount; ++x)
		{
			ChunkyBoneGeometry* lGeometry = GetBoneGeometry(x);
			lOk = lGeometry->CreateJoint(this, pPhysics, pPhysicsFps);
		}
	}
	assert(lOk);
	return (lOk);
}



unsigned ChunkyPhysics::GetNextGeometryIndex()
{
	return (++mUniqeGeometryIndex);
}



LOG_CLASS_DEFINE(PHYSICS, ChunkyPhysics);



}
