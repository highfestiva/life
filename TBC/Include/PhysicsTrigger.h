
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
class PhysicsEngine;



class PhysicsTrigger
{
public:
	enum Type
	{
		TRIGGER_INVALID = 0,
		TRIGGER_ALWAYS,
		TRIGGER_MOVEMENT,
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

	static PhysicsTrigger* Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount);

	Type GetType() const;
	PhysicsManager::TriggerID GetPhysicsTriggerId() const;
	int GetGroupIndex() const;
	int GetPriority() const;
	const str& GetTypeName() const;

	void SetTriggerGeometry(ChunkyBoneGeometry* pGeometry);
	void AddControlledEngine(PhysicsEngine* pEngine, float pDelay, str pFunction);
	int GetControlledEngineCount() const;
	const EngineTrigger& GetControlledEngine(int pIndex) const;

	unsigned GetChunkySize() const;
	void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

private:
	void LoadChunkyData(ChunkyPhysics* pStructure, const void* pData);

	typedef std::vector<EngineTrigger> ConnectionArray;

	Type mTriggerType;
	int mGroupIndex;
	int mPriority;
	str mTypeName;
	ChunkyBoneGeometry* mTriggerNode;
	ConnectionArray mConnectionArray;

	LOG_CLASS_DECLARE();
};



}
