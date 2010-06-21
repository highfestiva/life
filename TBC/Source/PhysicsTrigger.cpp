
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



bool PhysicsTrigger::EngineTrigger::operator==(const EngineTrigger& pOther) const
{
	return (mEngine == pOther.mEngine &&
		mDelay == pOther.mDelay &&
		mFunction == pOther.mFunction);
}

size_t PhysicsTrigger::EngineTrigger::Hash() const
{
	return ((size_t)mEngine + *(int*)&mDelay + HashString(mFunction.c_str()));
}



PhysicsTrigger::PhysicsTrigger():
	mTriggerType(TRIGGER_INVALID),
	mGroupIndex(-1),
	mPriority(-1),
	mTriggerNode(0)
{
}

PhysicsTrigger::~PhysicsTrigger()
{
}



PhysicsTrigger* PhysicsTrigger::Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount)
{
	if (pByteCount < sizeof(uint32)*6 /*+ ...*/)
	{
		mLog.AError("Could not load; wrong data size.");
		assert(false);
		return (0);
	}

	PhysicsTrigger* lTrigger = new PhysicsTrigger;
	lTrigger->LoadChunkyData(pStructure, pData);
	if (lTrigger->GetChunkySize() != pByteCount)
	{
		mLog.AError("Corrupt data or error in loading algo.");
		assert(false);
		delete (lTrigger);
		lTrigger = 0;
	}
	return (lTrigger);
}



PhysicsTrigger::Type PhysicsTrigger::GetType() const
{
	return (mTriggerType);
}

PhysicsManager::TriggerID PhysicsTrigger::GetPhysicsTriggerId() const
{
	if (!mTriggerNode)
	{
		return (TBC::INVALID_TRIGGER);
	}
	assert(mTriggerNode->GetTriggerId() != 0);
	return (mTriggerNode->GetTriggerId());
}

int PhysicsTrigger::GetGroupIndex() const
{
	return (mGroupIndex);
}

int PhysicsTrigger::GetPriority() const
{
	return (mPriority);
}

const str& PhysicsTrigger::GetTypeName() const
{
	return (mTypeName);
}



void PhysicsTrigger::SetTriggerGeometry(ChunkyBoneGeometry* pGeometry)
{
	mTriggerNode = pGeometry;
}

void PhysicsTrigger::AddControlledEngine(PhysicsEngine* pEngine, float pDelay, str pFunction)
{
	EngineTrigger lConnection;
	lConnection.mEngine = pEngine;
	lConnection.mDelay = pDelay;
	lConnection.mFunction = pFunction;
	mConnectionArray.push_back(lConnection);
}

int PhysicsTrigger::GetControlledEngineCount() const
{
	return (mConnectionArray.size());
}

const PhysicsTrigger::EngineTrigger& PhysicsTrigger::GetControlledEngine(int pIndex) const
{
	assert((size_t)pIndex < mConnectionArray.size());
	return (mConnectionArray[pIndex]);
}



unsigned PhysicsTrigger::GetChunkySize() const
{
	size_t lStringSize = PackerUnicodeString::Pack(0, wstrutil::Encode(mTypeName));
	ConnectionArray::const_iterator x = mConnectionArray.begin();
	for (; x != mConnectionArray.end(); ++x)
	{
		lStringSize += PackerUnicodeString::Pack(0, wstrutil::Encode(x->mFunction));
	}
	return ((unsigned)(sizeof(uint32) * 5 +
		(sizeof(uint32)+sizeof(float)) * mConnectionArray.size() +
		lStringSize));
}

void PhysicsTrigger::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	uint32* lData = (uint32*)pData;
	int i = 0;
	lData[i++] = Endian::HostToBig(mTriggerType);
	i += PackerUnicodeString::Pack((uint8*)&lData[i], wstrutil::Encode(mTypeName));
	lData[i++] = Endian::HostToBig(mGroupIndex);
	lData[i++] = Endian::HostToBig(mPriority);
	lData[i++] = Endian::HostToBig(pStructure->GetIndex(mTriggerNode));
	lData[i++] = Endian::HostToBig((uint32)mConnectionArray.size());
	for (int x = 0; x < (int)mConnectionArray.size(); ++x)
	{
		const EngineTrigger& lConnection = mConnectionArray[x];
		lData[i++] = Endian::HostToBig(pStructure->GetEngineIndex(lConnection.mEngine));
		lData[i++] = Endian::HostToBigF(lConnection.mDelay);
		int lStringRawLength = PackerUnicodeString::Pack((uint8*)&lData[i], wstrutil::Encode(lConnection.mFunction));
		assert(lStringRawLength % sizeof(lData[0]) == 0);
		i += lStringRawLength / sizeof(lData[0]);
	}
}

void PhysicsTrigger::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	const uint32* lData = (const uint32*)pData;

	int i = 0;
	mTriggerType = (Type)Endian::BigToHost(lData[i++]);
	i += PackerUnicodeString::Unpack(mTypeName, (uint8*)&lData[i], 1024) / sizeof(lData[0]);
	mGroupIndex = Endian::BigToHost(lData[i++]);
	mPriority = Endian::BigToHost(lData[i++]);
	int lPhysTriggerIndex = Endian::BigToHost(lData[i++]);
	if (lPhysTriggerIndex >= 0)
	{
		SetTriggerGeometry(pStructure->GetBoneGeometry(lPhysTriggerIndex));
		assert(mTriggerNode);
	}
	const int lEngineCount = Endian::BigToHost(lData[i++]);
	for (int x = 0; x < lEngineCount; ++x)
	{
		PhysicsEngine* lEngine = pStructure->GetEngine(Endian::BigToHost(lData[i++]));
		assert(lEngine);
		float lDelay = Endian::BigToHostF(lData[i++]);
		str lFunction;
		int lStringRawLength = PackerUnicodeString::Unpack(lFunction, (uint8*)&lData[i], 1024);
		assert(lStringRawLength % sizeof(lData[0]) == 0);
		i += lStringRawLength / sizeof(lData[0]);
		AddControlledEngine(lEngine, lDelay, strutil::Encode(lFunction));
	}
}



LOG_CLASS_DEFINE(PHYSICS, PhysicsTrigger);



}
