
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
class PhysicsSpawner;
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
	enum GuideMode
	{
		GUIDE_NEVER = 0,
		GUIDE_EXTERNAL,		// Guiding happens (e.g. child steering).
		GUIDE_ALWAYS,
	};

	ChunkyPhysics(TransformOperation pTransformOperation = TRANSFORM_NONE, PhysicsType pPhysicsType = STATIC);
	ChunkyPhysics(const ChunkyPhysics& pOriginal);
	virtual ~ChunkyPhysics();

	void OnMicroTick(PhysicsManager* pPhysicsManager, float pFrameTime);

	PhysicsType GetPhysicsType() const;
	void SetPhysicsType(PhysicsType pPhysicsType);	// You may use this to change into a ragdoll (COLLISION_DETECT_ONLY -> DYNAMIC), or back.
	GuideMode GetGuideMode() const;	// Gets help with automatic motions, such as flipping over.
	void SetGuideMode(GuideMode pGuideMode);

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
	int GetEngineIndexFromControllerIndex(int pStartEngineIndex, int pEngineStep, unsigned pControllerIndex) const;
	int GetEngineIndex(const PhysicsEngine* pEngine) const;
	void AddEngine(PhysicsEngine* pEngine);	// Takes ownership of the given engine.
	bool SetEnginePower(unsigned pAspect, float pPower, float pAngle);
	void ClearEngines();

	int GetTriggerCount() const;
	const PhysicsTrigger* GetTrigger(int pTriggerIndex) const;
	void AddTrigger(PhysicsTrigger* pTrigger);	// Takes ownership of the given trigger.
	void ClearTriggers();

	int GetSpawnerCount() const;
	const PhysicsSpawner* GetSpawner(int pSpawnerIndex) const;
	void AddSpawner(PhysicsSpawner* pSpawner);	// Takes ownership of the given spawner.
	void ClearSpawners();

	// Overrides.
	void ClearAll(PhysicsManager* pPhysics);
	void SetBoneCount(int pBoneCount);
	bool FinalizeInit(PhysicsManager* pPhysics, unsigned pPhysicsFps, const TransformationF* pTransform,
		int pTrigListenerId, int pForceListenerId);

	unsigned GetNextGeometryIndex();

protected:
private:
	typedef std::vector<ChunkyBoneGeometry*> GeometryArray;
	typedef std::vector<PhysicsEngine*> EngineArray;
	typedef std::vector<PhysicsSpawner*> SpawnerArray;
	typedef std::vector<PhysicsTrigger*> TriggerArray;
	GeometryArray mGeometryArray;
	EngineArray mEngineArray;
	TriggerArray mTriggerArray;
	SpawnerArray mSpawnerArray;
	TransformOperation mTransformOperation;
	PhysicsType mPhysicsType;
	GuideMode mGuideMode;
	unsigned mUniqeGeometryIndex;

	LOG_CLASS_DECLARE();
};



}
