
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "TBC.h"
#include "../Include/Bones.h"
#include "../Include/PhysicsManager.h"



namespace TBC
{



class ChunkyBoneGeometry;
class PhysicsEngine;
class PhysicsTrigger;



class ChunkyPhysics: public BoneHierarchy
{
	typedef BoneHierarchy Parent;
public:
	enum PhysicsType
	{
		STATIC = 1,	// Static world object.
		DYNAMIC		// Dynamic object within the world.
	};

	ChunkyPhysics(TransformOperation pTransformOperation = TRANSFORM_NONE, PhysicsType pPhysicsType = STATIC);
	virtual ~ChunkyPhysics();

	void OnTick(PhysicsManager* pPhysicsManager, float pFrameTime);

	void SetPhysicsType(PhysicsType pPhysicsType);	// You may use this to change into a ragdoll (COLLISION_DETECT_ONLY -> DYNAMIC), or back.
	PhysicsType GetPhysicsType() const;

	ChunkyBoneGeometry* GetBoneGeometry(int pBoneIndex) const;
	ChunkyBoneGeometry* GetBoneGeometry(PhysicsManager::BodyID pBodyId) const;
	void AddBoneGeometry(ChunkyBoneGeometry* pGeometry);	// Takes ownership of the added geometry.
	void AddBoneGeometry(const TransformationF& pTransformation, ChunkyBoneGeometry* pGeometry,
		const ChunkyBoneGeometry* pParent = 0);	// Takes ownership of the added geometry.
	PhysicsManager::BodyType GetBodyType(const ChunkyBoneGeometry* pGeometry) const;
	int GetIndex(const ChunkyBoneGeometry* pGeometry) const;
	const TransformationF& GetTransformation(const ChunkyBoneGeometry* pGeometry) const;
	void ClearBoneGeometries(PhysicsManager* pPhysics);
	void EnableGravity(PhysicsManager* pPhysicsManager, bool pEnable);

	int GetEngineCount() const;
	PhysicsEngine* GetEngine(int pEngineIndex) const;
	int GetEngineIndex(const PhysicsEngine* pEngine) const;
	void AddEngine(PhysicsEngine* pEngine);	// Takes ownership of the given engine.
	void SetEnginePower(unsigned pAspect, float pPower, float pAngle);
	void ClearEngines();

	int GetTriggerCount() const;
	PhysicsTrigger* GetTrigger(int pTriggerIndex) const;
	void AddTrigger(PhysicsTrigger* pEngine);	// Takes ownership of the given trigger.
	void ClearTriggers();

	// Overrides.
	void ClearAll(PhysicsManager* pPhysics);
	void SetBoneCount(int pBoneCount);
	bool FinalizeInit(PhysicsManager* pPhysics, unsigned pPhysicsFps, Vector3DF* pPosition,
		PhysicsManager::TriggerListener* pTrigListener, PhysicsManager::ForceFeedbackListener* pForceListener);

	unsigned GetNextGeometryIndex();

protected:
private:
	typedef std::vector<ChunkyBoneGeometry*> GeometryArray;
	typedef std::vector<PhysicsEngine*> EngineArray;
	typedef std::vector<PhysicsTrigger*> TriggerArray;
	GeometryArray mGeometryArray;
	EngineArray mEngineArray;
	TriggerArray mTriggerArray;
	TransformOperation mTransformOperation;
	PhysicsType mPhysicsType;
	unsigned mUniqeGeometryIndex;

	LOG_CLASS_DECLARE();
};



}
