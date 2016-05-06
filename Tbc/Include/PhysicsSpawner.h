
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <vector>
#include "../../lepra/include/log.h"
#include "../../lepra/include/vector3d.h"
#include "../include/physicsmanager.h"
#include "../include/tbc.h"



namespace tbc {



class ChunkyBoneGeometry;
class ChunkyPhysics;



class PhysicsSpawner {
public:
	typedef std::vector<float> IntervalArray;
	enum Type {
		kSpawnerInvalid = 0,
		kSpawnerTeleport,
		kSpawnerCreator,
	};

	PhysicsSpawner();
	virtual ~PhysicsSpawner();
	void RelocatePointers(const ChunkyPhysics* target, const ChunkyPhysics* source, const PhysicsSpawner& original);

	static PhysicsSpawner* Load(ChunkyPhysics* structure, const void* data, unsigned byte_count);

	Type GetType() const;
	const str& GetFunction() const;
	int GetSpawnPointCount() const;
	xform GetSpawnPoint(const ChunkyPhysics* structure, const vec3& scaled_point, int index, vec3& initial_velocity) const;
	float GetNumber() const;
	const IntervalArray& GetIntervals() const;
	const str GetSpawnObject(float probability_threshold) const;
	bool IsEaseDown() const;

	unsigned GetChunkySize() const;
	void SaveChunkyData(const ChunkyPhysics* structure, void* data) const;

private:
	void LoadChunkyData(ChunkyPhysics* structure, const void* data);

	struct SpawnObject {
		str spawn_object_;
		float probability_;
		SpawnObject(const str& spawn_object, float probability);
	};
	typedef std::vector<SpawnObject> SpawnObjectArray;
	typedef std::vector<ChunkyBoneGeometry*> SpawnNodeArray;

	Type spawner_type_;
	str function_;
	SpawnNodeArray spawner_node_array_;
	vec3 initial_velocity_;
	float number_;
	IntervalArray interval_array_;
	SpawnObjectArray spawn_object_array_;
	bool is_ease_down_;
	float total_object_probability_;

	logclass();
};



}
