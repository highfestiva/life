
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



struct PhysicsReferenceInitData
{
	TransformationF mTransformation;
	PhysicsOverride mPhysicsOverride;
	TBC::PhysicsManager* mPhysicsManager;
	const int mPhysicsFps;
	const GameObjectId mInstanceId;

	PhysicsReferenceInitData(TransformationF pTransformation, PhysicsOverride pPhysicsOverride, TBC::PhysicsManager* pPhysicsManager, int pPhysicsFps, GameObjectId pInstanceId);
	void operator=(const PhysicsReferenceInitData&);
};



// This class is used when sharing a physics manager (engine) between different contexts (such as split-screen clients).
class PhysicsReferenceResource: public PhysicsResource
{
	typedef PhysicsResource Parent;
public:
	typedef UserTypeResource<PhysicsResource> ClassResource;

	PhysicsReferenceResource(ResourceManager* pManager, const str& pName, const PhysicsReferenceInitData& pInitData);
	virtual ~PhysicsReferenceResource();

private:
	void ReleasePhysics();
	const str GetType() const;
	bool IsReferenceType() const;

	bool Load();
	ResourceLoadState PostProcess();
	bool FinalizeInit();
	void OnLoadClass(ClassResource*);

	PhysicsReferenceInitData mInitData;
	ClassResource* mClassResource;
	ResourceLoadState mPhysicsLoadState;

	LOG_CLASS_DECLARE();
};



typedef UserExtraTypeResource<PhysicsReferenceResource, PhysicsReferenceInitData> UserPhysicsReferenceResource;



}
