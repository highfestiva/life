
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/PhysicsTrigger.h"
#include <assert.h>
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Packer.h"
#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyPhysics.h"



namespace TBC
{



PhysicsTrigger::PhysicsTrigger(TriggerType pTriggerType):
	mTriggerType(pTriggerType)
{
}

PhysicsTrigger::~PhysicsTrigger()
{
}



PhysicsTrigger* PhysicsTrigger::Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount)
{
	const uint32* lData = (const uint32*)pData;
	if (pByteCount < sizeof(uint32)*3 + Endian::BigToHost(lData[2])*sizeof(uint32))
	{
		mLog.AError("Could not load; wrong data size.");
		assert(false);
		return (0);
	}

	PhysicsTrigger* lTrigger = new PhysicsTrigger(TRIGGER_TOGGLE);
	lTrigger->LoadChunkyData(pStructure, pData);
	if (lTrigger->GetChunkySize() != pByteCount)
	{
		assert(false);
		mLog.AError("Corrupt data or error in loading algo.");
		delete (lTrigger);
		lTrigger = 0;
	}
	return (lTrigger);
}



PhysicsTrigger::TriggerType PhysicsTrigger::GetTriggerType() const
{
	return (mTriggerType);
}

PhysicsManager::TriggerID PhysicsTrigger::GetTriggerId() const
{
	assert(mTriggerNode->GetTriggerId() != 0);
	return (mTriggerNode->GetTriggerId());
}


void PhysicsTrigger::SetTriggerGeometry(ChunkyBoneGeometry* pGeometry)
{
	mTriggerNode = pGeometry;
}

void PhysicsTrigger::AddControlledEngine(PhysicsEngine* pEngine, float pDelay, str pFunction)
{
	Connection lConnection;
	lConnection.mEngine = pEngine;
	lConnection.mDelay = pDelay;
	lConnection.mFunction = pFunction;
	mConnectionArray.push_back(lConnection);
}

int PhysicsTrigger::GetControlledEngineCount() const
{
	return (mConnectionArray.size());
}

PhysicsEngine* PhysicsTrigger::GetControlledEngine(int pIndex) const
{
	assert((size_t)pIndex < mConnectionArray.size());
	return (mConnectionArray[pIndex].mEngine);
}

str PhysicsTrigger::GetControlledFunction(int pIndex) const
{
	assert((size_t)pIndex < mConnectionArray.size());
	return (mConnectionArray[pIndex].mFunction);
}



unsigned PhysicsTrigger::GetChunkySize() const
{
	size_t lStringSize = 0;
	ConnectionArray::const_iterator x = mConnectionArray.begin();
	for (; x != mConnectionArray.end(); ++x)
	{
		lStringSize += PackerUnicodeString::Pack(0, wstrutil::ToOwnCode(x->mFunction));
	}
	return ((unsigned)(sizeof(uint32) * 3 +
		(sizeof(uint32)+sizeof(float)) * mConnectionArray.size() +
		lStringSize));
}

void PhysicsTrigger::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	uint32* lData = (uint32*)pData;
	lData[0] = Endian::HostToBig(GetTriggerType());
	lData[1] = Endian::HostToBig(pStructure->GetIndex(mTriggerNode));
	lData[2] = Endian::HostToBig((uint32)mConnectionArray.size());
	int y = 3;
	for (int x = 0; x < (int)mConnectionArray.size(); ++x)
	{
		const Connection& lConnection = mConnectionArray[x];
		lData[y++] = Endian::HostToBig(pStructure->GetEngineIndex(lConnection.mEngine));
		lData[y++] = Endian::HostToBigF(lConnection.mDelay);
		int lStringRawLength = PackerUnicodeString::Pack((uint8*)&lData[y], wstrutil::ToOwnCode(lConnection.mFunction));
		assert(lStringRawLength % sizeof(lData[0]) == 0);
		y += lStringRawLength / sizeof(lData[0]);
	}
}

void PhysicsTrigger::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	const uint32* lData = (const uint32*)pData;

	mTriggerType = (TriggerType)Endian::BigToHost(lData[0]);
	SetTriggerGeometry(pStructure->GetBoneGeometry(Endian::BigToHost(lData[1])));
	assert(mTriggerNode);
	const int lEngineCount = Endian::BigToHost(lData[2]);
	int y = 3;
	for (int x = 0; x < lEngineCount; ++x)
	{
		PhysicsEngine* lEngine = pStructure->GetEngine(Endian::BigToHost(lData[y++]));
		assert(lEngine);
		float lDelay = Endian::BigToHostF(lData[y++]);
		wstr lFunction;
		int lStringRawLength = PackerUnicodeString::Unpack(&lFunction, (uint8*)&lData[y], 1024);
		assert(lStringRawLength % sizeof(lData[0]) == 0);
		y += lStringRawLength / sizeof(lData[0]);
		AddControlledEngine(lEngine, lDelay, wstrutil::ToCurrentCode(lFunction));
	}
}



LOG_CLASS_DEFINE(PHYSICS, PhysicsTrigger);



}
