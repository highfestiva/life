
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <vector>
#include "../../lepra/include/log.h"
#include "../../lepra/include/vector3d.h"
#include "../include/physicsmanager.h"
#include "../include/tbc.h"



namespace tbc {



class ChunkyBoneGeometry;
class ChunkyPhysics;
class PhysicsEngine;



class PhysicsTrigger {
public:
	enum Type {
		kTriggerInvalid = 0,
		kTriggerNonStop,	// Sorta like a "trampoline", which needs to push, even if at top.
		kTriggerAlways,		// Trigger after delay, "timer triggered".
		kTriggerMovement,	// Triggered by something physical.
	};

	struct EngineTrigger {
		PhysicsEngine* engine_;
		float delay_;
		str function_;
		bool operator==(const EngineTrigger& other) const;
		size_t Hash() const;
	};

	PhysicsTrigger();
	virtual ~PhysicsTrigger();
	void RelocatePointers(const ChunkyPhysics* target, const ChunkyPhysics* source, const PhysicsTrigger& original);

	static PhysicsTrigger* Load(ChunkyPhysics* structure, const void* data, unsigned byte_count);

	Type GetType() const;
	PhysicsManager::BodyID GetPhysicsTriggerId(int trigger_geometry_index) const;
	//int GetGroupIndex() const;
	int GetPriority() const;
	const str& GetFunction() const;

	void AddTriggerGeometry(ChunkyBoneGeometry* geometry);
	int GetTriggerGeometryCount() const;
	ChunkyBoneGeometry* GetTriggerGeometry(int index) const;
	void AddControlledEngine(PhysicsEngine* engine, float delay, str function);
	int GetControlledEngineCount() const;
	const EngineTrigger& GetControlledEngine(int index) const;

	unsigned GetChunkySize() const;
	void SaveChunkyData(const ChunkyPhysics* structure, void* data) const;

private:
	void LoadChunkyData(ChunkyPhysics* structure, const void* data);

	typedef std::vector<ChunkyBoneGeometry*> BoneTriggerArray;
	typedef std::vector<EngineTrigger> ConnectionArray;

	Type trigger_type_;
	//int mGroupIndex;
	int priority_;
	str function_;
	//ChunkyBoneGeometry* mTriggerNode;
	BoneTriggerArray trigger_array_;
	ConnectionArray connection_array_;

	logclass();
};



}
