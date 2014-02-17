
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	int GetSpawnPointCount() const;
	TransformationF GetSpawnPoint(const ChunkyPhysics* pStructure, const Vector3DF& pScaledPoint, int pIndex, Vector3DF& pInitialVelocity) const;
	float GetNumber() const;
	const IntervalArray& GetIntervals() const;
	const str GetSpawnObject(float pProbabilityThreshold) const;
	bool IsEaseDown() const;

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
	typedef std::vector<ChunkyBoneGeometry*> SpawnNodeArray;

	Type mSpawnerType;
	str mFunction;
	SpawnNodeArray mSpawnerNodeArray;
	Vector3DF mInitialVelocity;
	float mNumber;
	IntervalArray mIntervalArray;
	SpawnObjectArray mSpawnObjectArray;
	bool mIsEaseDown;
	float mTotalObjectProbability;

	LOG_CLASS_DECLARE();
};



}
