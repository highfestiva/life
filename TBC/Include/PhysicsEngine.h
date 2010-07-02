
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <vector>
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../Include/TBC.h"



namespace TBC
{



class ChunkyBoneGeometry;
class ChunkyPhysics;
class PhysicsManager;



class PhysicsEngine
{
public:
	enum EngineType
	{
		ENGINE_WALK = 1,
		ENGINE_CAMERA_FLAT_PUSH,
		ENGINE_HOVER,
		ENGINE_HINGE_ROLL,
		ENGINE_HINGE_GYRO,
		ENGINE_HINGE_BREAK,
		ENGINE_HINGE_TORQUE,
		ENGINE_HINGE2_TURN,
		ENGINE_ROTOR,
		ENGINE_TILTER,
		ENGINE_SLIDER_FORCE,
		ENGINE_GLUE,
	};
	enum EngineMode
	{
		MODE_NORMAL = 1,
		MODE_HALF_LOCK,
	};
	enum
	{
		MAX_CONTROLLER_COUNT = 8,
	};

	PhysicsEngine(EngineType pEngineType, float pStrength,
		float pMaxSpeed, float pMaxSpeed2, float pFriction, unsigned pControllerIndex);
	virtual ~PhysicsEngine();

	static PhysicsEngine* Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount);

	EngineType GetEngineType() const;

	void AddControlledGeometry(ChunkyBoneGeometry* pGeometry, float pScale, EngineMode pMode = MODE_NORMAL);
	bool SetValue(unsigned pAspect, float pValue, float pZAngle);
	void ForceSetValue(unsigned pAspect, float pValue);

	void OnTick(PhysicsManager* pPhysicsManager, const ChunkyPhysics* pStructure, float pFrameTime) const;
	float GetCurrentMaxSpeedSquare(const PhysicsManager* pPhysicsManager) const;

	unsigned GetControllerIndex() const;
	float GetValue() const;
	const float* GetValues() const;
	float GetIntensity() const;
	float GetMaxSpeed() const;
	float GetLerpThrottle(float pUp, float pDown) const;

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
		mutable float mLock;
	};

	Vector3DF GetRotorLiftForce(TBC::PhysicsManager* pPhysicsManager, ChunkyBoneGeometry* pGeometry, const EngineNode& pEngineNode) const;
	void ApplyTorque(TBC::PhysicsManager* pPhysicsManager, float pFrameTime, ChunkyBoneGeometry* pGeometry, const EngineNode& pEngineNode) const;

	typedef std::vector<EngineNode> EngineNodeArray;

	EngineType mEngineType;
	float mStrength;
	float mMaxSpeed;
	float mMaxSpeed2;
	float mFriction;
	unsigned mControllerIndex;
	EngineNodeArray mEngineNodeArray;
	float mValue[MAX_CONTROLLER_COUNT];
	mutable float mIntensity;
	mutable float mSmoothValue[MAX_CONTROLLER_COUNT];

	LOG_CLASS_DECLARE();
};



}
