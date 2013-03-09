
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/PhysicsSpawner.h"
#include <assert.h>
#include "../../Lepra/Include/Endian.h"
#include "../../Lepra/Include/Packer.h"
#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyPhysics.h"



namespace TBC
{



PhysicsSpawner::PhysicsSpawner():
	mSpawnerType(SPAWNER_INVALID),
	mSpawnerNode(0)
{
}

PhysicsSpawner::~PhysicsSpawner()
{
}

void PhysicsSpawner::RelocatePointers(const ChunkyPhysics* pTarget, const ChunkyPhysics* pSource, const PhysicsSpawner& pOriginal)
{
	const int lBoneIndex = pSource->GetIndex(pOriginal.mSpawnerNode);
	assert(lBoneIndex >= 0);
	mSpawnerNode = pTarget->GetBoneGeometry(lBoneIndex);
}



PhysicsSpawner* PhysicsSpawner::Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount)
{
	if (pByteCount < sizeof(uint32)*6 /*+ ...*/)
	{
		mLog.AError("Could not load; wrong data size.");
		assert(false);
		return (0);
	}

	PhysicsSpawner* lSpawner = new PhysicsSpawner;
	lSpawner->LoadChunkyData(pStructure, pData);
	if (lSpawner->GetChunkySize() != pByteCount)
	{
		mLog.AError("Corrupt data or error in loading algo.");
		assert(false);
		delete (lSpawner);
		lSpawner = 0;
	}
	return (lSpawner);
}



PhysicsSpawner::Type PhysicsSpawner::GetType() const
{
	return (mSpawnerType);
}

const str& PhysicsSpawner::GetFunction() const
{
	return (mFunction);
}

TransformationF PhysicsSpawner::GetSpawnPoint(const ChunkyPhysics* pStructure, const Vector3DF& pScaledPoint) const
{
	assert(mSpawnerNode);
	assert(mSpawnerNode->GetGeometryType() == ChunkyBoneGeometry::GEOMETRY_BOX);
	TBC::ChunkyBoneBox* lSpawnBox = (TBC::ChunkyBoneBox*)mSpawnerNode;
	Vector3DF lPoint = lSpawnBox->GetShapeSize();
	lPoint.x = (pScaledPoint.x-0.5f) * lPoint.x;
	lPoint.y = (pScaledPoint.y-0.5f) * lPoint.y;
	lPoint.z = (pScaledPoint.z-0.5f) * lPoint.z;
	const TransformationF& lTransformation = pStructure->GetTransformation(mSpawnerNode);
	lPoint = lTransformation.GetOrientation() * lPoint;
	return TransformationF(lTransformation.GetOrientation(), lTransformation.GetPosition()+lPoint);
}

float PhysicsSpawner::GetNumber() const
{
	return mNumber;
}

const PhysicsSpawner::IntervalArray& PhysicsSpawner::GetIntervals() const
{
	return mIntervalArray;
}

const str PhysicsSpawner::GetSpawnObject(float pProbabilityThreshold) const
{
	SpawnObjectArray::const_iterator x = mSpawnObjectArray.begin();
	for (; x != mSpawnObjectArray.end(); ++x)
	{
		if (pProbabilityThreshold <= x->mProbability)
		{
			return x->mSpawnObject;
		}
		pProbabilityThreshold -= x->mProbability;
	}
	return str();
}



unsigned PhysicsSpawner::GetChunkySize() const
{
	size_t lStringSize = PackerUnicodeString::Pack(0, wstrutil::Encode(mFunction));
	SpawnObjectArray::const_iterator x = mSpawnObjectArray.begin();
	for (; x != mSpawnObjectArray.end(); ++x)
	{
		lStringSize += PackerUnicodeString::Pack(0, wstrutil::Encode(x->mSpawnObject));
	}
	return ((unsigned)(sizeof(uint32) * 4 +
		sizeof(float) +
		sizeof(float) * mIntervalArray.size() +
		sizeof(float) * mSpawnObjectArray.size() +
		lStringSize));
}

void PhysicsSpawner::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	uint32* lData = (uint32*)pData;
	int i = 0;
	lData[i++] = Endian::HostToBig(mSpawnerType);
	i += PackerUnicodeString::Pack((uint8*)&lData[i], wstrutil::Encode(mFunction));
	lData[i++] = Endian::HostToBig(pStructure->GetIndex(mSpawnerNode));
	lData[i++] = Endian::HostToBigF(mNumber);
	lData[i++] = Endian::HostToBig((uint32)mIntervalArray.size());
	IntervalArray::const_iterator y = mIntervalArray.begin();
	for (; y != mIntervalArray.end(); ++y)
	{
		lData[i++] = Endian::HostToBigF(*y);
	}
	SpawnObjectArray::const_iterator x = mSpawnObjectArray.begin();
	for (; x != mSpawnObjectArray.end(); ++x)
	{
		int lStringRawLength = PackerUnicodeString::Pack((uint8*)&lData[i], wstrutil::Encode(x->mSpawnObject));
		assert(lStringRawLength % sizeof(lData[0]) == 0);
		i += lStringRawLength / sizeof(lData[0]);
		lData[i++] = Endian::HostToBigF(x->mProbability);
	}
}

void PhysicsSpawner::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	const uint32* lData = (const uint32*)pData;

	int i = 0;
	mSpawnerType = (Type)Endian::BigToHost(lData[i++]);
	i += PackerUnicodeString::Unpack(mFunction, (uint8*)&lData[i], 1024) / sizeof(lData[0]);
	const int lBodyIndex = Endian::BigToHost(lData[i++]);
	mSpawnerNode = pStructure->GetBoneGeometry(lBodyIndex);
	mNumber = Endian::BigToHostF(lData[i++]);
	const int lIntervalCount = Endian::BigToHost(lData[i++]);
	for (int x = 0; x < lIntervalCount; ++x)
	{
		mIntervalArray.push_back(Endian::BigToHostF(lData[i++]));
	}
	const int lSpawnObjectCount = Endian::BigToHost(lData[i++]);
	for (int x = 0; x < lSpawnObjectCount; ++x)
	{
		str lSpawnObject;
		const int lStringRawLength = PackerUnicodeString::Unpack(lSpawnObject, (uint8*)&lData[i], 1024);
		assert(lStringRawLength % sizeof(lData[0]) == 0);
		i += lStringRawLength / sizeof(lData[0]);
		const float lProbability = Endian::BigToHostF(lData[i++]);
		mSpawnObjectArray.push_back(SpawnObject(strutil::Encode(lSpawnObject), lProbability));
	}
}



PhysicsSpawner::SpawnObject::SpawnObject(const str& pSpawnObject, float pProbability):
	mSpawnObject(pSpawnObject),
	mProbability(pProbability)
{
}



LOG_CLASS_DEFINE(PHYSICS, PhysicsSpawner);



}
