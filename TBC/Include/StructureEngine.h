
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <vector>
#include "../../Lepra/Include/Log.h"



namespace TBC
{



class ChunkyBoneGeometry;
class ChunkyStructure;
class PhysicsEngine;



class StructureEngine
{
public:
	enum EngineType
	{
		ENGINE_WALK,
		ENGINE_CAMERA_FLAT_PUSH,
		ENGINE_HINGE2_ROLL,
		ENGINE_ROLL_STRAIGHT,
		ENGINE_HINGE2_TURN,
		ENGINE_HINGE2_BREAK,
		ENGINE_HINGE,
		ENGINE_GLUE,
	};
	enum EngineMode
	{
		MODE_NORMAL,
		MODE_HALF_LOCK,
	};

	StructureEngine(EngineType pEngineType, float pStrength,
		float pMaxSpeed, float pMaxSpeed2, unsigned pControllerIndex);
	virtual ~StructureEngine();

	static StructureEngine* Load(ChunkyStructure* pStructure, const void* pData, unsigned pByteCount);

	EngineType GetEngineType() const;

	void AddControlledGeometry(ChunkyBoneGeometry* pGeometry, float pScale, EngineMode pMode = MODE_NORMAL);
	bool SetValue(unsigned pAspect, float pValue, float pZAngle);

	void OnTick(PhysicsEngine* pPhysicsManager, float pFrameTime);

	unsigned GetControllerIndex() const;
	float GetValue() const;
	const float* GetValues() const;

	unsigned GetChunkySize() const;
	void SaveChunkyData(const ChunkyStructure* pStructure, void* pData) const;

private:
	void LoadChunkyData(ChunkyStructure* pStructure, const void* pData);

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

	void ApplyTorque(TBC::PhysicsEngine* pPhysicsManager, float pFrameTime, ChunkyBoneGeometry* pGeometry, const EngineNode& pEngineNode);

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
