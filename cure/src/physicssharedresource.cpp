
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/physicssharedresource.h"
#include "../../tbc/include/chunkybonegeometry.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../include/positionaldata.h"
#include "../include/positionhauler.h"



namespace cure {



PhysicsSharedInitData::PhysicsSharedInitData(const xform& transformation, const vec3& velocity, PhysicsOverride physics_override,
		Lock* physics_lock, tbc::PhysicsManager* physics_manager, int physics_fps, GameObjectId instance_id):
	transformation_(transformation),
	velocity_(velocity),
	physics_override_(physics_override),
	physics_lock_(physics_lock),
	physics_manager_(physics_manager),
	physics_fps_(physics_fps),
	instance_id_(instance_id) {
}

void PhysicsSharedInitData::operator=(const PhysicsSharedInitData&) {
	deb_assert(false);
}



PhysicsSharedResource::PhysicsSharedResource(ResourceManager* manager, const str& name, const PhysicsSharedInitData& init_data):
	Parent(manager, name),
	class_resource_(new ClassResource),
	init_data_(init_data),
	physics_load_state_(kResourceUnloaded) {
}

PhysicsSharedResource::~PhysicsSharedResource() {
	delete class_resource_;
	class_resource_ = 0;

	tbc::ChunkyPhysics* structure = GetRamData();
	if (structure) {
		ScopeLock lock(init_data_.physics_lock_);
		structure->ClearAll(init_data_.physics_manager_);
	}
}

ResourceLoadState PhysicsSharedResource::InjectPostProcess() {
	// TODO: leave this code be, if you try calling PostProcess() instead you won't
	//       be able to discover an already initialized resource.
	if (physics_load_state_ != kResourceUnloaded) {
		// Already initialized for another context object.
		return physics_load_state_;
	}

	ResourceLoadState load_state = class_resource_->GetLoadState();
	if (load_state != kResourceLoadComplete) {
		return load_state;	// Probably "in progress", die another day.
	}

	// First initalization of shared reference or unique instance.
	physics_load_state_ = kResourceLoadError;
	if (FinalizeInit()) {
		physics_load_state_ = Parent::PostProcess();
	}
	return physics_load_state_;
}

const str PhysicsSharedResource::GetType() const {
	return "PhysicsShared";
}

bool PhysicsSharedResource::IsReferenceType() const {
	return true;
}

PhysicsSharedResource::ClassResource* PhysicsSharedResource::GetParent() const {
	return class_resource_;
}



bool PhysicsSharedResource::Load() {
	const str filename = strutil::Split(GetName(), ";", 1)[0];
	deb_assert(filename != GetName());
	class_resource_->Load(GetManager(), filename, ClassResource::TypeLoadCallback(this, &PhysicsSharedResource::OnLoadClass));
	return true;
}

ResourceLoadState PhysicsSharedResource::PostProcess() {
	if (physics_load_state_ != kResourceUnloaded) {
		// Already initialized for another context object.
		return physics_load_state_;
	}

	ResourceLoadState load_state = class_resource_->GetLoadState();
	if (load_state != kResourceLoadComplete) {
		return load_state;	// Probably "in progress", die another day.
	}

	return kResourceLoadComplete;
}

bool PhysicsSharedResource::FinalizeInit() {
	tbc::ChunkyPhysics* structure = GetRamData();
	xform _transformation = init_data_.transformation_;
	if (init_data_.physics_override_ == kPhysicsOverrideBones) {
		return structure->FinalizeInit(0, 0, &_transformation, 0);
	} else if (init_data_.physics_override_ == kPhysicsOverrideStatic) {
		structure->SetPhysicsType(tbc::ChunkyPhysics::kStatic);
	} else if (init_data_.physics_override_ == kPhysicsOverrideDynamic) {
		structure->SetPhysicsType(tbc::ChunkyPhysics::kDynamic);
	}

	// Pick desired orientation, but reset for FinalizeInit() to work with proper joint orientations.
	//const bool is_dynamic = (structure->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic);
	quat target_orientation = _transformation.GetOrientation();
	_transformation.SetOrientation(quat());

	const int _physics_fps = init_data_.physics_fps_;
	ScopeLock lock(init_data_.physics_lock_);
	bool ok = structure->FinalizeInit(init_data_.physics_manager_, _physics_fps, &_transformation, init_data_.instance_id_);
	deb_assert(ok);

	// Set orienation (as given in initial transform). The orientation in initial transform
	// is relative to the initial root bone orientation.
	if (ok) {
		/*if (structure->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic)*/ {
			const float total_mass = structure->QueryTotalMass(init_data_.physics_manager_);
			ObjectPositionalData placement;
			ok = PositionHauler::Get(placement, init_data_.physics_manager_, structure, total_mass);
			deb_assert(ok);
			if (ok) {
				ObjectPositionalData* new_placement = (ObjectPositionalData*)placement.Clone();
				if (structure->GetPhysicsType() == tbc::ChunkyPhysics::kWorld) {
					target_orientation *= new_placement->position_.transformation_.GetOrientation();
				}
				new_placement->position_.transformation_ =
					xform(target_orientation,
						new_placement->position_.transformation_.GetPosition());
				new_placement->position_.velocity_ = init_data_.velocity_;
				PositionHauler::Set(*new_placement, init_data_.physics_manager_, structure, total_mass, true);
				delete new_placement;
			}
		}
	}
	return ok;
}

void PhysicsSharedResource::OnLoadClass(ClassResource* class_resource) {
	if (class_resource->GetLoadState() != kResourceLoadComplete) {
		return;
	}
	tbc::ChunkyPhysics* copy = new tbc::ChunkyPhysics(*class_resource->GetData());
	// We correctly inherit the parent physics' xform. Then the whole object can be displaced
	// when finalizing. Displacement is a special case, normal is just to load and reload in
	// the same spot.
	//  - Don't: copy->SetOriginalBoneTransformation(0, xform());
	//  - Don't: copy->SetBoneTransformation(0, xform());
	SetRamData(copy);
}



}
