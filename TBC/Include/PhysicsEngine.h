
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
	enum EngineAspects
	{
		ASPECT_PRIMARY		= 0,
		ASPECT_SECONDARY	= 1,
		ASPECT_TERTIARY		= 2,
		ASPECT_CAM		= 3,
		ASPECT_MAX_REMOTE_COUNT	= 4,

		ASPECT_LOCAL_SHADOW	= 4,
		ASPECT_LOCAL_SHADOW_ABS	= 5,
		ASPECT_LOCAL_PRIMARY	= 6,
		ASPECT_COUNT		= 8
	};

	typedef std::vector<ChunkyBoneGeometry*> GeometryList;

	PhysicsEngine(EngineType pEngineType, float pStrength,
		float pMaxSpeed, float pMaxSpeed2, float pFriction, unsigned pControllerIndex);
	virtual ~PhysicsEngine();

	static PhysicsEngine* Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount);

	EngineType GetEngineType() const;

	void AddControlledGeometry(ChunkyBoneGeometry* pGeometry, float pScale, EngineMode pMode = MODE_NORMAL);
	GeometryList GetControlledGeometryList() const;
	bool SetValue(unsigned pAspect, float pValue, float pZAngle);
	void ForceSetValue(unsigned pAspect, float pValue);

	void OnMicroTick(PhysicsManager* pPhysicsManager, const ChunkyPhysics* pStructure, float pFrameTime) const;
	Vector3DF GetCurrentMaxSpeed(const PhysicsManager* pPhysicsManager) const;

	static void UprightStabilize(PhysicsManager* pPhysicsManager, const ChunkyPhysics* pStructure,
		const ChunkyBoneGeometry* pGeometry, float pStrength, float pFriction);
	static void ForwardStabilize(PhysicsManager* pPhysicsManager, const ChunkyPhysics* pStructure,
		const ChunkyBoneGeometry* pGeometry, float pStrength, float pFriction);

	unsigned GetControllerIndex() const;
	float GetValue() const;
	const float* GetValues() const;
	float GetIntensity() const;
	float GetMaxSpeed() const;
	float GetLerpThrottle(float pUp, float pDown, bool pAbs) const;
	bool HasEngineMode(EngineMode pMode) const;

	unsigned GetChunkySize() const;
	void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

private:
	float GetPrimaryValue() const;

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
	float mValue[ASPECT_COUNT];
	mutable float mIntensity;
	mutable float mSmoothValue[ASPECT_COUNT];

	LOG_CLASS_DECLARE();
};



}
