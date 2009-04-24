
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include <vector>
#include "../../Lepra/Include/Log.h"
#include "ContextObjectAttribute.h"
#include "PhysicsNode.h"



namespace Cure
{



class ContextObjectEngine: public ContextObjectAttribute
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

	ContextObjectEngine(ContextObject* pContextObject, EngineType pEngineType, float pStrength,
		float pMaxSpeed, float pMaxSpeed2, unsigned pControllerIndex);
	virtual ~ContextObjectEngine();

	Type GetType() const;
	EngineType GetEngineType() const;

	void AddControlledNode(unsigned pPhysicsNodeId, float pScale, EngineMode pMode = MODE_NORMAL);
	bool SetValue(unsigned pAspect, float pValue, float pZAngle);

	void OnTick(float pFrameTime);

	unsigned GetControllerIndex() const;
	float GetValue() const;
	const float* GetValues() const;
	bool IsAnalogue() const;

private:
	struct EngineNode
	{
		EngineNode(PhysicsNode::Id pId, float pScale, EngineMode pMode):
			mId(pId),
			mScale(pScale),
			mMode(pMode)
		{
		}
		PhysicsNode::Id mId;
		float mScale;
		EngineMode mMode;
	};

	void ApplyTorque(float pFrameTime, TBC::PhysicsEngine* pPhysicsManager, const PhysicsNode& pNode, const EngineNode& pEngineNode);

	typedef std::vector<EngineNode> EngineNodeArray;

	EngineType mEngineType;
	float mStrength;
	float mMaxSpeed;
	float mMaxSpeed2;
	unsigned mControllerIndex;
	EngineNodeArray mEngineNodeArray;
	float mValue[4];
	bool mIsAnalogue;

	LOG_CLASS_DECLARE();
};



}
