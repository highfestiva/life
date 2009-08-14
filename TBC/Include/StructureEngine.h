
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <vector>
#include "../../Lepra/Include/Log.h"



namespace TBC
{



class ChunkyBoneGeometry;
class ChunkyPhysics;
class PhysicsManager;



class StructureEngine
{
public:
	enum EngineType
	{
		ENGINE_WALK = 1,
		ENGINE_CAMERA_FLAT_PUSH,
		ENGINE_HINGE2_ROLL,
		ENGINE_HINGE2_TURN,
		ENGINE_HINGE2_BREAK,
		ENGINE_HINGE,
		ENGINE_GLUE,

		ENGINE_ROLL_STRAIGHT = 100,	// TODO: remove!
	};
	enum EngineMode
	{
		MODE_NORMAL = 1,
		MODE_HALF_LOCK,
	};

	StructureEngine(EngineType pEngineType, float pStrength,
		float pMaxSpeed, float pMaxSpeed2, unsigned pControllerIndex);
	virtual ~StructureEngine();

	static StructureEngine* Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount);

	EngineType GetEngineType() const;

	void AddControlledGeometry(ChunkyBoneGeometry* pGeometry, float pScale, EngineMode pMode = MODE_NORMAL);
	bool SetValue(unsigned pAspect, float pValue, float pZAngle);

	void OnTick(PhysicsManager* pPhysicsManager, float pFrameTime);

	unsigned GetControllerIndex() const;
	float GetValue() const;
	const float* GetValues() const;

	unsigned GetChunkySize() const;
	void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

private:
	void LoadChunkyData(ChunkyPhysics* pStructure, const void* pData);

	struct EngineNode
	{
		EngineNode(ChunkyBoneGeometry* pGeometry, float pScale, EngineMode pMode):
			mGeometry(pGeometry),
			mScale(pScale),
			mMode(pMode)
		{
		}
		ChunkyBoneGeometry* mGeometry;
		float mScale;
		EngineMode mMode;
	};

	void ApplyTorque(TBC::PhysicsManager* pPhysicsManager, float pFrameTime, ChunkyBoneGeometry* pGeometry, const EngineNode& pEngineNode);

	typedef std::vector<EngineNode> EngineNodeArray;

	EngineType mEngineType;
	float mStrength;
	float mMaxSpeed;
	float mMaxSpeed2;
	unsigned mControllerIndex;
	EngineNodeArray mEngineNodeArray;
	float mValue[4];

	LOG_CLASS_DECLARE();
};



}
