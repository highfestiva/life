
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/chunkyphysics.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/listutil.h"
#include "../include/chunkybonegeometry.h"
#include "../include/physicsengine.h"
#include "../include/physicsspawner.h"
#include "../include/physicstrigger.h"



namespace tbc {



#define TBC_PHYSICS_RELOCATE_POINTERS(a)					\
{										\
	const size_t cnt = a.size();						\
	for (size_t x = 0; x < cnt; ++x)					\
	{									\
		a[x]->RelocatePointers(this, &original, *original.a[x]);	\
	}									\
}




ChunkyPhysics::ChunkyPhysics(TransformOperation transform_operation, PhysicsType physics_type):
	transform_operation_(transform_operation),
	physics_type_(physics_type),
	guide_mode_(kGuideExternal),
	uniqe_geometry_index_(0) {
}

ChunkyPhysics::ChunkyPhysics(const ChunkyPhysics& original):
	Parent(original) {
	guide_mode_ = original.guide_mode_;
	uniqe_geometry_index_ = original.uniqe_geometry_index_;
	VectorUtil<ChunkyBoneGeometry>::CloneListFactoryMethod(geometry_array_, original.geometry_array_);
	TBC_PHYSICS_RELOCATE_POINTERS(geometry_array_);
	VectorUtil<PhysicsEngine>::CloneList(engine_array_, original.engine_array_);
	TBC_PHYSICS_RELOCATE_POINTERS(engine_array_);
	VectorUtil<PhysicsTrigger>::CloneList(trigger_array_, original.trigger_array_);
	TBC_PHYSICS_RELOCATE_POINTERS(trigger_array_);
	VectorUtil<PhysicsSpawner>::CloneList(spawner_array_, original.spawner_array_);
	TBC_PHYSICS_RELOCATE_POINTERS(spawner_array_);
	transform_operation_ = original.transform_operation_;
	physics_type_ = original.physics_type_;
	guide_mode_ = original.guide_mode_;
	uniqe_geometry_index_ = original.uniqe_geometry_index_;
}

ChunkyPhysics::~ChunkyPhysics() {
	//deb_assert(geometry_array_.empty());	// Ensure all resources has been released prior to delete.
	ClearAll(0);
}



void ChunkyPhysics::OnMicroTick(PhysicsManager* physics_manager, float frame_time) {
	EngineArray::iterator x = engine_array_.begin();
	for (; x != engine_array_.end(); ++x) {
		(*x)->OnMicroTick(physics_manager, this, frame_time);
	}
}



void ChunkyPhysics::SetTransformOperation(TransformOperation operation) {
	transform_operation_ = operation;
}

ChunkyPhysics::PhysicsType ChunkyPhysics::GetPhysicsType() const {
	return (physics_type_);
}

void ChunkyPhysics::SetPhysicsType(PhysicsType physics_type) {
	// TODO: implement change in physics engine.
	physics_type_ = physics_type;
}

ChunkyPhysics::GuideMode ChunkyPhysics::GetGuideMode() const {
	return guide_mode_;
}

void ChunkyPhysics::SetGuideMode(GuideMode guide_mode) {
	guide_mode_ = guide_mode;
}

float ChunkyPhysics::QueryTotalMass(PhysicsManager* physics_manager) const {
	float total_mass = 0;
	const int _bone_count = GetBoneCount();
	for (int x = 0; x < _bone_count; ++x) {
		const tbc::ChunkyBoneGeometry* _geometry = GetBoneGeometry(x);
		if (_geometry->GetBodyId()) {
			total_mass += physics_manager->GetBodyMass(_geometry->GetBodyId());
		}
	}
	if (total_mass < 0.01f) {
		total_mass = 0.1f;
	}
	return total_mass;
}



ChunkyBoneGeometry* ChunkyPhysics::GetBoneGeometry(int bone_index) const {
	deb_assert(bone_index >= 0 && bone_index < GetBoneCount());
	return ((bone_index < (int)geometry_array_.size())? geometry_array_[bone_index] : 0);
}

ChunkyBoneGeometry* ChunkyPhysics::GetBoneGeometry(PhysicsManager::BodyID body_id) const {
	const int _bone_count = GetBoneCount();
	for (int x = 0; x < _bone_count; ++x) {
		ChunkyBoneGeometry* _geometry = GetBoneGeometry(x);
		if (_geometry->GetBodyId() == body_id) {
			return (_geometry);	// TRICKY: RAII.
		}
	}
	deb_assert(false);
	return (0);
}

void ChunkyPhysics::AddBoneGeometry(ChunkyBoneGeometry* geometry) {
	deb_assert((int)geometry_array_.size() < GetBoneCount());
	geometry_array_.push_back(geometry);
}

void ChunkyPhysics::AddBoneGeometry(const xform& transformation,
	ChunkyBoneGeometry* geometry, const ChunkyBoneGeometry* parent) {
	const int child_index = (int)geometry_array_.size();
	SetOriginalBoneTransformation(child_index, transformation);
	AddBoneGeometry(geometry);
	if (parent) {
		int parent_index = GetIndex(parent);
		AddChild(parent_index, child_index);
	}
}

PhysicsManager::BodyType ChunkyPhysics::GetBodyType(const ChunkyBoneGeometry* geometry) const {
	PhysicsManager::BodyType body_type = (physics_type_ == kDynamic)? PhysicsManager::kDynamic : PhysicsManager::kStatic;
	if (geometry->GetParent()) {
		if (geometry->GetJointType() == ChunkyBoneGeometry::kJointExclude) {
			body_type = PhysicsManager::kStatic;
		} else {
			body_type = PhysicsManager::kDynamic;
		}
	}
	return (body_type);
}

int ChunkyPhysics::GetIndex(const ChunkyBoneGeometry* geometry) const {
	const int _bone_count = GetBoneCount();
	for (int x = 0; x < _bone_count; ++x) {
		if (geometry == GetBoneGeometry(x)) {
			return (x);
		}
	}
	deb_assert(false);
	log_.Error("Trying to get uncontained geometry!");
	return (-1);
}

const xform& ChunkyPhysics::GetTransformation(const ChunkyBoneGeometry* geometry) const {
	return (GetBoneTransformation(GetIndex(geometry)));
}

void ChunkyPhysics::ClearBoneGeometries(PhysicsManager* physics) {
	for (size_t x = 0; x < geometry_array_.size(); ++x) {
		ChunkyBoneGeometry* _geometry = geometry_array_[x];
		if (_geometry) {
			if (physics) {
				_geometry->RemovePhysics(physics);
			}
			delete (_geometry);
			geometry_array_[x] = 0;
		}
	}
	geometry_array_.clear();
	uniqe_geometry_index_ = 0;
}

void ChunkyPhysics::EnableGravity(PhysicsManager* physics_manager, bool enable) {
	for (size_t x = 0; x < geometry_array_.size(); ++x) {
		ChunkyBoneGeometry* _geometry = geometry_array_[x];
		if (!_geometry || GetBodyType(_geometry) == PhysicsManager::kStatic) {
			continue;
		}
		PhysicsManager::BodyID _body_id = _geometry->GetBodyId();
		physics_manager->EnableGravity(_body_id, enable);
	}
}


int ChunkyPhysics::GetEngineCount() const {
	return ((int)engine_array_.size());
}

PhysicsEngine* ChunkyPhysics::GetEngine(int engine_index) const {
	deb_assert((size_t)engine_index < engine_array_.size());
	return (engine_array_[engine_index]);
}

int ChunkyPhysics::GetEngineIndexFromControllerIndex(int start_engine_index, int engine_step, unsigned controller_index) const {
	for (int x = start_engine_index; x >= 0 && x < (int)engine_array_.size(); x += engine_step) {
		if (engine_array_[x]->GetControllerIndex() == controller_index) {
			return x;
		}
	}
	return -1;
}

int ChunkyPhysics::GetEngineIndex(const PhysicsEngine* engine) const {
	for (size_t x = 0; x < engine_array_.size(); ++x) {
		if (engine_array_[x] == engine) {
			return (int)x;
		}
	}
	return -1;
}

void ChunkyPhysics::AddEngine(PhysicsEngine* engine) {
	engine_array_.push_back(engine);
}

void ChunkyPhysics::RemoveEngine(PhysicsEngine* engine) {
	engine_array_.erase(std::remove(engine_array_.begin(), engine_array_.end(), engine), engine_array_.end());
}

bool ChunkyPhysics::SetEnginePower(unsigned aspect, float power) {
	bool ok = false;
	EngineArray::iterator x = engine_array_.begin();
	for (; x != engine_array_.end(); ++x) {
		ok |= (*x)->SetValue(aspect, power);
	}
	return ok;
}

void ChunkyPhysics::ClearEngines() {
	EngineArray::iterator x = engine_array_.begin();
	for (; x != engine_array_.end(); ++x) {
		delete (*x);
	}
	engine_array_.clear();
}



int ChunkyPhysics::GetTriggerCount() const {
	return ((int)trigger_array_.size());
}

const PhysicsTrigger* ChunkyPhysics::GetTrigger(int trigger_index) const {
	deb_assert((size_t)trigger_index < trigger_array_.size());
	return (trigger_array_[trigger_index]);
}

void ChunkyPhysics::AddTrigger(PhysicsTrigger* trigger) {
	trigger_array_.push_back(trigger);
}

void ChunkyPhysics::ClearTriggers() {
	TriggerArray::iterator x = trigger_array_.begin();
	for (; x != trigger_array_.end(); ++x) {
		delete (*x);
	}
	trigger_array_.clear();
}



int ChunkyPhysics::GetSpawnerCount() const {
	return (int)spawner_array_.size();
}

const PhysicsSpawner* ChunkyPhysics::GetSpawner(int spawner_index) const {
	deb_assert((size_t)spawner_index < spawner_array_.size());
	return spawner_array_[spawner_index];
}

void ChunkyPhysics::AddSpawner(PhysicsSpawner* spawner) {
	spawner_array_.push_back(spawner);
}

void ChunkyPhysics::ClearSpawners() {
	SpawnerArray::iterator x = spawner_array_.begin();
	for (; x != spawner_array_.end(); ++x) {
		delete (*x);
	}
	spawner_array_.clear();
}



void ChunkyPhysics::ClearAll(PhysicsManager* physics) {
	ClearBoneGeometries(physics);
	BoneHierarchy::ClearAll(physics);
	ClearEngines();
	ClearTriggers();
	ClearSpawners();
}

void ChunkyPhysics::SetBoneCount(int bone_count) {
	BoneHierarchy::SetBoneCount(bone_count);

	deb_assert(geometry_array_.empty());
	geometry_array_.clear();

	uniqe_geometry_index_ = GetBoneCount();
}

bool ChunkyPhysics::FinalizeInit(PhysicsManager* physics, unsigned physics_fps, const xform* transform, int force_listener_id) {
	bool ok = ((int)geometry_array_.size() == GetBoneCount());
	deb_assert(ok);
	if (ok) {
		if (transform) {
			const int root = 0;
			xform _transformation = GetOriginalBoneTransformation(root);
			_transformation = *transform * _transformation;
			SetOriginalBoneTransformation(root, _transformation);
		}
		ok = Parent::FinalizeInit(transform_operation_);
	}
	if (ok && physics) {
		const int _bone_count = GetBoneCount();
		for (int x = 0; ok && x < _bone_count; ++x) {
			ChunkyBoneGeometry* _geometry = GetBoneGeometry(x);
			switch (_geometry->GetBoneType()) {
				case ChunkyBoneGeometry::kBoneBody:
				case ChunkyBoneGeometry::kBoneTrigger: {
					const PhysicsManager::BodyType body_type = GetBodyType(_geometry);
					const xform& bone = GetBoneTransformation(x);
					ok = _geometry->CreateBody(physics, x == 0, force_listener_id, body_type, bone);
					if (ok) {
						physics->EnableGravity(_geometry->GetBodyId(), _geometry->IsAffectedByGravity());
					}
					if (ok) {
						physics->EnableCollideWithSelf(_geometry->GetBodyId(), _geometry->IsCollideWithSelf());
					}
				} break;
			}
		}
		for (int x = 0; ok && x < _bone_count; ++x) {
			ChunkyBoneGeometry* _geometry = GetBoneGeometry(x);
			ok = _geometry->CreateJoint(this, physics, physics_fps);
		}
	}
	deb_assert(ok);
	return (ok);
}



unsigned ChunkyPhysics::GetNextGeometryIndex() {
	return (++uniqe_geometry_index_);
}



loginstance(kPhysics, ChunkyPhysics);



}
