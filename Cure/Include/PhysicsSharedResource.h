
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "ResourceManager.h"



namespace TBC
{
class PhysicsManager;
}



namespace Cure
{



enum PhysicsOverride
{
	PHYSICS_OVERRIDE_NORMAL = 1,
	PHYSICS_OVERRIDE_STATIC,
	PHYSICS_OVERRIDE_BONES,
};



struct PhysicsSharedInitData
{
	TransformationF mTransformation;
	PhysicsOverride mPhysicsOverride;
	TBC::PhysicsManager* mPhysicsManager;
	const int mPhysicsFps;
	const GameObjectId mInstanceId;

	PhysicsSharedInitData(TransformationF pTransformation, PhysicsOverride pPhysicsOverride, TBC::PhysicsManager* pPhysicsManager, int pPhysicsFps, GameObjectId pInstanceId);
	void operator=(const PhysicsSharedInitData&);
};



// This class is used when sharing a physics manager (engine) between different contexts (such as split-screen clients).
class PhysicsSharedResource: public PhysicsResource
{
	typedef PhysicsResource Parent;
public:
	typedef UserTypeResource<PhysicsResource> ClassResource;

	PhysicsSharedResource(ResourceManager* pManager, const str& pName, const PhysicsSharedInitData& pInitData);
	virtual ~PhysicsSharedResource();

private:
	void ReleasePhysics();
	const str GetType() const;

	bool Load();
	ResourceLoadState PostProcess();
	bool FinalizeInit();

	PhysicsSharedInitData mInitData;
	ResourceLoadState mPhysicsLoadState;

	LOG_CLASS_DECLARE();
};



typedef UserExtraTypeResource<PhysicsSharedResource, PhysicsSharedInitData> UserPhysicsReferenceResource;



}
