
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
	enum TriggerType
	{
		TRIGGER_TOGGLE = 1,
		TRIGGER_MINIMUM,
		TRIGGER_MAXIMUM,
	};

	PhysicsTrigger(TriggerType pTriggerType);
	virtual ~PhysicsTrigger();

	static PhysicsTrigger* Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount);

	TriggerType GetTriggerType() const;
	PhysicsManager::TriggerID GetTriggerId() const;

	void SetTriggerGeometry(ChunkyBoneGeometry* pGeometry);
	void AddControlledEngine(PhysicsEngine* pEngine, float pDelay, str pFunction);
	int GetControlledEngineCount() const;
	PhysicsEngine* GetControlledEngine(int pIndex) const;
	str GetControlledFunction(int pIndex) const;

	unsigned GetChunkySize() const;
	void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

private:
	void LoadChunkyData(ChunkyPhysics* pStructure, const void* pData);

	struct Connection
	{
		PhysicsEngine* mEngine;
		float mDelay;
		str mFunction;
	};
	typedef std::vector<Connection> ConnectionArray;

	TriggerType mTriggerType;
	ChunkyBoneGeometry* mTriggerNode;
	ConnectionArray mConnectionArray;

	LOG_CLASS_DECLARE();
};



}
