
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/PhysicsSharedResource.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../Include/PositionalData.h"
#include "../Include/PositionHauler.h"



namespace Cure
{



PhysicsSharedInitData::PhysicsSharedInitData(TransformationF pTransformation, PhysicsOverride pPhysicsOverride, TBC::PhysicsManager* pPhysicsManager, int pPhysicsFps, GameObjectId pInstanceId):
	mTransformation(pTransformation),
	mPhysicsOverride(pPhysicsOverride),
	mPhysicsManager(pPhysicsManager),
	mPhysicsFps(pPhysicsFps),
	mInstanceId(pInstanceId)
{
}

void PhysicsSharedInitData::operator=(const PhysicsSharedInitData&)
{
	assert(false);
}



PhysicsSharedResource::PhysicsSharedResource(ResourceManager* pManager, const str& pName, const PhysicsSharedInitData& pInitData):
	Parent(pManager, pName),
	mClassResource(0),
	mInitData(pInitData),
	mPhysicsLoadState(RESOURCE_UNLOADED)
{
}

PhysicsSharedResource::~PhysicsSharedResource()
{
	delete mClassResource;
	mClassResource = 0;

	TBC::ChunkyPhysics* lStructure = GetRamData();
	if (lStructure)
	{
		lStructure->ClearAll(mInitData.mPhysicsManager);
	}
}

const str PhysicsSharedResource::GetType() const
{
	return _T("PhysicsShared");
}

bool PhysicsSharedResource::IsReferenceType() const
{
	return true;
}



bool PhysicsSharedResource::Load()
{
	bool lOk = (mClassResource == 0);
	assert(lOk);
	if (lOk)
	{
		const str lFilename = strutil::Split(GetName(), _T(";"), 1)[0];
		assert(lFilename != GetName());
		mClassResource = new ClassResource;
		mClassResource->Load(GetManager(), lFilename, ClassResource::TypeLoadCallback(this, &PhysicsSharedResource::OnLoadClass));
	}
	return lOk;
}

ResourceLoadState PhysicsSharedResource::PostProcess()
{
	if (mPhysicsLoadState != RESOURCE_UNLOADED)
	{
		// Already initialized for another context object.
		return mPhysicsLoadState;
	}

	ResourceLoadState lLoadState = mClassResource->GetLoadState();
	if (lLoadState != RESOURCE_LOAD_COMPLETE)
	{
		return lLoadState;      // Probably "in progress", die another day.
	}

	// First initalization of shared reference or unique instance.
	mPhysicsLoadState = RESOURCE_LOAD_ERROR;
	if (FinalizeInit())
	{
		mPhysicsLoadState = Parent::PostProcess();
	}
	return mPhysicsLoadState;
}

bool PhysicsSharedResource::FinalizeInit()
{
	TBC::ChunkyPhysics* lStructure = GetRamData();
	TransformationF lTransformation = mInitData.mTransformation;
	if (mInitData.mPhysicsOverride == PHYSICS_OVERRIDE_BONES)
	{
		return lStructure->FinalizeInit(0, 0, &lTransformation, 0, 0);
	}
	else if (mInitData.mPhysicsOverride == PHYSICS_OVERRIDE_STATIC)
	{
		lStructure->SetPhysicsType(TBC::ChunkyPhysics::STATIC);
	}

	const int lPhysicsFps = mInitData.mPhysicsFps;
	bool lOk = lStructure->FinalizeInit(mInitData.mPhysicsManager, lPhysicsFps, &lTransformation, mInitData.mInstanceId, mInitData.mInstanceId);
	assert(lOk);

	// Set orienation (as given in initial transform). The orientation in initial transform
	// is relative to the initial root bone orientation.
	if (lOk)
	{
		if (lStructure->GetPhysicsType() != TBC::ChunkyPhysics::STATIC)
		{
			const QuaternionF lPhysOrientation(mInitData.mPhysicsManager->GetBodyOrientation(lStructure->GetBoneGeometry(0)->GetBodyId()));
			const float lTotalMass = lStructure->QueryTotalMass(mInitData.mPhysicsManager);
			ObjectPositionalData lPlacement;
			lOk = PositionHauler::Get(lPlacement, mInitData.mPhysicsManager, lStructure, lTotalMass);
			assert(lOk);
			if (lOk)
			{
				ObjectPositionalData* lNewPlacement = (ObjectPositionalData*)lPlacement.Clone();
				lNewPlacement->mPosition.mTransformation =
					TransformationF(lTransformation.GetOrientation() * lPhysOrientation,
						lTransformation.GetPosition());
				PositionHauler::Set(*lNewPlacement, mInitData.mPhysicsManager, lStructure, lTotalMass, true);
				delete lNewPlacement;
			}
		}
	}
	return lOk;
}

void PhysicsSharedResource::OnLoadClass(ClassResource* pClassResource)
{
	if (pClassResource->GetLoadState() != RESOURCE_LOAD_COMPLETE)
	{
		return;
	}
	TBC::ChunkyPhysics* lCopy = new TBC::ChunkyPhysics(*pClassResource->GetData());
	SetRamData(lCopy);
}



}
