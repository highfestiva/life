
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "ResourceManager.h"



namespace Tbc
{
class PhysicsManager;
}



namespace Cure
{



struct PhysicsSharedInitData
{
	xform mTransformation;
	vec3 mVelocity;
	PhysicsOverride mPhysicsOverride;
	Lock* mPhysicsLock;
	Tbc::PhysicsManager* mPhysicsManager;
	const int mPhysicsFps;
	const GameObjectId mInstanceId;

	PhysicsSharedInitData(const xform& pTransformation, const vec3& pVelocity, PhysicsOverride pPhysicsOverride,
		Lock* pPhysicsLock, Tbc::PhysicsManager* pPhysicsManager, int pPhysicsFps, GameObjectId pInstanceId);
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

	void ReleasePhysics();
	const str GetType() const;
	bool IsReferenceType() const;
	ClassResource* GetParent() const;

	bool Load();
	ResourceLoadState PostProcess();
	bool FinalizeInit();
	void OnLoadClass(ClassResource*);

private:
	ClassResource* mClassResource;
	PhysicsSharedInitData mInitData;
	ResourceLoadState mPhysicsLoadState;

	logclass();
};



typedef UserExtraTypeResource<PhysicsSharedResource, PhysicsSharedInitData> UserPhysicsReferenceResource;



}
