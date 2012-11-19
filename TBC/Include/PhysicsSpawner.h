
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <vector>
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../Include/PhysicsManager.h"
#include "../Include/TBC.h"



namespace TBC
{



class ChunkyBoneGeometry;
class ChunkyPhysics;



class PhysicsSpawner
{
public:
	typedef std::vector<float> IntervalArray;
	enum Type
	{
		SPAWNER_INVALID = 0,
		SPAWNER_TELEPORT,
		SPAWNER_CREATOR,
	};

	PhysicsSpawner();
	virtual ~PhysicsSpawner();
	void RelocatePointers(const ChunkyPhysics* pTarget, const ChunkyPhysics* pSource, const PhysicsSpawner& pOriginal);

	static PhysicsSpawner* Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount);

	Type GetType() const;
	const str& GetFunction() const;
	TransformationF GetSpawnPoint(const ChunkyPhysics* pStructure, const Vector3DF& pScaledPoint) const;
	float GetNumber() const;
	const IntervalArray& GetIntervals() const;
	const str GetSpawnObject(float pProbabilityThreshold) const;

	unsigned GetChunkySize() const;
	void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

private:
	void LoadChunkyData(ChunkyPhysics* pStructure, const void* pData);

	struct SpawnObject
	{
		str mSpawnObject;
		float mProbability;
		SpawnObject(const str& pSpawnObject, float pProbability);
	};
	typedef std::vector<SpawnObject> SpawnObjectArray;

	Type mSpawnerType;
	str mFunction;
	ChunkyBoneGeometry* mSpawnerNode;
	float mNumber;
	IntervalArray mIntervalArray;
	SpawnObjectArray mSpawnObjectArray;
	

	LOG_CLASS_DECLARE();
};



}
