
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "../Include/PhysicsSharedResource.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../Include/PositionalData.h"
#include "../Include/PositionHauler.h"



namespace Cure
{



PhysicsSharedInitData::PhysicsSharedInitData(const TransformationF& pTransformation, const Vector3DF& pVelocity, PhysicsOverride pPhysicsOverride,
		TBC::PhysicsManager* pPhysicsManager, int pPhysicsFps, GameObjectId pInstanceId):
	mTransformation(pTransformation),
	mVelocity(pVelocity),
	mPhysicsOverride(pPhysicsOverride),
	mPhysicsManager(pPhysicsManager),
	mPhysicsFps(pPhysicsFps),
	mInstanceId(pInstanceId)
{
}

void PhysicsSharedInitData::operator=(const PhysicsSharedInitData&)
{
	deb_assert(false);
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

ResourceLoadState PhysicsSharedResource::InjectPostProcess()
{
	// TODO: leave this code be, if you try calling PostProcess() instead you won't
	//       be able to discover an already initialized resource.
	if (mPhysicsLoadState != RESOURCE_UNLOADED)
	{
		// Already initialized for another context object.
		return mPhysicsLoadState;
	}

	ResourceLoadState lLoadState = mClassResource->GetLoadState();
	if (lLoadState != RESOURCE_LOAD_COMPLETE)
	{
		return lLoadState;	// Probably "in progress", die another day.
	}

	// First initalization of shared reference or unique instance.
	mPhysicsLoadState = RESOURCE_LOAD_ERROR;
	if (FinalizeInit())
	{
		mPhysicsLoadState = Parent::PostProcess();
	}
	return mPhysicsLoadState;
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
	deb_assert(lOk);
	if (lOk)
	{
		const str lFilename = strutil::Split(GetName(), _T(";"), 1)[0];
		deb_assert(lFilename != GetName());
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
		return lLoadState;	// Probably "in progress", die another day.
	}

	return RESOURCE_LOAD_COMPLETE;
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

	// Pick desired orientation, but reset for FinalizeInit() to work with proper joint orientations.
	//const bool lIsDynamic = (lStructure->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC);
	QuaternionF lTargetOrientation = lTransformation.GetOrientation();
	lTransformation.SetOrientation(QuaternionF());

	const int lPhysicsFps = mInitData.mPhysicsFps;
	bool lOk = lStructure->FinalizeInit(mInitData.mPhysicsManager, lPhysicsFps, &lTransformation, mInitData.mInstanceId, mInitData.mInstanceId);
	deb_assert(lOk);

	// Set orienation (as given in initial transform). The orientation in initial transform
	// is relative to the initial root bone orientation.
	if (lOk)
	{
		if (lStructure->GetBoneGeometry(0)->GetBodyId() != TBC::INVALID_BODY)
		{
			const float lTotalMass = lStructure->QueryTotalMass(mInitData.mPhysicsManager);
			ObjectPositionalData lPlacement;
			lOk = PositionHauler::Get(lPlacement, mInitData.mPhysicsManager, lStructure, lTotalMass);
			deb_assert(lOk);
			if (lOk)
			{
				ObjectPositionalData* lNewPlacement = (ObjectPositionalData*)lPlacement.Clone();
				if (lStructure->GetPhysicsType() == TBC::ChunkyPhysics::WORLD)
				{
					lTargetOrientation *= lNewPlacement->mPosition.mTransformation.GetOrientation();
				}
				/*else if (lStructure->GetPhysicsType() == TBC::ChunkyPhysics::STATIC)
				{
					lTargetOrientation.RotateAroundOwnY(PIF);
				}*/
				lNewPlacement->mPosition.mTransformation =
					TransformationF(lTargetOrientation,
						lNewPlacement->mPosition.mTransformation.GetPosition());
				lNewPlacement->mPosition.mVelocity = mInitData.mVelocity;
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
