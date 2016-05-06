
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "resourcemanager.h"



namespace tbc {
class PhysicsManager;
}



namespace cure {



struct PhysicsSharedInitData {
	xform transformation_;
	vec3 velocity_;
	PhysicsOverride physics_override_;
	Lock* physics_lock_;
	tbc::PhysicsManager* physics_manager_;
	const int physics_fps_;
	const GameObjectId instance_id_;

	PhysicsSharedInitData(const xform& transformation, const vec3& velocity, PhysicsOverride physics_override,
		Lock* physics_lock, tbc::PhysicsManager* physics_manager, int physics_fps, GameObjectId instance_id);
	void operator=(const PhysicsSharedInitData&);
};



// This class is used when sharing a physics manager (engine) between different contexts (such as split-screen clients).
class PhysicsSharedResource: public PhysicsResource {
	typedef PhysicsResource Parent;
public:
	typedef UserTypeResource<PhysicsResource> ClassResource;

	PhysicsSharedResource(ResourceManager* manager, const str& name, const PhysicsSharedInitData& init_data);
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
	ClassResource* class_resource_;
	PhysicsSharedInitData init_data_;
	ResourceLoadState physics_load_state_;

	logclass();
};



typedef UserExtraTypeResource<PhysicsSharedResource, PhysicsSharedInitData> UserPhysicsReferenceResource;



}
