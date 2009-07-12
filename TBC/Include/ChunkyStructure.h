
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include "TBC.h"
#include "../Include/Bones.h"
#include "../Include/PhysicsEngine.h"



namespace TBC
{



class ChunkyBoneGeometry;
class StructureEngine;



class ChunkyStructure: public BoneHierarchy
{
public:
	enum PhysicsType
	{
		STATIC = 1,		// Static world object.
		DYNAMIC,		// Dynamic object within the world.
		COLLISION_DETECT_ONLY,	// Only collision detection. (Used when ray-testing NPCs/avatars.) Change type to dynamic for ragdoll (when falling or dead).
	};

	ChunkyStructure(PhysicsType pPhysicsType = STATIC);
	virtual ~ChunkyStructure();

	void OnTick(PhysicsEngine* pPhysicsManager, float pFrameTime);

	void SetPhysicsType(PhysicsType pPhysicsType);	// You may use this to change into a ragdoll (COLLISION_DETECT_ONLY -> DYNAMIC), or back.
	PhysicsType GetPhysicsType() const;

	ChunkyBoneGeometry* GetBoneGeometry(int pBoneIndex) const;
	ChunkyBoneGeometry* GetBoneGeometry(TBC::PhysicsEngine::BodyID pBodyId) const;
	void AddBoneGeometry(ChunkyBoneGeometry* pGeometry);	// Takes ownership of the given geometry.
	void AddBoneGeometry(const Lepra::TransformationF& pTransformation, ChunkyBoneGeometry* pGeometry);	// Takes ownership of the given geometry.
	int GetIndex(const ChunkyBoneGeometry* pGeometry) const;
	const Lepra::TransformationF& GetTransformation(const ChunkyBoneGeometry* pGeometry) const;
	void ClearBoneGeometries(PhysicsEngine* pPhysics);

	int GetEngineCount() const;
	StructureEngine* GetEngine(int pBoneIndex) const;
	void AddEngine(StructureEngine* pEngine);	// Takes ownership of the given engine.
	void SetEnginePower(unsigned pAspect, float pPower, float pAngle);
	void ClearEngines();

	// Overrides.
	void ClearAll(PhysicsEngine* pPhysics);
	void SetBoneCount(int pBoneCount);
	bool FinalizeInit(PhysicsEngine* pPhysics, unsigned pPhysicsFps, Lepra::TransformationF pTransform,
		PhysicsEngine::TriggerListener* pTrigListener, PhysicsEngine::ForceFeedbackListener* pForceListener);

	unsigned GetNextGeometryIndex();

protected:
private:
	typedef std::vector<ChunkyBoneGeometry*> GeometryArray;
	typedef std::vector<StructureEngine*> EngineArray;
	GeometryArray mGeometryArray;
	EngineArray mEngineArray;
	PhysicsType mPhysicsType;
	unsigned mUniqeGeometryIndex;

	LOG_CLASS_DECLARE();
};



}
