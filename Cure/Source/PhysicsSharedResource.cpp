
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/PhysicsSharedResource.h"
#include "../../Tbc/Include/ChunkyBoneGeometry.h"
#include "../../Tbc/Include/ChunkyPhysics.h"
#include "../Include/PositionalData.h"
#include "../Include/PositionHauler.h"



namespace Cure
{



PhysicsSharedInitData::PhysicsSharedInitData(const xform& pTransformation, const vec3& pVelocity, PhysicsOverride pPhysicsOverride,
		Lock* pPhysicsLock, Tbc::PhysicsManager* pPhysicsManager, int pPhysicsFps, GameObjectId pInstanceId):
	mTransformation(pTransformation),
	mVelocity(pVelocity),
	mPhysicsOverride(pPhysicsOverride),
	mPhysicsLock(pPhysicsLock),
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
	mClassResource(new ClassResource),
	mInitData(pInitData),
	mPhysicsLoadState(RESOURCE_UNLOADED)
{
}

PhysicsSharedResource::~PhysicsSharedResource()
{
	delete mClassResource;
	mClassResource = 0;

	Tbc::ChunkyPhysics* lStructure = GetRamData();
	if (lStructure)
	{
		ScopeLock lLock(mInitData.mPhysicsLock);
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
	return "PhysicsShared";
}

bool PhysicsSharedResource::IsReferenceType() const
{
	return true;
}

PhysicsSharedResource::ClassResource* PhysicsSharedResource::GetParent() const
{
	return mClassResource;
}



bool PhysicsSharedResource::Load()
{
	const str lFilename = strutil::Split(GetName(), ";", 1)[0];
	deb_assert(lFilename != GetName());
	mClassResource->Load(GetManager(), lFilename, ClassResource::TypeLoadCallback(this, &PhysicsSharedResource::OnLoadClass));
	return true;
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
	Tbc::ChunkyPhysics* lStructure = GetRamData();
	xform lTransformation = mInitData.mTransformation;
	if (mInitData.mPhysicsOverride == PHYSICS_OVERRIDE_BONES)
	{
		return lStructure->FinalizeInit(0, 0, &lTransformation, 0);
	}
	else if (mInitData.mPhysicsOverride == PHYSICS_OVERRIDE_STATIC)
	{
		lStructure->SetPhysicsType(Tbc::ChunkyPhysics::STATIC);
	}
	else if (mInitData.mPhysicsOverride == PHYSICS_OVERRIDE_DYNAMIC)
	{
		lStructure->SetPhysicsType(Tbc::ChunkyPhysics::DYNAMIC);
	}

	// Pick desired orientation, but reset for FinalizeInit() to work with proper joint orientations.
	//const bool lIsDynamic = (lStructure->GetPhysicsType() == Tbc::ChunkyPhysics::DYNAMIC);
	quat lTargetOrientation = lTransformation.GetOrientation();
	lTransformation.SetOrientation(quat());

	const int lPhysicsFps = mInitData.mPhysicsFps;
	ScopeLock lLock(mInitData.mPhysicsLock);
	bool lOk = lStructure->FinalizeInit(mInitData.mPhysicsManager, lPhysicsFps, &lTransformation, mInitData.mInstanceId);
	deb_assert(lOk);

	// Set orienation (as given in initial transform). The orientation in initial transform
	// is relative to the initial root bone orientation.
	if (lOk)
	{
		if (lStructure->GetPhysicsType() == Tbc::ChunkyPhysics::DYNAMIC)
		{
			const float lTotalMass = lStructure->QueryTotalMass(mInitData.mPhysicsManager);
			ObjectPositionalData lPlacement;
			lOk = PositionHauler::Get(lPlacement, mInitData.mPhysicsManager, lStructure, lTotalMass);
			deb_assert(lOk);
			if (lOk)
			{
				ObjectPositionalData* lNewPlacement = (ObjectPositionalData*)lPlacement.Clone();
				if (lStructure->GetPhysicsType() == Tbc::ChunkyPhysics::WORLD)
				{
					lTargetOrientation *= lNewPlacement->mPosition.mTransformation.GetOrientation();
				}
				lNewPlacement->mPosition.mTransformation =
					xform(lTargetOrientation,
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
	Tbc::ChunkyPhysics* lCopy = new Tbc::ChunkyPhysics(*pClassResource->GetData());
	// We correctly inherit the parent physics' xform. Then the whole object can be displaced
	// when finalizing. Displacement is a special case, normal is just to load and reload in
	// the same spot.
	//  - Don't: lCopy->SetOriginalBoneTransformation(0, xform());
	//  - Don't: lCopy->SetBoneTransformation(0, xform());
	SetRamData(lCopy);
}



}
