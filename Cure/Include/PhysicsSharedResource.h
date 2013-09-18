
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	Vector3DF mVelocity;
	PhysicsOverride mPhysicsOverride;
	TBC::PhysicsManager* mPhysicsManager;
	const int mPhysicsFps;
	const GameObjectId mInstanceId;

	PhysicsSharedInitData(const TransformationF& pTransformation, const Vector3DF& pVelocity, PhysicsOverride pPhysicsOverride,
		TBC::PhysicsManager* pPhysicsManager, int pPhysicsFps, GameObjectId pInstanceId);
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

	ResourceLoadState InjectPostProcess();

private:
	void ReleasePhysics();
	const str GetType() const;
	bool IsReferenceType() const;

	bool Load();
	ResourceLoadState PostProcess();
	bool FinalizeInit();
	void OnLoadClass(ClassResource*);

	ClassResource* mClassResource;
	PhysicsSharedInitData mInitData;
	ResourceLoadState mPhysicsLoadState;

	LOG_CLASS_DECLARE();
};



typedef UserExtraTypeResource<PhysicsSharedResource, PhysicsSharedInitData> UserPhysicsReferenceResource;



}
