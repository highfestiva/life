
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/PhysicsReferenceResource.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../Include/PositionalData.h"
#include "../Include/PositionHauler.h"



namespace Cure
{



PhysicsReferenceInitData::PhysicsReferenceInitData(TransformationF pTransformation, PhysicsOverride pPhysicsOverride, TBC::PhysicsManager* pPhysicsManager, int pPhysicsFps, GameObjectId pInstanceId):
	mTransformation(pTransformation),
	mPhysicsOverride(pPhysicsOverride),
	mPhysicsManager(pPhysicsManager),
	mPhysicsFps(pPhysicsFps),
	mInstanceId(pInstanceId)
{
}

void PhysicsReferenceInitData::operator=(const PhysicsReferenceInitData&)
{
	assert(false);
}



PhysicsReferenceResource::PhysicsReferenceResource(ResourceManager* pManager, const str& pName, const PhysicsReferenceInitData& pInitData):
	Parent(pManager, pName),
	mInitData(pInitData),
	mClassResource(0),
	mPhysicsLoadState(RESOURCE_UNLOADED)
{
}

PhysicsReferenceResource::~PhysicsReferenceResource()
{
	ReleasePhysics();
}

void PhysicsReferenceResource::ReleasePhysics()
{
	if (IsUnique())
	{
		SetRamData(0);
	}
	else
	{
		SetRamDataType(0);
	}
	SetLoadState(RESOURCE_UNLOADED);
	delete (mClassResource);
	mClassResource = 0;
}

const str PhysicsReferenceResource::GetType() const
{
	return _T("PhysicsRef");
}

bool PhysicsReferenceResource::IsReferenceType() const
{
	return true;
}

bool PhysicsReferenceResource::Load()
{
	bool lOk = (mClassResource == 0);
	assert(lOk);
	if (lOk)
	{
		const str lFilename = strutil::Split(GetName(), _T(";"), 1)[0];
		assert(lFilename != GetName());
		if (!IsUnique())
		{
			mClassResource = new ClassResource;
			mClassResource->Load(GetManager(), lFilename, ClassResource::TypeLoadCallback(this,
				&PhysicsReferenceResource::OnLoadClass));
		}
		else
		{
			// Unique means loading from parent instead of class/reference.
			lOk = Parent::LoadName(lFilename);
		}
	}
	return lOk;
}

ResourceLoadState PhysicsReferenceResource::PostProcess()
{
	if (mPhysicsLoadState != RESOURCE_UNLOADED)
	{
		// Already initialized for another context object.
		return mPhysicsLoadState;
	}

	if (!IsUnique())
	{
		// Non-unique (=shared) means loading from class/reference instead of parent.
		ResourceLoadState lLoadState = mClassResource->GetLoadState();
		if (lLoadState != RESOURCE_LOAD_COMPLETE)
		{
			return lLoadState;	// Probably "in progress", die another day.
		}
		TBC::ChunkyPhysics* lStructure = mClassResource->GetRamData();
		SetRamData(lStructure);
	}
	// First initalization of shared reference or unique instance.
	mPhysicsLoadState = RESOURCE_LOAD_ERROR;
	if (FinalizeInit())
	{
		mPhysicsLoadState = Parent::PostProcess();
	}
	return mPhysicsLoadState;
}

bool PhysicsReferenceResource::FinalizeInit()
{
	TBC::ChunkyPhysics* lStructure = GetRamData();
	TransformationF& lTransformation = mInitData.mTransformation;	// TRICKY: will change as we don't use reference!
	if (mInitData.mPhysicsOverride == PHYSICS_OVERRIDE_BONES)
	{
		return lStructure->FinalizeInit(0, 0, &lTransformation.GetPosition(), 0, 0);
	}
	else if (mInitData.mPhysicsOverride == PHYSICS_OVERRIDE_STATIC)
	{
		lStructure->SetPhysicsType(TBC::ChunkyPhysics::STATIC);
	}

	const int lPhysicsFps = mInitData.mPhysicsFps;
	bool lOk = lStructure->FinalizeInit(mInitData.mPhysicsManager, lPhysicsFps, &lTransformation.GetPosition(), mInitData.mInstanceId, mInitData.mInstanceId);
	assert(lOk);

	// Set orienation (as given in initial transform). The orientation in initial transform
	// is relative to the initial root bone orientation.
	if (lOk)
	{
		if (lStructure->GetPhysicsType() != TBC::ChunkyPhysics::STATIC)
		{
			const QuaternionF lPhysOrientation(mInitData.mPhysicsManager->GetBodyOrientation(lStructure->GetBoneGeometry(0)->GetBodyId()));
			ObjectPositionalData lPlacement;
			lOk = PositionHauler::Get(lPlacement, mInitData.mPhysicsManager, lStructure);
			assert(lOk);
			if (lOk)
			{
				ObjectPositionalData* lNewPlacement = (ObjectPositionalData*)lPlacement.Clone();
				lNewPlacement->mPosition.mTransformation =
					TransformationF(lTransformation.GetOrientation() * lPhysOrientation,
						lTransformation.GetPosition());
				PositionHauler::Set(*lNewPlacement, mInitData.mPhysicsManager, lStructure, true);
				delete lNewPlacement;
			}
		}
	}
	return lOk;
}

void PhysicsReferenceResource::OnLoadClass(ClassResource*)
{
}



}
