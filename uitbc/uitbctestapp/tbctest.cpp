
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
/*
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/diskfile.h"
#include "../../tbc/include/chunkybonegeometry.h"
#include "../../tbc/include/chunkyloader.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../../tbc/include/physicsmanagerfactory.h"
#include "../../tbc/include/physicsengine.h"



void ReportTestResult(const lepra::LogDecorator& log, const str& test_name, const str& context, bool result);



class TbcTest {
};
lepra::LogDecorator gTbcLog(lepra::LogType::GetLogger(lepra::LogType::kTest), typeid(TbcTest));



bool WriteStructure(const str& filename, const tbc::ChunkyPhysics& structure) {
	bool ok = true;
	lepra::DiskFile file;
	if (ok) {
		ok = file.Open(filename, lepra::DiskFile::kModeWrite);
	}
	if (ok) {
		tbc::ChunkyPhysicsLoader loader(&file, false);
		ok = loader.Save(&structure);
	}
	return (ok);
}

bool ReadStructure(const str& filename, tbc::ChunkyPhysics& structure) {
	bool ok = true;
	lepra::DiskFile file;
	if (ok) {
		ok = file.Open(filename, lepra::DiskFile::kModeRead);
	}
	if (ok) {
		tbc::ChunkyPhysicsLoader loader(&file, false);
		ok = loader.Load(&structure);
	}
	return (ok);
}

bool ExportStructure() {
	str context;
	bool ok = true;

	int physics_fps = kCureStandardFrameRate;
	tbc::PhysicsManager* physics = tbc::PhysicsManagerFactory::Create(tbc::PhysicsManagerFactory::kEngineOde);
	if (ok) {
		context = "box save";
		str filename("box_01.phys");

		lepra::vec3 dimensions(2.0f, 1.0f, 3.5f);

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformWorld2Local, tbc::ChunkyPhysics::kDynamic);
		structure.SetBoneCount(1);

		tbc::ChunkyBoneGeometry* geometry = new tbc::ChunkyBoneBox(
			tbc::ChunkyBoneGeometry::BodyData(1.0f, 0.2f, 1.0f), dimensions);
		structure.AddBoneGeometry(lepra::xform(), geometry);

		ok = structure.FinalizeInit(physics, physics_fps, 0, 0);
		if (ok) {
			ok = WriteStructure(filename, structure);
		}
		structure.ClearAll(physics);
		deb_assert(ok);
	}
	if (ok) {
		context = "box load";
		str filename("box_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformLocal2World);
		ok = ReadStructure(filename, structure);
		deb_assert(ok);
		if (ok) {
			lepra::xform transformation;
			ok = structure.FinalizeInit(physics, physics_fps, &transformation, 0);
			deb_assert(ok);
		}
		if (ok) {
			ok = (structure.GetBoneCount() == 1 &&
				structure.GetPhysicsType() == tbc::ChunkyPhysics::kDynamic &&
				structure.GetBoneGeometry(0) != 0 &&
				structure.GetBoneGeometry(0)->GetBodyId() != tbc::INVALID_BODY &&
				structure.GetEngineCount() == 0);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
	}
	if (ok) {
		context = "sphere save";
		str filename("sphere_01.phys");

		const float radius = 1.0f;

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformWorld2Local, tbc::ChunkyPhysics::kDynamic);
		structure.SetBoneCount(1);

		tbc::ChunkyBoneGeometry* geometry = new tbc::ChunkyBoneSphere(
			tbc::ChunkyBoneGeometry::BodyData(1.0f, 0.2f, 1.0f), radius);
		structure.AddBoneGeometry(lepra::xform(), geometry);

		tbc::PhysicsEngine* engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEnginePushRelative,
			55, 50, 0, 0.01f, 0);
		engine->AddControlledGeometry(geometry, 1);
		structure.AddEngine(engine);

		ok = structure.FinalizeInit(physics, physics_fps, 0, 0);
		if (ok) {
			ok = WriteStructure(filename, structure);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
		deb_assert(ok);
	}
	if (ok) {
		context = "sphere load";
		str filename("sphere_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformLocal2World);
		ok = ReadStructure(filename, structure);
		deb_assert(ok);
		if (ok) {
			lepra::xform transformation;
			ok = structure.FinalizeInit(physics, physics_fps, &transformation, 0);
			deb_assert(ok);
		}
		if (ok) {
			ok = (structure.GetBoneCount() == 1 &&
				structure.GetPhysicsType() == tbc::ChunkyPhysics::kDynamic &&
				structure.GetBoneGeometry(0) != 0 &&
				structure.GetBoneGeometry(0)->GetBodyId() != tbc::INVALID_BODY &&
				structure.GetEngineCount() == 1);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
	}

	const float lVolvoMass = 1800;
	// Wheels and suspension.
	const float lVolvoSpringConstant = lVolvoMass*50;
	const float lVolvoDampingConstant = lVolvoMass/5;

	if (ok) {
		context = "car save";
		str filename("car_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformWorld2Local, tbc::ChunkyPhysics::kDynamic);
		structure.SetBoneCount(6);

		const float lWheelRadius = 0.3f;
		const float lWheelXOffset = -0.15f;
		const float lWheelZOffset = 0.3f;
		const float lBackWheelYOffset = -0.8f;
		const float lWheelYDistance = 3.6f;
		const lepra::vec3 lBodyDimensions(1.9f, 4.9f, 0.6f);
		const lepra::vec3 lTopDimensions(1.6f, 2.9f, 0.6f);
		lepra::xform transformation;

		// Body.
		tbc::ChunkyBoneGeometry::BodyData body_data(lVolvoMass, 0.1f, 1.0f, 0,
			tbc::ChunkyBoneGeometry::kJointExclude, tbc::ChunkyBoneGeometry::kConnectee3Dof);
		tbc::ChunkyBoneGeometry* lBodyGeometry = new tbc::ChunkyBoneBox(body_data, lBodyDimensions);
		structure.AddBoneGeometry(transformation, lBodyGeometry);
		// Top of body.
		lepra::xform lTopTransform(transformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		lTopTransform.MoveForward(0.9f);
		body_data.mass_ = lVolvoMass/5;
		body_data.friction_ = 0.5f;
		body_data.parent_ = lBodyGeometry;
		tbc::ChunkyBoneGeometry* geometry = new tbc::ChunkyBoneBox(body_data, lTopDimensions);
		structure.AddBoneGeometry(lTopTransform, geometry, body_data.parent_);

		// Wheels and suspension.
		transformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		transformation.GetOrientation().RotateAroundOwnZ(lepra::PIF/2);
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointSuspendHinge;
		body_data.connector_type_ = tbc::ChunkyBoneGeometry::kConnectNone;
		body_data.mass_ = lVolvoMass/50;
		body_data.friction_ = 1.0f;
		body_data.bounce_ = 0.5f;
		body_data.parameter_[0] = lVolvoSpringConstant;
		body_data.parameter_[1] = lVolvoDampingConstant;
		body_data.parameter_[2] = 0;
		body_data.parameter_[3] = lepra::PIF * 0.5f;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetOrientation().RotateAroundOwnZ(-lepra::PIF);
		body_data.parameter_[2] = lepra::PIF;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetPosition().y -= lWheelYDistance;
		transformation.GetOrientation().RotateAroundOwnZ(lepra::PIF);
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointHinge2;
		body_data.parameter_[2] = 0;
		body_data.parameter_[4] = -0.5f;
		body_data.parameter_[5] = 0.5f;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetOrientation().RotateAroundOwnZ(-lepra::PIF);
		body_data.parameter_[2] = lepra::PIF;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		// Front wheel drive engine.
		tbc::PhysicsEngine* engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHingeRoll, 1*lVolvoMass, 300, 20, 0.01f, 0);
		engine->AddControlledGeometry(structure.GetBoneGeometry(4), -1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(5), +1);
		structure.AddEngine(engine);
		// Turning front wheels.
		engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHinge2Turn, 1*lVolvoMass, 1.0f, 0, 0.01f, 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(4), +1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(5), +1);
		structure.AddEngine(engine);
		// Normal breaks (all nodes, scaled >= 0) and handbrake (rear wheels, nodes with high scaling values).
		const float lBreakInputThreashold = 0.02f;
		tbc::PhysicsEngine* lBreak = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHingeBrake, 8*lVolvoMass, lBreakInputThreashold, 0, 0.01f, 2);
		lBreak->AddControlledGeometry(structure.GetBoneGeometry(2), 100);
		lBreak->AddControlledGeometry(structure.GetBoneGeometry(3), 100);
		lBreak->AddControlledGeometry(structure.GetBoneGeometry(4), 0);
		lBreak->AddControlledGeometry(structure.GetBoneGeometry(5), 0);
		structure.AddEngine(lBreak);

		ok = structure.FinalizeInit(physics, physics_fps, 0, 0);
		if (ok) {
			ok = WriteStructure(filename, structure);
			deb_assert(ok);
		}
		if (ok) {
			float low_stop = -1;
			float high_stop = -1;
			float bounce = -1;
			physics->GetJointParams(structure.GetBoneGeometry(4)->GetJointId(), low_stop, high_stop, bounce);
			if (ok) {
				ok = (low_stop == -0.5f && high_stop == 0.5f && bounce == 0.0f);
			}
			deb_assert(ok);
		}
		structure.ClearAll(physics);
		deb_assert(ok);
	}
	if (ok) {
		context = "car load";
		str filename("car_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformLocal2World);
		ok = ReadStructure(filename, structure);
		deb_assert(ok);
		if (ok) {
			lepra::xform transform;
			transform.SetPosition(lepra::vec3(100, 100, 100));
			ok = structure.FinalizeInit(physics, physics_fps, &transform, 0);
			deb_assert(ok);
		}
		if (ok) {
			ok = (structure.GetBoneCount() == 6 &&
				structure.GetPhysicsType() == tbc::ChunkyPhysics::kDynamic &&
				structure.GetBoneGeometry(0) != 0 &&
				structure.GetBoneGeometry(4)->GetBodyId() != tbc::INVALID_BODY &&
				structure.GetBoneGeometry(4)->GetJointId() != tbc::INVALID_JOINT &&
				structure.GetEngineCount() == 4);
			deb_assert(ok);
		}
		if (ok) {
			lepra::vec3 position = physics->GetBodyPosition(structure.GetBoneGeometry(4)->GetBodyId());
			ok = (lepra::Math::IsInRange(position.x, 99.1f, 99.3f) &&
				lepra::Math::IsInRange(position.y, 98.0f, 98.1f) &&
				lepra::Math::IsInRange(position.z, 99.6f, 99.8f));
			deb_assert(ok);
		}
		if (ok) {
			float low_stop = -1;
			float high_stop = -1;
			float bounce = -1;
			physics->GetJointParams(structure.GetBoneGeometry(4)->GetJointId(), low_stop, high_stop, bounce);
			if (ok) {
				ok = (low_stop == -0.5f && high_stop == 0.5f && bounce == 0.0f);
			}
			deb_assert(ok);
		}
		if (ok) {
			float lErp = -1;
			float lCfm = -1;
			ok = physics->GetSuspension(structure.GetBoneGeometry(4)->GetJointId(), lErp, lCfm);
			if (ok) {
				const float frame_time = 1/(float)physics_fps;
				const float lCorrectErp = frame_time * lVolvoSpringConstant / (frame_time * lVolvoSpringConstant + lVolvoDampingConstant);
				const float lCorrectCfm = 1 / (frame_time * lVolvoSpringConstant + lVolvoDampingConstant);
				ok = (lErp == lCorrectErp && lCfm == lCorrectCfm);
			}
			deb_assert(ok);
		}
		if (ok) {
			lepra::xform transform;
			physics->GetBodyTransform(structure.GetBoneGeometry(4)->GetBodyId(), transform);
			transform.GetPosition().z += 1.0f;
			physics->SetBodyTransform(structure.GetBoneGeometry(4)->GetBodyId(), transform);
			tbc::PhysicsManager::Joint3Diff diff;
			ok = physics->GetJoint3Diff(structure.GetBoneGeometry(4)->GetBodyId(),
				structure.GetBoneGeometry(4)->GetJointId(), diff);
			deb_assert(ok);
			if (ok) {
				ok = (lepra::Math::IsEpsEqual(diff.value_, -1.0f));
			}
			deb_assert(ok);
		}
		structure.ClearAll(physics);
	}
	if (ok) {
		context = "monster save";
		str filename("monster_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformWorld2Local, tbc::ChunkyPhysics::kDynamic);
		structure.SetBoneCount(6);

		const float lCarWeight = 3000;
		const float lWheelRadius = 1.0f;
		const float lWheelXOffset = 0.5f;
		const float lWheelZOffset = 0.8f;
		const float lBackWheelYOffset = 0.0f;
		const float lWheelYDistance = 5.0f;
		const lepra::vec3 lBodyDimensions(3.0f, 5.0f, 0.7f);
		const lepra::vec3 lTopDimensions(3.0f, 3.0f, 0.7f);
		lepra::xform transformation;

		// Body.
		tbc::ChunkyBoneGeometry::BodyData body_data(lCarWeight, 0.1f, 0.5f);
		tbc::ChunkyBoneGeometry* lBodyGeometry = new tbc::ChunkyBoneBox(body_data, lBodyDimensions);
		structure.AddBoneGeometry(transformation, lBodyGeometry);
		// Top of body.
		lepra::xform lTopTransform(transformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		body_data.mass_ = lCarWeight/10;
		body_data.friction_ = 0.8f;
		body_data.parent_ = lBodyGeometry;
		tbc::ChunkyBoneGeometry* geometry = new tbc::ChunkyBoneBox(body_data, lTopDimensions);
		structure.AddBoneGeometry(lTopTransform, geometry, body_data.parent_);

		// Wheels and suspension.
		const float spring_constant = lCarWeight*4;
		const float lDamperConstant = lCarWeight/50;

		transformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		transformation.GetOrientation().RotateAroundOwnZ(lepra::PIF/2);
		body_data.mass_ = lCarWeight/50;
		body_data.friction_ = 1.0f;
		body_data.bounce_ = 0.5f;
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointSuspendHinge;
		body_data.connector_type_ = tbc::ChunkyBoneGeometry::kConnectNone;
		body_data.parameter_[0] = spring_constant;
		body_data.parameter_[1] = lDamperConstant;
		body_data.parameter_[2] = 0;
		body_data.parameter_[3] = lepra::PIF * 0.5f;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetOrientation().RotateAroundOwnZ(-lepra::PIF);
		body_data.parameter_[2] = lepra::PIF;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetPosition().y -= lWheelYDistance;
		transformation.GetOrientation().RotateAroundOwnZ(lepra::PIF);
		body_data.parameter_[2] = 0;
		body_data.parameter_[4] = -0.5f;
		body_data.parameter_[5] = 0.5f;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetOrientation().RotateAroundOwnZ(-lepra::PIF);
		body_data.parameter_[2] = lepra::PIF;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		// Rear wheel drive engine.
		tbc::PhysicsEngine* engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHingeRoll, 5*lCarWeight, 300, 20, 0.01f, 0);
		engine->AddControlledGeometry(structure.GetBoneGeometry(2), -1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(3), 1);
		structure.AddEngine(engine);
		// Turning front wheels.
		engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHinge2Turn, 1*lCarWeight, 0.5f, 0, 0.01f, 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(4), 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(5), 1);
		structure.AddEngine(engine);
		// Normal breaks (all nodes, scaled >= 0) and handbrake (rear wheels, nodes with high scaling values).
		const float lBreakInputThreashold = 0.02f;
		tbc::PhysicsEngine* lBreak = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHingeBrake, 8*lCarWeight, lBreakInputThreashold, 0, 0.01f, 2);
		lBreak->AddControlledGeometry(structure.GetBoneGeometry(2), 100);
		lBreak->AddControlledGeometry(structure.GetBoneGeometry(3), 100);
		lBreak->AddControlledGeometry(structure.GetBoneGeometry(4), 0);
		lBreak->AddControlledGeometry(structure.GetBoneGeometry(5), 0);
		structure.AddEngine(lBreak);

		ok = structure.FinalizeInit(physics, physics_fps, 0, 0);
		if (ok) {
			ok = WriteStructure(filename, structure);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
		deb_assert(ok);
	}
	if (ok) {
		context = "monster load";
		str filename("monster_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformLocal2World);
		ok = ReadStructure(filename, structure);
		deb_assert(ok);
		if (ok) {
			lepra::xform transformation;
			ok = structure.FinalizeInit(physics, physics_fps, &transformation, 0);
			deb_assert(ok);
		}
		if (ok) {
			ok = (structure.GetBoneCount() == 6 &&
				structure.GetPhysicsType() == tbc::ChunkyPhysics::kDynamic &&
				structure.GetBoneGeometry(0) != 0 &&
				structure.GetBoneGeometry(0)->GetBodyId() != tbc::INVALID_BODY &&
				structure.GetEngineCount() == 4);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
	}
	if (ok) {
		context = "excavator save";
		str filename("excavator_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformWorld2Local, tbc::ChunkyPhysics::kDynamic);
		structure.SetBoneCount(13);

		const float lCarWeight = 18000;
		const float lWheelRadius = 0.5f;
		const float lWheelXOffset = 0.2f;
		const float lWheelZOffset = 0.6f;
		const float lBackWheelYOffset = 0.5f;
		const float lWheelYDistance = 2.0f;
		const lepra::vec3 lBodyDimensions(2.9f, 3.0f, 1.5f);
		const lepra::vec3 lTopDimensions(1.0f, 1.5f, 0.6f);
		lepra::xform transformation;

		// Body.
		const lepra::xform lBodyTransformation(transformation);
		tbc::ChunkyBoneGeometry::BodyData body_data(lCarWeight, 0.5f, 0.5f);
		tbc::ChunkyBoneGeometry* lBodyGeometry = new tbc::ChunkyBoneBox(body_data, lBodyDimensions);
		structure.AddBoneGeometry(transformation, lBodyGeometry);
		// Top of body.
		lepra::xform lTopTransform(transformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		lTopTransform.MoveRight(1.0f);
		lTopTransform.MoveBackward(0.75f);
		body_data.mass_ = lCarWeight/20;
		body_data.bounce_ = 1.0f;
		body_data.parent_ = lBodyGeometry;
		tbc::ChunkyBoneGeometry* geometry = new tbc::ChunkyBoneBox(body_data, lTopDimensions);
		structure.AddBoneGeometry(lTopTransform, geometry, body_data.parent_);

		// Wheels and suspension.
		const float spring_constant = lCarWeight*100;
		const float lDamperConstant = lCarWeight/5;

		transformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		transformation.GetOrientation().RotateAroundOwnZ(lepra::PIF/2);
		body_data.mass_ = lCarWeight/50;
		body_data.friction_ = 4.0f;
		body_data.bounce_ = 0.2f;
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointSuspendHinge;
		body_data.connector_type_ = tbc::ChunkyBoneGeometry::kConnectNone;
		body_data.parameter_[0] = spring_constant;
		body_data.parameter_[1] = lDamperConstant;
		body_data.parameter_[2] = 0;
		body_data.parameter_[3] = lepra::PIF * 0.5f;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetOrientation().RotateAroundOwnZ(-lepra::PIF);
		body_data.parameter_[2] = lepra::PIF;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetPosition().y -= lWheelYDistance;
		transformation.GetOrientation().RotateAroundOwnZ(lepra::PIF);
		body_data.parameter_[2] = 0;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetOrientation().RotateAroundOwnZ(-lepra::PIF);
		body_data.parameter_[2] = lepra::PIF;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetPosition().y -= lWheelYDistance;
		transformation.GetOrientation().RotateAroundOwnZ(lepra::PIF);
		body_data.parameter_[2] = 0;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		transformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		transformation.GetOrientation().RotateAroundOwnZ(-lepra::PIF);
		body_data.parameter_[2] = lepra::PIF;
		geometry = new tbc::ChunkyBoneSphere(body_data, lWheelRadius);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		// Boom.
		const lepra::vec3 lBoom1Dimensions(0.6f, 0.6f, 2.5f);
		const lepra::vec3 lBoom2Dimensions(0.6f, 0.6f, 3.2f);
		transformation = lBodyTransformation;
		transformation.MoveUp(lBodyDimensions.z/2);
		transformation.MoveLeft(0.5f);
		transformation.MoveBackward(0.75f);
		lepra::vec3 lBoomAnchor(transformation.GetPosition());
		transformation.MoveUp(lBoom1Dimensions.z/2);
		body_data.mass_ = lCarWeight*3/(12*5);
		body_data.friction_ = 1.0f;
		body_data.bounce_ = 0.2f;
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointHinge;
		body_data.parameter_[2] = 0;
		body_data.parameter_[3] = lepra::PIF * 0.5f;
		body_data.parameter_[4] = -1.2f;
		body_data.parameter_[5] = 0.5f;
		lBoomAnchor -= transformation.GetPosition();
		body_data.parameter_[6] = lBoomAnchor.x;
		body_data.parameter_[7] = lBoomAnchor.y;
		body_data.parameter_[8] = lBoomAnchor.z;
		geometry = new tbc::ChunkyBoneBox(body_data, lBoom1Dimensions);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);
		// Boom, part 2.
		transformation.MoveUp(lBoom1Dimensions.z/2);
		const float lBoom2Angle = lepra::PIF/4;
		transformation.MoveBackward(::sin(lBoom2Angle)*lBoom2Dimensions.z/2);
		transformation.MoveUp(::cos(lBoom2Angle)*lBoom2Dimensions.z/2);
		lepra::xform lBoom2Transform(transformation);
		lBoom2Transform.RotatePitch(lBoom2Angle);
		body_data.mass_ = lCarWeight*2/(12*5);
		body_data.parent_ = geometry;
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointExclude;
		geometry = new tbc::ChunkyBoneBox(body_data, lBoom2Dimensions);
		structure.AddBoneGeometry(lBoom2Transform, geometry, body_data.parent_);

		// Arm.
		transformation.MoveBackward(::sin(lBoom2Angle)*lBoom2Dimensions.z/2);
		transformation.MoveUp(::cos(lBoom2Angle)*lBoom2Dimensions.z/2);
		lepra::vec3 lArmAnchor(transformation.GetPosition());
		const lepra::vec3 lArmDimensions(0.4f, 3.0f, 0.4f);
		transformation.MoveBackward(lArmDimensions.y/2);
		body_data.mass_ = lCarWeight/25;
		body_data.parent_ = geometry;
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointHinge;
		body_data.parameter_[4] = -1.0f;
		body_data.parameter_[5] = 0.5f;
		lArmAnchor -= transformation.GetPosition();
		body_data.parameter_[6] = lArmAnchor.x;
		body_data.parameter_[7] = lArmAnchor.y;
		body_data.parameter_[8] = lArmAnchor.z;
		geometry = new tbc::ChunkyBoneBox(body_data, lArmDimensions);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		// Bucket.
		transformation.MoveBackward(lArmDimensions.y/2);
		lepra::vec3 lBucketAnchor(transformation.GetPosition());
		const lepra::vec3 lBucketBackDimensions(1.5f, 0.8f, 0.1f);
		const float lBucketBackAngle = lepra::PIF/4;
		transformation.MoveBackward(::cos(lBucketBackAngle)*lBucketBackDimensions.y/2);
		transformation.MoveUp(::sin(lBucketBackAngle)*lBucketBackDimensions.y/2);
		lepra::xform lBucketBackTransform(transformation);
		lBucketBackTransform.RotatePitch(-lBucketBackAngle);
		body_data.mass_ = lCarWeight/100;
		body_data.friction_ = 10.0f;
		body_data.parent_ = geometry;
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointHinge;
		body_data.parameter_[4] = -1.0f;
		body_data.parameter_[5] = 0.5f;
		body_data.parameter_[6] = 0;
		body_data.parameter_[7] = lBucketBackDimensions.y/2;
		body_data.parameter_[8] = 0;
		geometry = new tbc::ChunkyBoneBox(body_data, lBucketBackDimensions);
		structure.AddBoneGeometry(lBucketBackTransform, geometry, body_data.parent_);
		// Bucket, floor part.
		transformation.MoveBackward(::cos(lBucketBackAngle)*lBucketBackDimensions.y/2);
		transformation.MoveUp(::sin(lBucketBackAngle)*lBucketBackDimensions.y/2);
		const lepra::vec3 lBucketFloorDimensions(1.5f, 1.0f, 0.1f);
		const float lBucketFloorAngle = lepra::PIF/4;
		transformation.MoveBackward(::cos(lBucketFloorAngle)*lBucketFloorDimensions.y/2);
		transformation.MoveDown(::sin(lBucketFloorAngle)*lBucketFloorDimensions.y/2);
		lepra::xform lBucketFloorTransform(transformation);
		lBucketFloorTransform.RotatePitch(lBucketFloorAngle);
		body_data.parent_ = geometry;
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointExclude;
		geometry = new tbc::ChunkyBoneBox(body_data, lBucketBackDimensions);
		structure.AddBoneGeometry(lBucketFloorTransform, geometry, body_data.parent_);

		// All wheel drive engine.
		tbc::PhysicsEngine* engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHingeRoll, lCarWeight, 25, 25, 0.01f, 0);
		engine->AddControlledGeometry(structure.GetBoneGeometry(2), -1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(3), 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(4), -1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(5), 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(6), -1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(7), 1);
		structure.AddEngine(engine);
		// Turning with tracks are controlled by rolling of the wheels.
		engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHingeRoll, lCarWeight, 20, 20, 0.01f, 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(2), 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(3), 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(4), 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(5), 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(6), 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(7), 1);
		structure.AddEngine(engine);
		// Normal breaks (all nodes, scaled >= 0) and handbrake (nodes with high scaling values).
		const float lBreakInputThreashold = 0.02f;
		tbc::PhysicsEngine* lBreak = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHingeBrake, lCarWeight, lBreakInputThreashold, 0, 0.01f, 2);
		engine->AddControlledGeometry(structure.GetBoneGeometry(2), 100);
		engine->AddControlledGeometry(structure.GetBoneGeometry(3), 100);
		engine->AddControlledGeometry(structure.GetBoneGeometry(4), 100);
		engine->AddControlledGeometry(structure.GetBoneGeometry(5), 100);
		engine->AddControlledGeometry(structure.GetBoneGeometry(6), 100);
		engine->AddControlledGeometry(structure.GetBoneGeometry(7), 100);
		structure.AddEngine(lBreak);
		// The boom, arm and bucket are hinge-controlled.
		engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHingeTorque, 20*lCarWeight, 2.0f, 1.0f, 0.01f, 3);
		engine->AddControlledGeometry(structure.GetBoneGeometry(8), 5);
		engine->AddControlledGeometry(structure.GetBoneGeometry(10), 2, tbc::PhysicsEngine::kModeHalfLock);	// kModeHalfLock = has neutral/freeze position outside mid value.
		engine->AddControlledGeometry(structure.GetBoneGeometry(11), 1, tbc::PhysicsEngine::kModeHalfLock);	// kModeHalfLock = has neutral/freeze position outside mid value.
		structure.AddEngine(engine);

		ok = structure.FinalizeInit(physics, physics_fps, 0, 0);
		if (ok) {
			const lepra::vec3& lBucketPosition = structure.GetBoneTransformation(11).GetPosition();
			ok = (lepra::Math::IsInRange(lBucketPosition.x, -0.01f, 0.01f));
			deb_assert(ok);
		}
		if (ok) {
			ok = WriteStructure(filename, structure);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
		deb_assert(ok);
	}
	if (ok) {
		context = "excavator load";
		str filename("excavator_01.phys");

		gTbcLog.Debugf("--> Loading excavator. <--");
		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformLocal2World);
		ok = ReadStructure(filename, structure);
		deb_assert(ok);
		if (ok) {
			const lepra::vec3& lBucketPosition = structure.GetOriginalBoneTransformation(11).GetPosition();
			ok = (lepra::Math::IsInRange(lBucketPosition.x, -0.01f, 0.01f));
			deb_assert(ok);
		}
		if (ok) {
			lepra::xform transformation;
			transformation.SetPosition(lepra::vec3(4, 5, 6));
			ok = structure.FinalizeInit(physics, physics_fps, &transformation, 0);
			deb_assert(ok);
		}
		if (ok) {
			ok = (structure.GetBoneCount() == 13 &&
				structure.GetPhysicsType() == tbc::ChunkyPhysics::kDynamic &&
				structure.GetBoneGeometry(0) != 0 &&
				structure.GetBoneGeometry(0)->GetBodyId() != tbc::INVALID_BODY &&
				structure.GetBoneGeometry(8)->GetParent() == structure.GetBoneGeometry(0) &&
				structure.GetEngineCount() == 5);
			deb_assert(ok);
		}
		if (ok) {
			lepra::vec3 lBoomAnchor;
			lepra::vec3 lArmAnchor;
			lepra::vec3 lBucketAnchor;
			ok = (physics->GetAnchorPos(structure.GetBoneGeometry(8)->GetJointId(), lBoomAnchor) &&
				physics->GetAnchorPos(structure.GetBoneGeometry(10)->GetJointId(), lArmAnchor) &&
				physics->GetAnchorPos(structure.GetBoneGeometry(11)->GetJointId(), lBucketAnchor));
			deb_assert(ok);
			if (ok) {
				ok = (lepra::Math::IsInRange(lBoomAnchor.x, 3.45f, 3.55f) &&
					lepra::Math::IsInRange(lBoomAnchor.y, 4.2f, 4.3f) &&
					lepra::Math::IsInRange(lBoomAnchor.z, 6.7f, 6.8f) &&
					lepra::Math::IsInRange(lArmAnchor.x, 3.45f, 3.55f) &&
					lepra::Math::IsInRange(lArmAnchor.y, 1.98f, 1.99f) &&
					lepra::Math::IsInRange(lArmAnchor.z, 11.51f, 11.52f) &&
					lepra::Math::IsInRange(lBucketAnchor.x, 3.45f, 3.55f) &&
					lepra::Math::IsInRange(lBucketAnchor.y, 1.98f-3, 1.99f-3) &&
					lepra::Math::IsInRange(lBucketAnchor.z, 11.51f, 11.52f));
				deb_assert(ok);
			}
		}
		structure.ClearAll(physics);
	}
	if (ok) {
		context = "crane save";
		str filename("crane_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformWorld2Local, tbc::ChunkyPhysics::kDynamic);
		structure.SetBoneCount(7);

		const float lTowerWeight = 50000;
		const float lTowerHeight = 25.0f;
		const float lThickness = 2.5f;
		const lepra::vec3 lTowerDimensions(lThickness, lThickness, lTowerHeight);
		lepra::xform transformation;
		transformation.SetPosition(lepra::vec3(0, 0, lTowerHeight/2));

		// Body.
		tbc::ChunkyBoneGeometry::BodyData body_data(lTowerWeight, 1.0f, 0.5f);
		tbc::ChunkyBoneGeometry* geometry = new tbc::ChunkyBoneBox(body_data, lTowerDimensions);
		structure.AddBoneGeometry(transformation, geometry);

		// Jib.
		const float lJibWeight = lTowerWeight/3.5f;
		const float lJibPlacementHeight = 20.0f;
		const float lJibLength = 15.0f;
		const float lCounterJibLength = 5.0f;
		const float lTotalJibLength = lJibLength+lCounterJibLength;

		lepra::xform lJibTransform(transformation);
		lJibTransform.GetPosition().Add(0, 0, -lTowerHeight/2 + lJibPlacementHeight);
		lepra::vec3 lJibAnchor(lJibTransform.GetPosition());
		lJibTransform.GetPosition().Add(0, lTotalJibLength/2-lCounterJibLength, 0);
		const lepra::vec3 lJibDimensions(lThickness, lTotalJibLength, lThickness);
		body_data.mass_ = lJibWeight;
		body_data.parent_ = geometry;
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointHinge;
		body_data.parameter_[4] = -1e10f;
		body_data.parameter_[5] = 1e10f;
		lJibAnchor -= lJibTransform.GetPosition();
		body_data.parameter_[6] = lJibAnchor.x;
		body_data.parameter_[7] = lJibAnchor.y;
		body_data.parameter_[8] = lJibAnchor.z;
		geometry = new tbc::ChunkyBoneBox(body_data, lJibDimensions);
		structure.AddBoneGeometry(lJibTransform, geometry, body_data.parent_);

		// Wire.
		const float lWireWeight = 50;
		const int lWireSegmentCount = 4;
		const float lWireSegmentWeight = lWireWeight/lWireSegmentCount;
		const float lWireLength = (lJibPlacementHeight-1)/lWireSegmentCount;
		const float lWireThickness = 0.05f;
		lepra::xform lWireTransform(lJibTransform);
		lWireTransform.GetPosition().Add(0, lTotalJibLength/4, 0);
		for (int x = 0; x < lWireSegmentCount; ++x) {
			lepra::vec3 lWireAnchor(lWireTransform.GetPosition());
			lWireTransform.GetPosition().Add(0, 0, -lWireLength/2);

			body_data.mass_ = lWireSegmentWeight;
			body_data.parent_ = geometry;
			body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointUniversal;
			body_data.parameter_[3] = lepra::PIF * 0.5f;
			lWireAnchor -= lWireTransform.GetPosition();
			body_data.parameter_[6] = lWireAnchor.x;
			body_data.parameter_[7] = lWireAnchor.y;
			body_data.parameter_[8] = lWireAnchor.z;
			geometry = new tbc::ChunkyBoneCapsule(body_data, lWireThickness/2, lWireLength);
			structure.AddBoneGeometry(lWireTransform, geometry, body_data.parent_);

			lWireTransform.GetPosition().Add(0, 0, -lWireLength/2);
		}

		// Hook.
		const float lHookWeight = 20;
		const float lHookWidth = 0.25f;
		const float lHookHeight = 0.5f;
		lepra::vec3 lHookAnchor(lWireTransform.GetPosition());
		lepra::xform lHookTransform = lWireTransform;
		lHookTransform.GetPosition().Add(0, 0, -lHookHeight/2);

		body_data.mass_ = lHookWeight;
		body_data.parent_ = geometry;
		body_data.joint_type_ = tbc::ChunkyBoneGeometry::kJointUniversal;
		body_data.parameter_[3] = lepra::PIF * 0.5f;
		lHookAnchor -= lHookTransform.GetPosition();
		body_data.parameter_[6] = lHookAnchor.x;
		body_data.parameter_[7] = lHookAnchor.y;
		body_data.parameter_[8] = lHookAnchor.z;
		body_data.connector_type_ = tbc::ChunkyBoneGeometry::kConnectorBall;
		geometry = new tbc::ChunkyBoneBox(body_data, lepra::vec3(lHookWidth, lHookWidth, lHookHeight));
		structure.AddBoneGeometry(lHookTransform, geometry, body_data.parent_);

		// Jib rotational engine.
		tbc::PhysicsEngine* engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHingeTorque, lJibWeight, 2.0f, 1.0f, 0.01f, 1);
		engine->AddControlledGeometry(structure.GetBoneGeometry(1), 1.0f);
		structure.AddEngine(engine);

		ok = structure.FinalizeInit(physics, physics_fps, 0, 0);
		if (ok) {
			ok = WriteStructure(filename, structure);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
		deb_assert(ok);
	}
	if (ok) {
		context = "crane load";
		str filename("crane_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformLocal2World);
		ok = ReadStructure(filename, structure);
		deb_assert(ok);
		if (ok) {
			lepra::xform transformation;
			ok = structure.FinalizeInit(physics, physics_fps, &transformation, 0);
			deb_assert(ok);
		}
		if (ok) {
			ok = (structure.GetBoneCount() == 7 &&
				structure.GetPhysicsType() == tbc::ChunkyPhysics::kDynamic &&
				structure.GetBoneGeometry(0) != 0 &&
				structure.GetBoneGeometry(0)->GetBodyId() != tbc::INVALID_BODY &&
				structure.GetEngineCount() == 1);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
	}
	if (ok) {
		context = "world save";
		str filename("world_01.phys");

		static const float road_width = 5;
		static const float road_height = 7;
		static const float uphill_length = 70;
		static const float lUphillOrthogonalLength = uphill_length*::sin(lepra::PIF/4);
		static const float lPlateauLength = 15;
		static const float lPlateauLengthCompensation = road_height/1.5f;
		static const float floor_size = 500;
		lepra::xform transformation;

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformWorld2Local, tbc::ChunkyPhysics::kStatic);
		structure.SetBoneCount(6);
		tbc::ChunkyBoneGeometry::BodyData body_data(0.0f, 1.0f, 0.6f);

		lepra::vec3 dimensions(floor_size, floor_size, floor_size);
		tbc::ChunkyBoneGeometry* geometry = new tbc::ChunkyBoneBox(body_data, dimensions);
		structure.AddBoneGeometry(transformation, geometry);

		lepra::RotationMatrixF rotation;
		// Place lower climb.
		dimensions.Set(road_width, uphill_length, road_height);
		transformation.SetPosition(lepra::vec3(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength+road_width/2,
			0,
			lUphillOrthogonalLength/2+floor_size/2-road_height/2/1.5f));
		rotation.RotateAroundOwnX(lepra::PIF/4);
		transformation.SetOrientation(rotation);
		geometry = new tbc::ChunkyBoneBox(body_data, dimensions);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);
		// Upper climb.
		dimensions.Set(uphill_length, road_width, road_height);
		transformation.SetPosition(lepra::vec3(lPlateauLength+lUphillOrthogonalLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-road_width/2,
			lUphillOrthogonalLength*3/2+floor_size/2-road_height/2/1.5f));
		rotation = lepra::RotationMatrixF();
		rotation.RotateAroundOwnY(lepra::PIF/4);
		transformation.SetOrientation(rotation);
		geometry = new tbc::ChunkyBoneBox(body_data, dimensions);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);
		// First plateau.
		dimensions.Set(road_width, lPlateauLength+lPlateauLengthCompensation, road_height);
		transformation.SetPosition(lepra::vec3(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength+road_width/2,
			lUphillOrthogonalLength/2+lPlateauLength/2,
			lUphillOrthogonalLength+floor_size/2-road_height/2));
		transformation.SetOrientation(lepra::RotationMatrixF());
		geometry = new tbc::ChunkyBoneBox(body_data, dimensions);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);
		// Second plateau.
		dimensions.Set(lPlateauLength, road_width, road_height);
		transformation.SetPosition(lepra::vec3(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-road_width/2,
			lUphillOrthogonalLength+floor_size/2-road_height/2));
		geometry = new tbc::ChunkyBoneBox(body_data, dimensions);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);
		// Top plateau.
		dimensions.Set(lPlateauLength+lPlateauLengthCompensation, road_width, road_height);
		transformation.SetPosition(lepra::vec3(lPlateauLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-road_width/2,
			lUphillOrthogonalLength*2+floor_size/2-road_height/2));
		geometry = new tbc::ChunkyBoneBox(body_data, dimensions);
		structure.AddBoneGeometry(transformation, geometry, body_data.parent_);

		ok = structure.FinalizeInit(physics, physics_fps, 0,  0);
		if (ok) {
			ok = WriteStructure(filename, structure);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
		deb_assert(ok);
	}
	if (ok) {
		context = "world load";
		str filename("world_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformLocal2World);
		ok = ReadStructure(filename, structure);
		deb_assert(ok);
		if (ok) {
			lepra::xform transformation;
			ok = structure.FinalizeInit(physics, physics_fps, &transformation, 0);
			deb_assert(ok);
		}
		if (ok) {
			ok = (structure.GetBoneCount() == 6 &&
				structure.GetPhysicsType() == tbc::ChunkyPhysics::kStatic &&
				structure.GetBoneGeometry(0) != 0 &&
				structure.GetBoneGeometry(0)->GetBodyId() != tbc::INVALID_BODY &&
				structure.GetEngineCount() == 0);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
	}
	if (ok) {
		context = "tracktor load";
		str filename("tractor_01.phys");

		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformLocal2World);
		ok = ReadStructure(filename, structure);
		deb_assert(ok);
		if (ok) {
			lepra::xform transformation;
			ok = structure.FinalizeInit(physics, physics_fps, &transformation, 0, 0);
			deb_assert(ok);
		}
		if (ok) {
			ok = (structure.GetBoneCount() == 12 &&
				structure.GetPhysicsType() == tbc::ChunkyPhysics::kDynamic &&
				structure.GetBoneGeometry(0) != 0 &&
				structure.GetBoneGeometry(0)->GetBodyId() != tbc::INVALID_BODY &&
				structure.GetEngineCount() == 4);
			deb_assert(ok);
		}
		structure.ClearAll(physics);
	}

	delete (physics);

	ReportTestResult(gTbcLog, "ExportStructure", context, ok);
	return (ok);
}*/
