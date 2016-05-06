
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "tbc.h"
#include "../include/bones.h"
#include "../include/physicsmanager.h"



namespace tbc {



class ChunkyBoneGeometry;
class PhysicsEngine;
class PhysicsSpawner;
class PhysicsTrigger;



class ChunkyPhysics: public BoneHierarchy {
	typedef BoneHierarchy Parent;
public:
	enum PhysicsType {
		kWorld = 1,	// World object (the level itself).
		kStatic,		// Static object (turret).
		kDynamic		// Dynamic object within the world (car).
	};
	enum GuideMode {
		kGuideNever = 0,
		kGuideExternal,		// Guiding happens (e.g. child steering).
		kGuideAlways,
	};

	ChunkyPhysics(TransformOperation transform_operation = kTransformNone, PhysicsType physics_type = kStatic);
	ChunkyPhysics(const ChunkyPhysics& original);
	virtual ~ChunkyPhysics();

	void OnMicroTick(PhysicsManager* physics_manager, float frame_time);

	void SetTransformOperation(TransformOperation operation);
	PhysicsType GetPhysicsType() const;
	void SetPhysicsType(PhysicsType physics_type);	// You may use this to change into a ragdoll (COLLISION_DETECT_ONLY -> kDynamic), or back.
	GuideMode GetGuideMode() const;	// Gets help with automatic motions, such as flipping over.
	void SetGuideMode(GuideMode guide_mode);
	float QueryTotalMass(PhysicsManager* physics_manager) const;

	ChunkyBoneGeometry* GetBoneGeometry(int bone_index) const;
	ChunkyBoneGeometry* GetBoneGeometry(PhysicsManager::BodyID body_id) const;
	void AddBoneGeometry(ChunkyBoneGeometry* geometry);	// Takes ownership of the added geometry.
	void AddBoneGeometry(const xform& transformation, ChunkyBoneGeometry* geometry,
		const ChunkyBoneGeometry* parent = 0);	// Takes ownership of the added geometry.
	PhysicsManager::BodyType GetBodyType(const ChunkyBoneGeometry* geometry) const;
	int GetIndex(const ChunkyBoneGeometry* geometry) const;
	const xform& GetTransformation(const ChunkyBoneGeometry* geometry) const;
	void ClearBoneGeometries(PhysicsManager* physics);
	void EnableGravity(PhysicsManager* physics_manager, bool enable);

	int GetEngineCount() const;
	PhysicsEngine* GetEngine(int engine_index) const;
	int GetEngineIndexFromControllerIndex(int start_engine_index, int engine_step, unsigned controller_index) const;
	int GetEngineIndex(const PhysicsEngine* engine) const;
	void AddEngine(PhysicsEngine* engine);	// Takes ownership of the given engine.
	void RemoveEngine(PhysicsEngine* engine);
	bool SetEnginePower(unsigned aspect, float power);
	void ClearEngines();

	int GetTriggerCount() const;
	const PhysicsTrigger* GetTrigger(int trigger_index) const;
	void AddTrigger(PhysicsTrigger* trigger);	// Takes ownership of the given trigger.
	void ClearTriggers();

	int GetSpawnerCount() const;
	const PhysicsSpawner* GetSpawner(int spawner_index) const;
	void AddSpawner(PhysicsSpawner* spawner);	// Takes ownership of the given spawner.
	void ClearSpawners();

	// Overrides.
	void ClearAll(PhysicsManager* physics);
	void SetBoneCount(int bone_count);
	bool FinalizeInit(PhysicsManager* physics, unsigned physics_fps, const xform* transform, int force_listener_id);

	unsigned GetNextGeometryIndex();

protected:
private:
	typedef std::vector<ChunkyBoneGeometry*> GeometryArray;
	typedef std::vector<PhysicsEngine*> EngineArray;
	typedef std::vector<PhysicsSpawner*> SpawnerArray;
	typedef std::vector<PhysicsTrigger*> TriggerArray;
	GeometryArray geometry_array_;
	EngineArray engine_array_;
	TriggerArray trigger_array_;
	SpawnerArray spawner_array_;
	TransformOperation transform_operation_;
	PhysicsType physics_type_;
	GuideMode guide_mode_;
	unsigned uniqe_geometry_index_;

	logclass();
};



}
