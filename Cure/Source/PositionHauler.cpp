
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/positionhauler.h"
#include "../../tbc/include/chunkybonegeometry.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../../tbc/include/physicsengine.h"
#include "../../tbc/include/physicsmanager.h"
#include "../include/positionaldata.h"
#include "../../lepra/include/lepraassert.h"



namespace cure {



#define GET_OBJECT_POSITIONAL_AT(obj, at, cls, name, type)		\
	cls* name = (cls*)((obj).GetAt(at));				\
	if (name && name->GetType() != type)				\
	{								\
		name = 0;						\
	}

#define GETSET_OBJECT_POSITIONAL_AT(obj, at, cls, name, type, scale)	\
	GET_OBJECT_POSITIONAL_AT(obj, at, cls, name, type)		\
	if (!name)							\
	{								\
		name = new cls;						\
		(obj).SetAt(at, name);					\
	}								\
	name->SetScale(scale);



bool PositionHauler::Get(ObjectPositionalData& position, const tbc::PhysicsManager* physics_manager, const tbc::ChunkyPhysics* structure, float total_mass) {
	if (!structure) {
		return false;
	}

	tbc::ChunkyBoneGeometry* root_geometry = structure->GetBoneGeometry(structure->GetRootBone());
	tbc::PhysicsManager::BodyID body = root_geometry->GetBodyId();
	physics_manager->GetBodyTransform(body, position.position_.transformation_);
	physics_manager->GetBodyVelocity(body, position.position_.velocity_);
	physics_manager->GetBodyAcceleration(body, total_mass, position.position_.acceleration_);
	physics_manager->GetBodyAngularVelocity(body, position.position_.angular_velocity_);
	physics_manager->GetBodyAngularAcceleration(body, total_mass, position.position_.angular_acceleration_);

	const int geometry_count = structure->GetBoneCount();
	size_t y = 0;
	for (int x = 0; x < geometry_count; ++x) {
		// TODO: add support for parent ID??? (JB 2009-07-07: Don't know anymore what this comment might mean.)
		// ??? Could it be when connected to something else, like a car connected to a crane?
		const tbc::ChunkyBoneGeometry* structure_geometry = structure->GetBoneGeometry(x);
		if (!structure_geometry) {
			log_.Error("Could not get positional update (for streaming), since *WHOLE* physical object not loaded!");
			return false;
		}
		body = structure_geometry->GetBodyId();
		tbc::PhysicsManager::JointID joint = structure_geometry->GetJointId();
		switch (structure_geometry->GetJointType()) {
			case tbc::ChunkyBoneGeometry::kJointSuspendHinge: {
				GETSET_OBJECT_POSITIONAL_AT(position, y, PositionalData2, data, PositionalData::kTypePosition2, 1);
				++y;
				tbc::PhysicsManager::Joint3Diff diff;
				if (!physics_manager->GetJoint3Diff(body, joint, diff)) {
					log_.Error("Could not get hinge-2!");
					return false;
				}
				data->transformation_[0] = diff.value_;
				data->transformation_[1] = diff.angle2_;
				data->velocity_[0] = diff.value_velocity_;
				data->velocity_[1] = diff.angle2_velocity_;
				data->acceleration_[0] = diff.value_acceleration_;
				data->acceleration_[1] = diff.angle2_acceleration_;
			} break;
			case tbc::ChunkyBoneGeometry::kJointHinge2: {
				GETSET_OBJECT_POSITIONAL_AT(position, y, PositionalData3, data, PositionalData::kTypePosition3, 1);
				++y;
				tbc::PhysicsManager::Joint3Diff diff;
				if (!physics_manager->GetJoint3Diff(body, joint, diff)) {
					log_.Error("Could not get hinge-2!");
					return false;
				}
				data->transformation_[0] = diff.value_;
				data->transformation_[1] = diff.angle1_;
				data->transformation_[2] = diff.angle2_;
				data->velocity_[0] = diff.value_velocity_;
				data->velocity_[1] = diff.angle1_velocity_;
				data->velocity_[2] = diff.angle2_velocity_;
				data->acceleration_[0] = diff.value_acceleration_;
				data->acceleration_[1] = diff.angle1_acceleration_;
				data->acceleration_[2] = diff.angle2_acceleration_;
			} break;
			case tbc::ChunkyBoneGeometry::kJointHinge:
			case tbc::ChunkyBoneGeometry::kJointSlider: {
				GETSET_OBJECT_POSITIONAL_AT(position, y, PositionalData1, data, PositionalData::kTypePosition1, 1);
				++y;
				tbc::PhysicsManager::Joint1Diff diff;
				if (!physics_manager->GetJoint1Diff(body, joint, diff)) {
					log_.Error("Could not get hinge!");
					return false;
				}
				data->transformation_ = diff.value_;
				data->velocity_ = diff.velocity_;
				data->acceleration_ = diff.acceleration_;
			} break;
			case tbc::ChunkyBoneGeometry::kJointBall: {
				GETSET_OBJECT_POSITIONAL_AT(position, y, PositionalData3, data, PositionalData::kTypePosition3, 0.00001f);
				++y;
				tbc::PhysicsManager::Joint3Diff diff;
				if (!physics_manager->GetJoint3Diff(body, joint, diff)) {
					log_.Error("Could not get ball!");
					return false;
				}
				data->transformation_[0] = diff.value_;
				data->transformation_[1] = diff.angle1_;
				data->transformation_[2] = diff.angle2_;
				data->velocity_[0] = diff.value_velocity_;
				data->velocity_[1] = diff.angle1_velocity_;
				data->velocity_[2] = diff.angle2_velocity_;
				data->acceleration_[0] = diff.value_acceleration_;
				data->acceleration_[1] = diff.angle1_acceleration_;
				data->acceleration_[2] = diff.angle2_acceleration_;
			} break;
			case tbc::ChunkyBoneGeometry::kJointUniversal: {
				GETSET_OBJECT_POSITIONAL_AT(position, y, PositionalData2, data, PositionalData::kTypePosition2, 1);
				++y;
				tbc::PhysicsManager::Joint2Diff diff;
				if (!physics_manager->GetJoint2Diff(body, joint, diff)) {
					log_.Error("Could not get universal!");
					return false;
				}
				data->transformation_[0] = diff.value_;
				data->transformation_[1] = diff.angle_;
				data->velocity_[0] = diff.value_velocity_;
				data->velocity_[1] = diff.angle_velocity_;
				data->acceleration_[0] = diff.value_acceleration_;
				data->acceleration_[1] = diff.angle_acceleration_;
			} break;
			case tbc::ChunkyBoneGeometry::kJointExclude: {
			} break;
			default: {
				deb_assert(false);
			} break;
		}
	}

	const int engine_count = structure->GetEngineCount();
	for (int z = 0; z != engine_count; ++z) {
		// TODO: add support for parent ID??????????? JB 2009-07-08: don't know what this is anymore.
		const tbc::PhysicsEngine* engine = structure->GetEngine(z);
		switch (engine->GetEngineType()) {
			case tbc::PhysicsEngine::kEngineWalk:
			case tbc::PhysicsEngine::kEnginePushRelative:
			case tbc::PhysicsEngine::kEnginePushAbsolute:
			case tbc::PhysicsEngine::kEnginePushTurnRelative:
			case tbc::PhysicsEngine::kEnginePushTurnAbsolute: {
				GETSET_OBJECT_POSITIONAL_AT(position, y, RealData3, data, PositionalData::kTypeReal3, 100);
				++y;
				::memcpy(data->value_, engine->GetValues(), sizeof(float)*tbc::PhysicsEngine::kAspectMaxRemoteCount);
			} break;
			case tbc::PhysicsEngine::kEngineHover:
			case tbc::PhysicsEngine::kEngineHingeRoll:
			case tbc::PhysicsEngine::kEngineHingeGyro:
			case tbc::PhysicsEngine::kEngineHingeBrake:
			case tbc::PhysicsEngine::kEngineHingeTorque:
			case tbc::PhysicsEngine::kEngineHinge2Turn:
			case tbc::PhysicsEngine::kEngineRotor:
			case tbc::PhysicsEngine::kEngineRotorTilt:
			case tbc::PhysicsEngine::kEngineJet:
			case tbc::PhysicsEngine::kEngineSliderForce:
			case tbc::PhysicsEngine::kEngineYawBrake:
			case tbc::PhysicsEngine::kEngineAirBrake: {
				GETSET_OBJECT_POSITIONAL_AT(position, y, RealData1, data, PositionalData::kTypeReal1, 100);
				++y;
				data->value_ = engine->GetValue();
				//deb_assert(data->value_ >= -1 && data->value_ <= 1);
			} break;
			case tbc::PhysicsEngine::kEngineGlue:
			case tbc::PhysicsEngine::kEngineBallBrake: {
				// Unsynchronized "engine".
			} break;
			default: {
				deb_assert(false);
			} break;
		}
	}

	position.Trunkate(y);

	return true;
}

void PositionHauler::Set(const ObjectPositionalData& position, tbc::PhysicsManager* physics_manager, tbc::ChunkyPhysics* structure, float total_mass, bool allow_move_root) {
	if (allow_move_root) {
		const tbc::ChunkyBoneGeometry* root_geometry = structure->GetBoneGeometry(structure->GetRootBone());
		const tbc::PhysicsManager::BodyID body = root_geometry->GetBodyId();
		if (body) {
			physics_manager->SetBodyTransform(body, position.position_.transformation_);
			if (structure->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic) {
				physics_manager->SetBodyVelocity(body, position.position_.velocity_);
				physics_manager->SetBodyAcceleration(body, total_mass, position.position_.acceleration_);
				physics_manager->SetBodyAngularVelocity(body, position.position_.angular_velocity_);
				physics_manager->SetBodyAngularAcceleration(body, total_mass, position.position_.angular_acceleration_);
			}
		}
	} else {
		log_.Info("Skipping setting of main body; we're owned by someone else.");
	}

	if (position.body_position_array_.size() <= 0) {
		return;
	}

	//log_.Info("Setting full position.");

	const int bone_count = structure->GetBoneCount();
	size_t y = 0;
	for (int x = 0; x < bone_count && y < position.body_position_array_.size(); ++x) {
		// TODO: add support for parent ID.
		const tbc::ChunkyBoneGeometry* structure_geometry = structure->GetBoneGeometry(x);
		tbc::PhysicsManager::BodyID body = structure_geometry->GetBodyId();
		tbc::PhysicsManager::JointID joint = structure_geometry->GetJointId();
		switch (structure_geometry->GetJointType()) {
			case tbc::ChunkyBoneGeometry::kJointSuspendHinge: {
				deb_assert(position.body_position_array_[y]->GetType() == PositionalData::kTypePosition2);
				GET_OBJECT_POSITIONAL_AT(position, y, const PositionalData2, data, PositionalData::kTypePosition2);
				++y;
				deb_assert(data);
				if (!data) {
					log_.Error("Could not fetch the right type of network positional!");
					return;
				}
				const tbc::PhysicsManager::Joint3Diff diff(data->transformation_[0], data->transformation_[1], 100000,
					data->velocity_[0], data->velocity_[1], 100000,
					data->acceleration_[0], data->acceleration_[1], 100000);
				if (!physics_manager->SetJoint3Diff(body, joint, diff)) {
					log_.Error("Could not set hinge-2!");
					return;
				}
			} break;
			case tbc::ChunkyBoneGeometry::kJointHinge2: {
				deb_assert(position.body_position_array_[y]->GetType() == PositionalData::kTypePosition3);
				GET_OBJECT_POSITIONAL_AT(position, y, const PositionalData3, data, PositionalData::kTypePosition3);
				++y;
				deb_assert(data);
				if (!data) {
					log_.Error("Could not fetch the right type of network positional!");
					return;
				}
				const tbc::PhysicsManager::Joint3Diff diff(data->transformation_[0], data->transformation_[1], data->transformation_[2],
					data->velocity_[0], data->velocity_[1], data->velocity_[2],
					data->acceleration_[0], data->acceleration_[1], data->acceleration_[2]);
				if (!physics_manager->SetJoint3Diff(body, joint, diff)) {
					log_.Error("Could not set hinge-2!");
					return;
				}
			} break;
			case tbc::ChunkyBoneGeometry::kJointHinge:
			case tbc::ChunkyBoneGeometry::kJointSlider: {
				deb_assert(position.body_position_array_[y]->GetType() == PositionalData::kTypePosition1);
				GET_OBJECT_POSITIONAL_AT(position, y, const PositionalData1, data, PositionalData::kTypePosition1);
				++y;
				deb_assert(data);
				if (!data) {
					log_.Error("Could not fetch the right type of network positional!");
					return;
				}
				const tbc::PhysicsManager::Joint1Diff diff(data->transformation_,
					data->velocity_, data->acceleration_);
				if (!physics_manager->SetJoint1Diff(body, joint, diff)) {
					log_.Error("Could not set hinge!");
					return;
				}
			} break;
			case tbc::ChunkyBoneGeometry::kJointBall: {
				deb_assert(position.body_position_array_[y]->GetType() == PositionalData::kTypePosition3);
				GET_OBJECT_POSITIONAL_AT(position, y, const PositionalData3, data, PositionalData::kTypePosition3);
				++y;
				deb_assert(data);
				if (!data) {
					log_.Error("Could not fetch the right type of network positional!");
					return;
				}
				const tbc::PhysicsManager::Joint3Diff diff(data->transformation_[0], data->transformation_[1], data->transformation_[2],
					data->velocity_[0], data->velocity_[1], data->velocity_[2],
					data->acceleration_[0], data->acceleration_[1], data->acceleration_[2]);
				if (!physics_manager->SetJoint3Diff(body, joint, diff)) {
					log_.Error("Could not set ball!");
					return;
				}
			} break;
			case tbc::ChunkyBoneGeometry::kJointUniversal: {
				deb_assert(position.body_position_array_[y]->GetType() == PositionalData::kTypePosition2);
				GET_OBJECT_POSITIONAL_AT(position, y, const PositionalData2, data, PositionalData::kTypePosition2);
				++y;
				deb_assert(data);
				if (!data) {
					log_.Error("Could not fetch the right type of network positional!");
					return;
				}
				const tbc::PhysicsManager::Joint2Diff diff(data->transformation_[0], data->transformation_[1],
					data->velocity_[0], data->velocity_[1],
					data->acceleration_[0], data->acceleration_[1]);
				if (!physics_manager->SetJoint2Diff(body, joint, diff)) {
					log_.Error("Could not set universal!");
					return;
				}
			} break;
			case tbc::ChunkyBoneGeometry::kJointExclude: {
			} break;
			default: {
				deb_assert(false);
			} break;
		}
	}

	const int engine_count = structure->GetEngineCount();
	for (int z = 0; z != engine_count; ++z) {
		// TODO: add support for parent ID??????????? JB 2009-07-08: don't know what this is anymore.
		const tbc::PhysicsEngine* engine = structure->GetEngine(z);
		switch (engine->GetEngineType()) {
			case tbc::PhysicsEngine::kEngineWalk:
			case tbc::PhysicsEngine::kEnginePushRelative:
			case tbc::PhysicsEngine::kEnginePushAbsolute:
			case tbc::PhysicsEngine::kEnginePushTurnRelative:
			case tbc::PhysicsEngine::kEnginePushTurnAbsolute: {
				deb_assert(position.body_position_array_.size() > y);
				deb_assert(position.body_position_array_[y]->GetType() == PositionalData::kTypeReal3);
				GET_OBJECT_POSITIONAL_AT(position, y, const RealData3, data, PositionalData::kTypeReal3);
				++y;
				deb_assert(data);
				if (!data) {
					log_.Error("Could not fetch the right type of network positional!");
					return;
				}
				structure->SetEnginePower(engine->GetControllerIndex()+0, data->value_[0]);
				structure->SetEnginePower(engine->GetControllerIndex()+1, data->value_[1]);
				structure->SetEnginePower(engine->GetControllerIndex()+3, data->value_[2]);
			} break;
			case tbc::PhysicsEngine::kEngineHover:
			case tbc::PhysicsEngine::kEngineHingeRoll:
			case tbc::PhysicsEngine::kEngineHingeGyro:
			case tbc::PhysicsEngine::kEngineHingeBrake:
			case tbc::PhysicsEngine::kEngineHingeTorque:
			case tbc::PhysicsEngine::kEngineHinge2Turn:
			case tbc::PhysicsEngine::kEngineRotor:
			case tbc::PhysicsEngine::kEngineRotorTilt:
			case tbc::PhysicsEngine::kEngineJet:
			case tbc::PhysicsEngine::kEngineSliderForce:
			case tbc::PhysicsEngine::kEngineYawBrake:
			case tbc::PhysicsEngine::kEngineAirBrake: {
				deb_assert(position.body_position_array_.size() > y);
				deb_assert(position.body_position_array_[y]->GetType() == PositionalData::kTypeReal1);
				GET_OBJECT_POSITIONAL_AT(position, y, const RealData1, data, PositionalData::kTypeReal1);
				++y;
				deb_assert(data);
				if (!data) {
					log_.Error("Could not fetch the right type of network positional!");
					return;
				}
				deb_assert(data->value_ >= -5 && data->value_ <= 5);
				structure->SetEnginePower(engine->GetControllerIndex(), data->value_);
			} break;
			case tbc::PhysicsEngine::kEngineGlue:
			case tbc::PhysicsEngine::kEngineBallBrake: {
				// Unsynchronized "engine".
			} break;
			default: {
				deb_assert(false);
			} break;
		}
	}
}



loginstance(kGameContext, PositionHauler);



}
