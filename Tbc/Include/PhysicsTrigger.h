
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <vector>
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../Include/PhysicsManager.h"
#include "../Include/Tbc.h"



namespace Tbc
{



class ChunkyBoneGeometry;
class ChunkyPhysics;
class PhysicsEngine;



class PhysicsTrigger
{
public:
	enum Type
	{
		TRIGGER_INVALID = 0,
		TRIGGER_NON_STOP,	// Sorta like a "trampoline", which needs to push, even if at top.
		TRIGGER_ALWAYS,		// Trigger after delay, "timer triggered".
		TRIGGER_MOVEMENT,	// Triggered by something physical.
	};

	struct EngineTrigger
	{
		PhysicsEngine* mEngine;
		float mDelay;
		str mFunction;
		bool operator==(const EngineTrigger& pOther) const;
		size_t Hash() const;
	};

	PhysicsTrigger();
	virtual ~PhysicsTrigger();
	void RelocatePointers(const ChunkyPhysics* pTarget, const ChunkyPhysics* pSource, const PhysicsTrigger& pOriginal);

	static PhysicsTrigger* Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount);

	Type GetType() const;
	PhysicsManager::TriggerID GetPhysicsTriggerId(int pTriggerGeometryIndex) const;
	//int GetGroupIndex() const;
	int GetPriority() const;
	const str& GetFunction() const;

	void AddTriggerGeometry(ChunkyBoneGeometry* pGeometry);
	int GetTriggerGeometryCount() const;
	ChunkyBoneGeometry* GetTriggerGeometry(int pIndex) const;
	void AddControlledEngine(PhysicsEngine* pEngine, float pDelay, str pFunction);
	int GetControlledEngineCount() const;
	const EngineTrigger& GetControlledEngine(int pIndex) const;

	unsigned GetChunkySize() const;
	void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

private:
	void LoadChunkyData(ChunkyPhysics* pStructure, const void* pData);

	typedef std::vector<ChunkyBoneGeometry*> BoneTriggerArray;
	typedef std::vector<EngineTrigger> ConnectionArray;

	Type mTriggerType;
	//int mGroupIndex;
	int mPriority;
	str mFunction;
	//ChunkyBoneGeometry* mTriggerNode;
	BoneTriggerArray mTriggerArray;
	ConnectionArray mConnectionArray;

	logclass();
};



}
