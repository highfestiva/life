
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/PhysicsTrigger.h"
#include <assert.h>
#include "../../Lepra/Include/Math.h"
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
	if (pByteCount != sizeof(uint32)*3 + Endian::BigToHost(lData[2])*sizeof(uint32))
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



void PhysicsTrigger::SetTriggerGeometry(ChunkyBoneGeometry* pGeometry)
{
	mTriggerNode = pGeometry;
}

void PhysicsTrigger::AddControlledEngine(PhysicsEngine* pEngine)
{
	mEngineArray.push_back(pEngine);
}



unsigned PhysicsTrigger::GetChunkySize() const
{
	return ((unsigned)(sizeof(uint32)*3 + sizeof(uint32)*mEngineArray.size()));
}

void PhysicsTrigger::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	uint32* lData = (uint32*)pData;
	lData[0] = Endian::HostToBig(GetTriggerType());
	lData[1] = Endian::HostToBig(pStructure->GetIndex(mTriggerNode));
	lData[2] = Endian::HostToBig((uint32)mEngineArray.size());
	int y = 3;
	for (int x = 0; x < (int)mEngineArray.size(); ++x)
	{
		const PhysicsEngine* lEngine = mEngineArray[x];
		lData[y++] = Endian::HostToBig(pStructure->GetEngineIndex(lEngine));
	}
}

void PhysicsTrigger::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	const uint32* lData = (const uint32*)pData;

	mTriggerType = (TriggerType)Endian::BigToHost(lData[0]);
	mTriggerNode = pStructure->GetBoneGeometry(Endian::BigToHost(lData[1]));
	assert(mTriggerNode);
	const int lEngineCount = Endian::BigToHost(lData[2]);
	int y = 3;
	for (int x = 0; x < lEngineCount; ++x)
	{
		PhysicsEngine* lEngine = pStructure->GetEngine(Endian::BigToHost(lData[y++]));
		assert(lEngine);
		AddControlledEngine(lEngine);
	}
}



LOG_CLASS_DEFINE(PHYSICS, PhysicsTrigger);



}
