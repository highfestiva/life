
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <vector>
#include "../../lepra/include/log.h"
#include "../../lepra/include/vector3d.h"
#include "../include/tbc.h"



namespace tbc {



class ChunkyBoneGeometry;
class ChunkyPhysics;
class PhysicsManager;



class PhysicsEngine {
public:
	enum EngineType {
		kEngineWalk = 1,
		kEnginePushRelative,
		kEnginePushAbsolute,
		kEnginePushTurnRelative,
		kEnginePushTurnAbsolute,
		kEngineHover,
		kEngineHingeRoll,
		kEngineHingeGyro,
		kEngineHingeBrake,
		kEngineHingeTorque,
		kEngineHinge2Turn,
		kEngineRotor,
		kEngineRotorTilt,
		kEngineJet,
		kEngineSliderForce,
		kEngineGlue,
		kEngineBallBrake,
		kEngineYawBrake,
		kEngineAirBrake,
		kEngineUprightStabilize,
		kEngineForwardStabilize,
	};
	enum EngineMode {
		kModeNormal = 1,
		kModeHalfLock,
		kModeRelease,
	};
	enum EngineAspects {
		kAspectPrimary		= 0,
		kAspectSecondary	= 1,
		kAspectTertiary		= 2,
		kAspectMaxRemoteCount	= 3,
		kAspectLocalShadow	= 3,
		kAspectLocalShadowAbs	= 4,
		kAspectLocalPrimary	= 5,
		kAspectCount		= 10
	};

	typedef std::vector<ChunkyBoneGeometry*> GeometryList;

	PhysicsEngine(EngineType engine_type, float strength,
		float max_speed, float max_speed2, float friction, unsigned controller_index);
	virtual ~PhysicsEngine();
	void RelocatePointers(const ChunkyPhysics* target, const ChunkyPhysics* source, const PhysicsEngine& original);

	static PhysicsEngine* Load(ChunkyPhysics* structure, const void* data, unsigned byte_count);

	EngineType GetEngineType() const;

	void AddControlledGeometry(ChunkyBoneGeometry* geometry, float scale, EngineMode mode = kModeNormal);
	void RemoveControlledGeometry(ChunkyBoneGeometry* geometry);
	GeometryList GetControlledGeometryList() const;
	void SetStrength(float strength);
	bool SetValue(unsigned aspect, float value);
	void ForceSetValue(unsigned aspect, float value);

	void OnMicroTick(PhysicsManager* physics_manager, const ChunkyPhysics* structure, float frame_time) const;
	vec3 GetCurrentMaxSpeed(const PhysicsManager* physics_manager) const;

	static void UprightStabilize(PhysicsManager* physics_manager, const ChunkyPhysics* structure,
		const ChunkyBoneGeometry* geometry, float strength, float friction);
	static void ForwardStabilize(PhysicsManager* physics_manager, const ChunkyPhysics* structure,
		const ChunkyBoneGeometry* geometry, float strength, float friction);

	unsigned GetControllerIndex() const;
	float GetValue() const;
	const float* GetValues() const;
	float GetIntensity() const;
	float GetMaxSpeed() const;
	float GetLerpThrottle(float up, float down, bool abs) const;
	bool HasEngineMode(EngineMode mode) const;

	unsigned GetChunkySize() const;
	void SaveChunkyData(const ChunkyPhysics* structure, void* data) const;

private:
	float GetPrimaryValue() const;

	void LoadChunkyData(ChunkyPhysics* structure, const void* data);

	struct EngineNode {
		EngineNode(ChunkyBoneGeometry* geometry, float scale, EngineMode mode):
			geometry_(geometry),
			scale_(scale),
			mode_(mode) {
		}
		ChunkyBoneGeometry* geometry_;
		float scale_;
		EngineMode mode_;
		mutable float lock_;
	};

	vec3 GetRotorLiftForce(tbc::PhysicsManager* physics_manager, ChunkyBoneGeometry* geometry, const EngineNode& engine_node) const;
	void ApplyTorque(tbc::PhysicsManager* physics_manager, float frame_time, ChunkyBoneGeometry* geometry, const EngineNode& engine_node) const;

	typedef std::vector<EngineNode> EngineNodeArray;

	EngineType engine_type_;
	float strength_;
	float max_speed_;
	float max_speed2_;
	float friction_;
	unsigned controller_index_;
	EngineNodeArray engine_node_array_;
	float value_[kAspectCount];
	mutable float intensity_;
	mutable float smooth_value_[kAspectCount];

	logclass();
};



}
