
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "../Include/PhysicsSpawner.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/Endian.h"
#include "../../Lepra/Include/Packer.h"
#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyPhysics.h"



namespace TBC
{



PhysicsSpawner::PhysicsSpawner():
	mSpawnerType(SPAWNER_INVALID)
{
}

PhysicsSpawner::~PhysicsSpawner()
{
}

void PhysicsSpawner::RelocatePointers(const ChunkyPhysics* pTarget, const ChunkyPhysics* pSource, const PhysicsSpawner& pOriginal)
{
	const int lCount = pOriginal.GetSpawnPointCount();
	mSpawnerNodeArray.clear();
	for (int x = 0; x < lCount; ++x)
	{
		const int lBoneIndex = pSource->GetIndex(pOriginal.mSpawnerNodeArray[x]);
		deb_assert(lBoneIndex >= 0);
		mSpawnerNodeArray.push_back(pTarget->GetBoneGeometry(lBoneIndex));
	}
}



PhysicsSpawner* PhysicsSpawner::Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount)
{
	if (pByteCount < sizeof(uint32)*6 /*+ ...*/)
	{
		mLog.AError("Could not load; wrong data size.");
		deb_assert(false);
		return (0);
	}

	PhysicsSpawner* lSpawner = new PhysicsSpawner;
	lSpawner->LoadChunkyData(pStructure, pData);
	if (lSpawner->GetChunkySize() != pByteCount)
	{
		mLog.AError("Corrupt data or error in loading algo.");
		deb_assert(false);
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

int PhysicsSpawner::GetSpawnPointCount() const
{
	return mSpawnerNodeArray.size();
}

TransformationF PhysicsSpawner::GetSpawnPoint(const ChunkyPhysics* pStructure, const Vector3DF& pScaledPoint, int pIndex) const
{
	deb_assert((size_t)pIndex < mSpawnerNodeArray.size() && pIndex >= 0);
	deb_assert(mSpawnerNodeArray[pIndex]->GetGeometryType() == ChunkyBoneGeometry::GEOMETRY_BOX);
	TBC::ChunkyBoneBox* lSpawnBox = (TBC::ChunkyBoneBox*)mSpawnerNodeArray[pIndex];
	Vector3DF lPoint = lSpawnBox->GetShapeSize();
	lPoint.x = (pScaledPoint.x-0.5f) * lPoint.x;
	lPoint.y = (pScaledPoint.y-0.5f) * lPoint.y;
	lPoint.z = (pScaledPoint.z-0.5f) * lPoint.z;
	const TransformationF& lTransformation = pStructure->GetTransformation(lSpawnBox);
	QuaternionF q = lTransformation.GetOrientation();
	lPoint = q * lPoint;
	q.RotateAroundWorldZ(PIF);	// This is so since the level ("the spawner") has a 180 degree orientation offset compared to objects.
	return TransformationF(q, lTransformation.GetPosition()+lPoint);
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
		sizeof(uint32) * mSpawnerNodeArray.size() +
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
	lData[i++] = Endian::HostToBig((uint32)mSpawnerNodeArray.size());
	for (int z = 0; (size_t)z < mSpawnerNodeArray.size(); ++z)
	{
		lData[i++] = Endian::HostToBig(pStructure->GetIndex(mSpawnerNodeArray[z]));
	}
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
		deb_assert(lStringRawLength % sizeof(lData[0]) == 0);
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
	mSpawnerNodeArray.clear();
	const int lSpawnerNodeCount = Endian::BigToHost(lData[i++]);
	for (int x = 0; x < lSpawnerNodeCount; ++x)
	{
		const int lBodyIndex = Endian::BigToHost(lData[i++]);
		mSpawnerNodeArray.push_back(pStructure->GetBoneGeometry(lBodyIndex));
	}
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
		deb_assert(lStringRawLength % sizeof(lData[0]) == 0);
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
