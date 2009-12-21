
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



class ChunkyPhysics: public BoneHierarchy
{
	typedef BoneHierarchy Parent;
public:
	enum PhysicsType
	{
		STATIC = 1,		// Static world object.
		DYNAMIC,		// Dynamic object within the world.
		COLLISION_DETECT_ONLY,	// Only collision detection. (Used when ray-testing NPCs/avatars.) Change type to dynamic for ragdoll (when falling or dead).
	};

	ChunkyPhysics(TransformOperation pTransformOperation = TRANSFORM_NONE, PhysicsType pPhysicsType = STATIC);
	virtual ~ChunkyPhysics();

	void OnTick(PhysicsManager* pPhysicsManager, float pFrameTime);

	void SetPhysicsType(PhysicsType pPhysicsType);	// You may use this to change into a ragdoll (COLLISION_DETECT_ONLY -> DYNAMIC), or back.
	PhysicsType GetPhysicsType() const;

	ChunkyBoneGeometry* GetBoneGeometry(int pBoneIndex) const;
	ChunkyBoneGeometry* GetBoneGeometry(TBC::PhysicsManager::BodyID pBodyId) const;
	void AddBoneGeometry(ChunkyBoneGeometry* pGeometry);	// Takes ownership of the added geometry.
	void AddBoneGeometry(const Lepra::TransformationF& pTransformation, ChunkyBoneGeometry* pGeometry,
		const ChunkyBoneGeometry* pParent = 0);	// Takes ownership of the added geometry.
	int GetIndex(const ChunkyBoneGeometry* pGeometry) const;
	const Lepra::TransformationF& GetTransformation(const ChunkyBoneGeometry* pGeometry) const;
	void ClearBoneGeometries(PhysicsManager* pPhysics);

	int GetEngineCount() const;
	PhysicsEngine* GetEngine(int pEngineIndex) const;
	void AddEngine(PhysicsEngine* pEngine);	// Takes ownership of the given engine.
	void SetEnginePower(unsigned pAspect, float pPower, float pAngle);
	void ClearEngines();

	// Overrides.
	void ClearAll(PhysicsManager* pPhysics);
	void SetBoneCount(int pBoneCount);
	bool FinalizeInit(PhysicsManager* pPhysics, unsigned pPhysicsFps, Lepra::TransformationF* pTransform,
		PhysicsManager::TriggerListener* pTrigListener, PhysicsManager::ForceFeedbackListener* pForceListener);

	unsigned GetNextGeometryIndex();

protected:
private:
	typedef std::vector<ChunkyBoneGeometry*> GeometryArray;
	typedef std::vector<PhysicsEngine*> EngineArray;
	GeometryArray mGeometryArray;
	EngineArray mEngineArray;
	TransformOperation mTransformOperation;
	PhysicsType mPhysicsType;
	unsigned mUniqeGeometryIndex;

	LOG_CLASS_DECLARE();
};



}
