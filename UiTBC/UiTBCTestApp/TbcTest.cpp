
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include <assert.h>
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/DiskFile.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyLoader.h"
#include "../../TBC/Include/ChunkyStructure.h"
#include "../../TBC/Include/PhysicsEngineFactory.h"
#include "../../TBC/Include/StructureEngine.h"



void ReportTestResult(const Lepra::LogDecorator& pLog, const Lepra::String& pTestName, const Lepra::String& pContext, bool pResult);



class TbcTest
{
};
Lepra::LogDecorator gTbcLog(Lepra::LogType::GetLog(Lepra::LogType::SUB_TEST), typeid(TbcTest));



bool WriteStructure(const Lepra::String& pFilename, const TBC::ChunkyStructure& pStructure)
{
	bool lOk = true;
	Lepra::DiskFile lFile;
	if (lOk)
	{
		lOk = lFile.Open(pFilename, Lepra::DiskFile::MODE_WRITE);
	}
	if (lOk)
	{
		TBC::ChunkyStructureLoader lLoader(&lFile, false);
		lOk = lLoader.Save(&pStructure);
	}
	return (lOk);
}

bool ReadStructure(const Lepra::String& pFilename, TBC::ChunkyStructure& pStructure)
{
	bool lOk = true;
	Lepra::DiskFile lFile;
	if (lOk)
	{
		lOk = lFile.Open(pFilename, Lepra::DiskFile::MODE_READ);
	}
	if (lOk)
	{
		TBC::ChunkyStructureLoader lLoader(&lFile, false);
		lOk = lLoader.Load(&pStructure);
	}
	return (lOk);
}

bool ExportStructure()
{
	Lepra::String lContext;
	bool lOk = true;

	int lPhysicsFps = 60;
	TBC::PhysicsEngine* lPhysics = TBC::PhysicsEngineFactory::Create(TBC::PhysicsEngineFactory::ENGINE_ODE);
	if (lOk)
	{
		lContext = _T("box save");
		Lepra::String lFilename(_T("box_01.str"));

		Lepra::Vector3DF lDimensions(2.0f, 1.0f, 3.5f);

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_WORLD2LOCAL, TBC::ChunkyStructure::DYNAMIC);
		lStructure.SetBoneCount(1);

		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(
			TBC::ChunkyBoneGeometry::BodyData(1.0f, 0.2f, 1.0f), lDimensions);
		lStructure.AddBoneGeometry(Lepra::TransformationF(), lGeometry);

		lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, 0, 0, 0);
		if (lOk)
		{
			lOk = WriteStructure(lFilename, lStructure);
		}
		lStructure.ClearAll(lPhysics);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("box load");
		Lepra::String lFilename(_T("box_01.str"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_LOCAL2WORLD);
		lOk = ReadStructure(lFilename, lStructure);
		assert(lOk);
		if (lOk)
		{
			Lepra::TransformationF lTransformation;
			lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, &lTransformation, 0, 0);
			assert(lOk);
		}
		if (lOk)
		{
			lOk = (lStructure.GetBoneCount() == 1 &&
				lStructure.GetPhysicsType() == TBC::ChunkyStructure::DYNAMIC &&
				lStructure.GetBoneGeometry(0) != 0 &&
				lStructure.GetBoneGeometry(0)->GetBodyId() != TBC::INVALID_BODY &&
				lStructure.GetEngineCount() == 0);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
	}
	if (lOk)
	{
		lContext = _T("sphere save");
		Lepra::String lFilename(_T("sphere_01.str"));

		const float lRadius = 1.0f;

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_WORLD2LOCAL, TBC::ChunkyStructure::DYNAMIC);
		lStructure.SetBoneCount(1);

		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneSphere(
			TBC::ChunkyBoneGeometry::BodyData(1.0f, 0.2f, 1.0f), lRadius);
		lStructure.AddBoneGeometry(Lepra::TransformationF(), lGeometry);

		TBC::StructureEngine* lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_CAMERA_FLAT_PUSH,
			55, 50, 0, 0);
		lEngine->AddControlledGeometry(lGeometry, 1);
		lStructure.AddEngine(lEngine);

		lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, 0, 0, 0);
		if (lOk)
		{
			lOk = WriteStructure(lFilename, lStructure);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("sphere load");
		Lepra::String lFilename(_T("sphere_01.str"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_LOCAL2WORLD);
		lOk = ReadStructure(lFilename, lStructure);
		assert(lOk);
		if (lOk)
		{
			Lepra::TransformationF lTransformation;
			lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, &lTransformation, 0, 0);
			assert(lOk);
		}
		if (lOk)
		{
			lOk = (lStructure.GetBoneCount() == 1 &&
				lStructure.GetPhysicsType() == TBC::ChunkyStructure::DYNAMIC &&
				lStructure.GetBoneGeometry(0) != 0 &&
				lStructure.GetBoneGeometry(0)->GetBodyId() != TBC::INVALID_BODY &&
				lStructure.GetEngineCount() == 1);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
	}

	const float lVolvoMass = 1800;
	// Wheels and suspension.
	const float lVolvoSpringConstant = lVolvoMass*50;
	const float lVolvoDampingConstant = lVolvoMass/5;

	if (lOk)
	{
		lContext = _T("car save");
		Lepra::String lFilename(_T("car_01.str"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_WORLD2LOCAL, TBC::ChunkyStructure::DYNAMIC);
		lStructure.SetBoneCount(6);

		const float lWheelRadius = 0.3f;
		const float lWheelXOffset = -0.15f;
		const float lWheelZOffset = 0.3f;
		const float lBackWheelYOffset = -0.8f;
		const float lWheelYDistance = 3.6f;
		const Lepra::Vector3DF lBodyDimensions(1.9f, 4.9f, 0.6f);
		const Lepra::Vector3DF lTopDimensions(1.6f, 2.9f, 0.6f);
		Lepra::TransformationF lTransformation;

		// Body.
		TBC::ChunkyBoneGeometry::BodyData lBodyData(lVolvoMass, 0.1f, 1.0f, 0,
			TBC::ChunkyBoneGeometry::JOINT_EXCLUDE, TBC::ChunkyBoneGeometry::CONNECTEE_3DOF);
		TBC::ChunkyBoneGeometry* lBodyGeometry = new TBC::ChunkyBoneBox(lBodyData, lBodyDimensions);
		lStructure.AddBoneGeometry(lTransformation, lBodyGeometry);
		// Top of body.
		Lepra::TransformationF lTopTransform(lTransformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		lTopTransform.MoveForward(0.9f);
		lBodyData.mMass = lVolvoMass/5;
		lBodyData.mFriction = 0.5f;
		lBodyData.mParent = lBodyGeometry;
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(lBodyData, lTopDimensions);
		lStructure.AddBoneGeometry(lTopTransform, lGeometry, lBodyData.mParent);

		// Wheels and suspension.
		lTransformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF/2);
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_SUSPEND_HINGE;
		lBodyData.mConnectorType = TBC::ChunkyBoneGeometry::CONNECT_NONE;
		lBodyData.mMass = lVolvoMass/50;
		lBodyData.mFriction = 1.0f;
		lBodyData.mBounce = 0.5f;
		lBodyData.mParameter[0] = lVolvoSpringConstant;
		lBodyData.mParameter[1] = lVolvoDampingConstant;
		lBodyData.mParameter[2] = 0;
		lBodyData.mParameter[3] = Lepra::PIF * 0.5f;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[2] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_HINGE2;
		lBodyData.mParameter[2] = 0;
		lBodyData.mParameter[4] = -0.5f;
		lBodyData.mParameter[5] = 0.5f;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[2] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		// Front wheel drive engine.
		TBC::StructureEngine* lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_ROLL, 1*lVolvoMass, 300, 20, 0);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(4), -1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(5), +1);
		lStructure.AddEngine(lEngine);
		// Turning front wheels.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_TURN, 1*lVolvoMass, 1.0f, 0, 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(4), +1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(5), +1);
		lStructure.AddEngine(lEngine);
		// Normal breaks (all nodes, scaled >= 0) and handbreak (rear wheels, nodes with high scaling values).
		const float lBreakInputThreashold = 0.02f;
		TBC::StructureEngine* lBreak = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_BREAK, 8*lVolvoMass, lBreakInputThreashold, 0, 2);
		lBreak->AddControlledGeometry(lStructure.GetBoneGeometry(2), 100);
		lBreak->AddControlledGeometry(lStructure.GetBoneGeometry(3), 100);
		lBreak->AddControlledGeometry(lStructure.GetBoneGeometry(4), 0);
		lBreak->AddControlledGeometry(lStructure.GetBoneGeometry(5), 0);
		lStructure.AddEngine(lBreak);
		// Just a "dummy engine" to keep rear wheel straight at all times.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_ROLL_STRAIGHT, 0, 0, 0, 0);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(2), -1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(3), +1);
		lStructure.AddEngine(lEngine);

		lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, 0, 0, 0);
		if (lOk)
		{
			lOk = WriteStructure(lFilename, lStructure);
			assert(lOk);
		}
		if (lOk)
		{
			float lLowStop = -1;
			float lHighStop = -1;
			float lBounce = -1;
			lPhysics->GetJointParams(lStructure.GetBoneGeometry(4)->GetJointId(), lLowStop, lHighStop, lBounce);
			if (lOk)
			{
				lOk = (lLowStop == -0.5f && lHighStop == 0.5f && lBounce == 0.0f);
			}
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("car load");
		Lepra::String lFilename(_T("car_01.str"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_LOCAL2WORLD);
		lOk = ReadStructure(lFilename, lStructure);
		assert(lOk);
		if (lOk)
		{
			Lepra::TransformationF lTransform;
			lTransform.SetPosition(Lepra::Vector3DF(100, 100, 100));
			lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, &lTransform, 0, 0);
			assert(lOk);
		}
		if (lOk)
		{
			lOk = (lStructure.GetBoneCount() == 6 &&
				lStructure.GetPhysicsType() == TBC::ChunkyStructure::DYNAMIC &&
				lStructure.GetBoneGeometry(0) != 0 &&
				lStructure.GetBoneGeometry(4)->GetBodyId() != TBC::INVALID_BODY &&
				lStructure.GetBoneGeometry(4)->GetJointId() != TBC::INVALID_JOINT &&
				lStructure.GetEngineCount() == 4);
			assert(lOk);
		}
		if (lOk)
		{
			Lepra::Vector3DF lPosition = lPhysics->GetBodyPosition(lStructure.GetBoneGeometry(4)->GetBodyId());
			lOk = (Lepra::Math::IsInRange(lPosition.x, 99.1f, 99.3f) &&
				Lepra::Math::IsInRange(lPosition.y, 98.0f, 98.1f) &&
				Lepra::Math::IsInRange(lPosition.z, 99.6f, 99.8f));
			assert(lOk);
		}
		if (lOk)
		{
			float lLowStop = -1;
			float lHighStop = -1;
			float lBounce = -1;
			lPhysics->GetJointParams(lStructure.GetBoneGeometry(4)->GetJointId(), lLowStop, lHighStop, lBounce);
			if (lOk)
			{
				lOk = (lLowStop == -0.5f && lHighStop == 0.5f && lBounce == 0.0f);
			}
			assert(lOk);
		}
		if (lOk)
		{
			float lErp = -1;
			float lCfm = -1;
			lOk = lPhysics->GetSuspension(lStructure.GetBoneGeometry(4)->GetJointId(), lErp, lCfm);
			if (lOk)
			{
				const float lFrameTime = 1/(float)lPhysicsFps;
				const float lCorrectErp = lFrameTime * lVolvoSpringConstant / (lFrameTime * lVolvoSpringConstant + lVolvoDampingConstant);
				const float lCorrectCfm = 1 / (lFrameTime * lVolvoSpringConstant + lVolvoDampingConstant);
				lOk = (lErp == lCorrectErp && lCfm == lCorrectCfm);
			}
			assert(lOk);
		}
		if (lOk)
		{
			Lepra::TransformationF lTransform;
			lPhysics->GetBodyTransform(lStructure.GetBoneGeometry(4)->GetBodyId(), lTransform);
			lTransform.GetPosition().z += 1.0f;
			lPhysics->SetBodyTransform(lStructure.GetBoneGeometry(4)->GetBodyId(), lTransform);
			TBC::PhysicsEngine::Joint3Diff lDiff;
			lOk = lPhysics->GetJoint3Diff(lStructure.GetBoneGeometry(4)->GetBodyId(),
				lStructure.GetBoneGeometry(4)->GetJointId(), lDiff);
			assert(lOk);
			if (lOk)
			{
				lOk = (Lepra::Math::IsEpsEqual(lDiff.mValue, -1.0f));
			}
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
	}
	if (lOk)
	{
		lContext = _T("monster save");
		Lepra::String lFilename(_T("monster_01.str"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_WORLD2LOCAL, TBC::ChunkyStructure::DYNAMIC);
		lStructure.SetBoneCount(6);

		const float lCarWeight = 3000;
		const float lWheelRadius = 1.0f;
		const float lWheelXOffset = 0.5f;
		const float lWheelZOffset = 0.8f;
		const float lBackWheelYOffset = 0.0f;
		const float lWheelYDistance = 5.0f;
		const Lepra::Vector3DF lBodyDimensions(3.0f, 5.0f, 0.7f);
		const Lepra::Vector3DF lTopDimensions(3.0f, 3.0f, 0.7f);
		Lepra::TransformationF lTransformation;

		// Body.
		TBC::ChunkyBoneGeometry::BodyData lBodyData(lCarWeight, 0.1f, 0.5f);
		TBC::ChunkyBoneGeometry* lBodyGeometry = new TBC::ChunkyBoneBox(lBodyData, lBodyDimensions);
		lStructure.AddBoneGeometry(lTransformation, lBodyGeometry);
		// Top of body.
		Lepra::TransformationF lTopTransform(lTransformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		lBodyData.mMass = lCarWeight/10;
		lBodyData.mFriction = 0.8f;
		lBodyData.mParent = lBodyGeometry;
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(lBodyData, lTopDimensions);
		lStructure.AddBoneGeometry(lTopTransform, lGeometry, lBodyData.mParent);

		// Wheels and suspension.
		const float lSpringConstant = lCarWeight*4;
		const float lDamperConstant = lCarWeight/50;

		lTransformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF/2);
		lBodyData.mMass = lCarWeight/50;
		lBodyData.mFriction = 1.0f;
		lBodyData.mBounce = 0.5f;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_SUSPEND_HINGE;
		lBodyData.mConnectorType = TBC::ChunkyBoneGeometry::CONNECT_NONE;
		lBodyData.mParameter[0] = lSpringConstant;
		lBodyData.mParameter[1] = lDamperConstant;
		lBodyData.mParameter[2] = 0;
		lBodyData.mParameter[3] = Lepra::PIF * 0.5f;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[2] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lBodyData.mParameter[2] = 0;
		lBodyData.mParameter[4] = -0.5f;
		lBodyData.mParameter[5] = 0.5f;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[2] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		// Rear wheel drive engine.
		TBC::StructureEngine* lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_ROLL, 5*lCarWeight, 300, 20, 0);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(2), -1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(3), 1);
		lStructure.AddEngine(lEngine);
		// Turning front wheels.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_TURN, 1*lCarWeight, 0.5f, 0, 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(4), 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(5), 1);
		lStructure.AddEngine(lEngine);
		// Normal breaks (all nodes, scaled >= 0) and handbreak (rear wheels, nodes with high scaling values).
		const float lBreakInputThreashold = 0.02f;
		TBC::StructureEngine* lBreak = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_BREAK, 8*lCarWeight, lBreakInputThreashold, 0, 2);
		lBreak->AddControlledGeometry(lStructure.GetBoneGeometry(2), 100);
		lBreak->AddControlledGeometry(lStructure.GetBoneGeometry(3), 100);
		lBreak->AddControlledGeometry(lStructure.GetBoneGeometry(4), 0);
		lBreak->AddControlledGeometry(lStructure.GetBoneGeometry(5), 0);
		lStructure.AddEngine(lBreak);
		// Just a "dummy engine" to keep rear wheel straight at all times.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_ROLL_STRAIGHT, 0, 0, 0, 0);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(2), -1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(3), 1);
		lStructure.AddEngine(lEngine);

		lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, 0, 0, 0);
		if (lOk)
		{
			lOk = WriteStructure(lFilename, lStructure);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("monster load");
		Lepra::String lFilename(_T("monster_01.str"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_LOCAL2WORLD);
		lOk = ReadStructure(lFilename, lStructure);
		assert(lOk);
		if (lOk)
		{
			Lepra::TransformationF lTransformation;
			lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, &lTransformation, 0, 0);
			assert(lOk);
		}
		if (lOk)
		{
			lOk = (lStructure.GetBoneCount() == 6 &&
				lStructure.GetPhysicsType() == TBC::ChunkyStructure::DYNAMIC &&
				lStructure.GetBoneGeometry(0) != 0 &&
				lStructure.GetBoneGeometry(0)->GetBodyId() != TBC::INVALID_BODY &&
				lStructure.GetEngineCount() == 4);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
	}
	if (lOk)
	{
		lContext = _T("excavator save");
		Lepra::String lFilename(_T("excavator_01.str"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_WORLD2LOCAL, TBC::ChunkyStructure::DYNAMIC);
		lStructure.SetBoneCount(13);

		const float lCarWeight = 18000;
		const float lWheelRadius = 0.5f;
		const float lWheelXOffset = 0.2f;
		const float lWheelZOffset = 0.6f;
		const float lBackWheelYOffset = 0.5f;
		const float lWheelYDistance = 2.0f;
		const Lepra::Vector3DF lBodyDimensions(2.9f, 3.0f, 1.5f);
		const Lepra::Vector3DF lTopDimensions(1.0f, 1.5f, 0.6f);
		Lepra::TransformationF lTransformation;

		// Body.
		const Lepra::TransformationF lBodyTransformation(lTransformation);
		TBC::ChunkyBoneGeometry::BodyData lBodyData(lCarWeight, 0.5f, 0.5f);
		TBC::ChunkyBoneGeometry* lBodyGeometry = new TBC::ChunkyBoneBox(lBodyData, lBodyDimensions);
		lStructure.AddBoneGeometry(lTransformation, lBodyGeometry);
		// Top of body.
		Lepra::TransformationF lTopTransform(lTransformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		lTopTransform.MoveRight(1.0f);
		lTopTransform.MoveBackward(0.75f);
		lBodyData.mMass = lCarWeight/20;
		lBodyData.mBounce = 1.0f;
		lBodyData.mParent = lBodyGeometry;
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(lBodyData, lTopDimensions);
		lStructure.AddBoneGeometry(lTopTransform, lGeometry, lBodyData.mParent);

		// Wheels and suspension.
		const float lSpringConstant = lCarWeight*100;
		const float lDamperConstant = lCarWeight/5;

		lTransformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF/2);
		lBodyData.mMass = lCarWeight/50;
		lBodyData.mFriction = 4.0f;
		lBodyData.mBounce = 0.2f;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_SUSPEND_HINGE;
		lBodyData.mConnectorType = TBC::ChunkyBoneGeometry::CONNECT_NONE;
		lBodyData.mParameter[0] = lSpringConstant;
		lBodyData.mParameter[1] = lDamperConstant;
		lBodyData.mParameter[2] = 0;
		lBodyData.mParameter[3] = Lepra::PIF * 0.5f;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[2] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lBodyData.mParameter[2] = 0;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[2] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lBodyData.mParameter[2] = 0;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[2] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		// Boom.
		const Lepra::Vector3DF lBoom1Dimensions(0.6f, 0.6f, 2.5f);
		const Lepra::Vector3DF lBoom2Dimensions(0.6f, 0.6f, 3.2f);
		lTransformation = lBodyTransformation;
		lTransformation.MoveUp(lBodyDimensions.z/2);
		lTransformation.MoveLeft(0.5f);
		lTransformation.MoveBackward(0.75f);
		Lepra::Vector3DF lBoomAnchor(lTransformation.GetPosition());
		lTransformation.MoveUp(lBoom1Dimensions.z/2);
		lBodyData.mMass = lCarWeight*3/(12*5);
		lBodyData.mFriction = 1.0f;
		lBodyData.mBounce = 0.2f;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_HINGE;
		lBodyData.mParameter[2] = 0;
		lBodyData.mParameter[3] = Lepra::PIF * 0.5f;
		lBodyData.mParameter[4] = -1.2f;
		lBodyData.mParameter[5] = 0.5f;
		lBoomAnchor -= lTransformation.GetPosition();
		lBodyData.mParameter[6] = lBoomAnchor.x;
		lBodyData.mParameter[7] = lBoomAnchor.y;
		lBodyData.mParameter[8] = lBoomAnchor.z;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lBoom1Dimensions);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);
		// Boom, part 2.
		lTransformation.MoveUp(lBoom1Dimensions.z/2);
		const float lBoom2Angle = Lepra::PIF/4;
		lTransformation.MoveBackward(::sin(lBoom2Angle)*lBoom2Dimensions.z/2);
		lTransformation.MoveUp(::cos(lBoom2Angle)*lBoom2Dimensions.z/2);
		Lepra::TransformationF lBoom2Transform(lTransformation);
		lBoom2Transform.RotatePitch(lBoom2Angle);
		lBodyData.mMass = lCarWeight*2/(12*5);
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_EXCLUDE;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lBoom2Dimensions);
		lStructure.AddBoneGeometry(lBoom2Transform, lGeometry, lBodyData.mParent);

		// Arm.
		lTransformation.MoveBackward(::sin(lBoom2Angle)*lBoom2Dimensions.z/2);
		lTransformation.MoveUp(::cos(lBoom2Angle)*lBoom2Dimensions.z/2);
		Lepra::Vector3DF lArmAnchor(lTransformation.GetPosition());
		const Lepra::Vector3DF lArmDimensions(0.4f, 3.0f, 0.4f);
		lTransformation.MoveBackward(lArmDimensions.y/2);
		lBodyData.mMass = lCarWeight/25;
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_HINGE;
		lBodyData.mParameter[4] = -1.0f;
		lBodyData.mParameter[5] = 0.5f;
		lArmAnchor -= lTransformation.GetPosition();
		lBodyData.mParameter[6] = lArmAnchor.x;
		lBodyData.mParameter[7] = lArmAnchor.y;
		lBodyData.mParameter[8] = lArmAnchor.z;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lArmDimensions);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		// Bucket.
		lTransformation.MoveBackward(lArmDimensions.y/2);
		Lepra::Vector3DF lBucketAnchor(lTransformation.GetPosition());
		const Lepra::Vector3DF lBucketBackDimensions(1.5f, 0.8f, 0.1f);
		const float lBucketBackAngle = Lepra::PIF/4;
		lTransformation.MoveBackward(::cos(lBucketBackAngle)*lBucketBackDimensions.y/2);
		lTransformation.MoveUp(::sin(lBucketBackAngle)*lBucketBackDimensions.y/2);
		Lepra::TransformationF lBucketBackTransform(lTransformation);
		lBucketBackTransform.RotatePitch(-lBucketBackAngle);
		lBodyData.mMass = lCarWeight/100;
		lBodyData.mFriction = 10.0f;
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_HINGE;
		lBodyData.mParameter[4] = -1.0f;
		lBodyData.mParameter[5] = 0.5f;
		lBodyData.mParameter[6] = 0;
		lBodyData.mParameter[7] = lBucketBackDimensions.y/2;
		lBodyData.mParameter[8] = 0;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lBucketBackDimensions);
		lStructure.AddBoneGeometry(lBucketBackTransform, lGeometry, lBodyData.mParent);
		// Bucket, floor part.
		lTransformation.MoveBackward(::cos(lBucketBackAngle)*lBucketBackDimensions.y/2);
		lTransformation.MoveUp(::sin(lBucketBackAngle)*lBucketBackDimensions.y/2);
		const Lepra::Vector3DF lBucketFloorDimensions(1.5f, 1.0f, 0.1f);
		const float lBucketFloorAngle = Lepra::PIF/4;
		lTransformation.MoveBackward(::cos(lBucketFloorAngle)*lBucketFloorDimensions.y/2);
		lTransformation.MoveDown(::sin(lBucketFloorAngle)*lBucketFloorDimensions.y/2);
		Lepra::TransformationF lBucketFloorTransform(lTransformation);
		lBucketFloorTransform.RotatePitch(lBucketFloorAngle);
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_EXCLUDE;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lBucketBackDimensions);
		lStructure.AddBoneGeometry(lBucketFloorTransform, lGeometry, lBodyData.mParent);

		// All wheel drive engine.
		TBC::StructureEngine* lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_ROLL, lCarWeight, 25, 25, 0);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(2), -1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(3), 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(4), -1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(5), 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(6), -1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(7), 1);
		lStructure.AddEngine(lEngine);
		// Turning with tracks are controlled by rolling of the wheels.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_ROLL, lCarWeight, 20, 20, 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(2), 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(3), 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(4), 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(5), 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(6), 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(7), 1);
		lStructure.AddEngine(lEngine);
		// Normal breaks (all nodes, scaled >= 0) and handbreak (nodes with high scaling values).
		const float lBreakInputThreashold = 0.02f;
		TBC::StructureEngine* lBreak = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_BREAK, lCarWeight, lBreakInputThreashold, 0, 2);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(2), 100);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(3), 100);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(4), 100);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(5), 100);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(6), 100);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(7), 100);
		lStructure.AddEngine(lBreak);
		// The boom, arm and bucket are hinge-controlled.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE, 20*lCarWeight, 2.0f, 1.0f, 3);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(8), 5);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(10), 2, TBC::StructureEngine::MODE_HALF_LOCK);	// MODE_HALF_LOCK = has neutral/freeze position outside mid value.
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(11), 1, TBC::StructureEngine::MODE_HALF_LOCK);	// MODE_HALF_LOCK = has neutral/freeze position outside mid value.
		lStructure.AddEngine(lEngine);
		// Just a "dummy engine" to keep wheels straight at all times.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_ROLL_STRAIGHT, 0, 0, 0, 0);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(2), -1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(3), 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(4), -1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(5), 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(6), -1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(7), 1);
		lStructure.AddEngine(lEngine);

		lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, 0, 0, 0);
		if (lOk)
		{
			const Lepra::Vector3DF& lBucketPosition = lStructure.GetBoneTransformation(11).GetPosition();
			lOk = (Lepra::Math::IsInRange(lBucketPosition.x, -0.01f, 0.01f));
			assert(lOk);
		}
		if (lOk)
		{
			lOk = WriteStructure(lFilename, lStructure);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("excavator load");
		Lepra::String lFilename(_T("excavator_01.str"));

		gTbcLog.Debugf(_T("--> Loading excavator. <--"));
		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_LOCAL2WORLD);
		lOk = ReadStructure(lFilename, lStructure);
		assert(lOk);
		if (lOk)
		{
			const Lepra::Vector3DF& lBucketPosition = lStructure.GetOriginalBoneTransformation(11).GetPosition();
			lOk = (Lepra::Math::IsInRange(lBucketPosition.x, -0.01f, 0.01f));
			assert(lOk);
		}
		if (lOk)
		{
			Lepra::TransformationF lTransformation;
			lTransformation.SetPosition(Lepra::Vector3DF(4, 5, 6));
			lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, &lTransformation, 0, 0);
			assert(lOk);
		}
		if (lOk)
		{
			lOk = (lStructure.GetBoneCount() == 13 &&
				lStructure.GetPhysicsType() == TBC::ChunkyStructure::DYNAMIC &&
				lStructure.GetBoneGeometry(0) != 0 &&
				lStructure.GetBoneGeometry(0)->GetBodyId() != TBC::INVALID_BODY &&
				lStructure.GetBoneGeometry(8)->GetParent() == lStructure.GetBoneGeometry(0) &&
				lStructure.GetEngineCount() == 5);
			assert(lOk);
		}
		if (lOk)
		{
			Lepra::Vector3DF lBoomAnchor;
			Lepra::Vector3DF lArmAnchor;
			Lepra::Vector3DF lBucketAnchor;
			lOk = (lPhysics->GetAnchorPos(lStructure.GetBoneGeometry(8)->GetJointId(), lBoomAnchor) &&
				lPhysics->GetAnchorPos(lStructure.GetBoneGeometry(10)->GetJointId(), lArmAnchor) &&
				lPhysics->GetAnchorPos(lStructure.GetBoneGeometry(11)->GetJointId(), lBucketAnchor));
			assert(lOk);
			if (lOk)
			{
				lOk = (Lepra::Math::IsInRange(lBoomAnchor.x, 3.45f, 3.55f) &&
					Lepra::Math::IsInRange(lBoomAnchor.y, 4.2f, 4.3f) &&
					Lepra::Math::IsInRange(lBoomAnchor.z, 6.7f, 6.8f) &&
					Lepra::Math::IsInRange(lArmAnchor.x, 3.45f, 3.55f) &&
					Lepra::Math::IsInRange(lArmAnchor.y, 1.98f, 1.99f) &&
					Lepra::Math::IsInRange(lArmAnchor.z, 11.51f, 11.52f) &&
					Lepra::Math::IsInRange(lBucketAnchor.x, 3.45f, 3.55f) &&
					Lepra::Math::IsInRange(lBucketAnchor.y, 1.98f-3, 1.99f-3) &&
					Lepra::Math::IsInRange(lBucketAnchor.z, 11.51f, 11.52f));
				assert(lOk);
			}
		}
		lStructure.ClearAll(lPhysics);
	}
	if (lOk)
	{
		lContext = _T("crane save");
		Lepra::String lFilename(_T("crane_01.str"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_WORLD2LOCAL, TBC::ChunkyStructure::DYNAMIC);
		lStructure.SetBoneCount(7);

		const float lTowerWeight = 50000;
		const float lTowerHeight = 25.0f;
		const float lThickness = 2.5f;
		const Lepra::Vector3DF lTowerDimensions(lThickness, lThickness, lTowerHeight);
		Lepra::TransformationF lTransformation;
		lTransformation.SetPosition(Lepra::Vector3DF(0, 0, lTowerHeight/2));

		// Body.
		TBC::ChunkyBoneGeometry::BodyData lBodyData(lTowerWeight, 1.0f, 0.5f);
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(lBodyData, lTowerDimensions);
		lStructure.AddBoneGeometry(lTransformation, lGeometry);

		// Jib.
		const float lJibWeight = lTowerWeight/3.5f;
		const float lJibPlacementHeight = 20.0f;
		const float lJibLength = 15.0f;
		const float lCounterJibLength = 5.0f;
		const float lTotalJibLength = lJibLength+lCounterJibLength;

		Lepra::TransformationF lJibTransform(lTransformation);
		lJibTransform.GetPosition().Add(0, 0, -lTowerHeight/2 + lJibPlacementHeight);
		Lepra::Vector3DF lJibAnchor(lJibTransform.GetPosition());
		lJibTransform.GetPosition().Add(0, lTotalJibLength/2-lCounterJibLength, 0);
		const Lepra::Vector3DF lJibDimensions(lThickness, lTotalJibLength, lThickness);
		lBodyData.mMass = lJibWeight;
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_HINGE;
		lBodyData.mParameter[4] = -1e10f;
		lBodyData.mParameter[5] = 1e10f;
		lJibAnchor -= lJibTransform.GetPosition();
		lBodyData.mParameter[6] = lJibAnchor.x;
		lBodyData.mParameter[7] = lJibAnchor.y;
		lBodyData.mParameter[8] = lJibAnchor.z;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lJibDimensions);
		lStructure.AddBoneGeometry(lJibTransform, lGeometry, lBodyData.mParent);

		// Wire.
		const float lWireWeight = 50;
		const int lWireSegmentCount = 4;
		const float lWireSegmentWeight = lWireWeight/lWireSegmentCount;
		const float lWireLength = (lJibPlacementHeight-1)/lWireSegmentCount;
		const float lWireThickness = 0.05f;
		Lepra::TransformationF lWireTransform(lJibTransform);
		lWireTransform.GetPosition().Add(0, lTotalJibLength/4, 0);
		for (int x = 0; x < lWireSegmentCount; ++x)
		{
			Lepra::Vector3DF lWireAnchor(lWireTransform.GetPosition());
			lWireTransform.GetPosition().Add(0, 0, -lWireLength/2);

			lBodyData.mMass = lWireSegmentWeight;
			lBodyData.mParent = lGeometry;
			lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_UNIVERSAL;
			lBodyData.mParameter[3] = Lepra::PIF * 0.5f;
			lWireAnchor -= lWireTransform.GetPosition();
			lBodyData.mParameter[6] = lWireAnchor.x;
			lBodyData.mParameter[7] = lWireAnchor.y;
			lBodyData.mParameter[8] = lWireAnchor.z;
			lGeometry = new TBC::ChunkyBoneCapsule(lBodyData, lWireThickness/2, lWireLength);
			lStructure.AddBoneGeometry(lWireTransform, lGeometry, lBodyData.mParent);

			lWireTransform.GetPosition().Add(0, 0, -lWireLength/2);
		}

		// Hook.
		const float lHookWeight = 20;
		const float lHookWidth = 0.25f;
		const float lHookHeight = 0.5f;
		Lepra::Vector3DF lHookAnchor(lWireTransform.GetPosition());
		Lepra::TransformationF lHookTransform = lWireTransform;
		lHookTransform.GetPosition().Add(0, 0, -lHookHeight/2);

		lBodyData.mMass = lHookWeight;
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::JOINT_UNIVERSAL;
		lBodyData.mParameter[3] = Lepra::PIF * 0.5f;
		lHookAnchor -= lHookTransform.GetPosition();
		lBodyData.mParameter[6] = lHookAnchor.x;
		lBodyData.mParameter[7] = lHookAnchor.y;
		lBodyData.mParameter[8] = lHookAnchor.z;
		lBodyData.mConnectorType = TBC::ChunkyBoneGeometry::CONNECTOR_3DOF;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, Lepra::Vector3DF(lHookWidth, lHookWidth, lHookHeight));
		lStructure.AddBoneGeometry(lHookTransform, lGeometry, lBodyData.mParent);

		// Jib rotational engine.
		TBC::StructureEngine* lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE, lJibWeight, 2.0f, 1.0f, 1);
		lEngine->AddControlledGeometry(lStructure.GetBoneGeometry(1), 1.0f);
		lStructure.AddEngine(lEngine);

		lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, 0, 0, 0);
		if (lOk)
		{
			lOk = WriteStructure(lFilename, lStructure);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("crane load");
		Lepra::String lFilename(_T("crane_01.str"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_LOCAL2WORLD);
		lOk = ReadStructure(lFilename, lStructure);
		assert(lOk);
		if (lOk)
		{
			Lepra::TransformationF lTransformation;
			lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, &lTransformation, 0, 0);
			assert(lOk);
		}
		if (lOk)
		{
			lOk = (lStructure.GetBoneCount() == 7 &&
				lStructure.GetPhysicsType() == TBC::ChunkyStructure::DYNAMIC &&
				lStructure.GetBoneGeometry(0) != 0 &&
				lStructure.GetBoneGeometry(0)->GetBodyId() != TBC::INVALID_BODY &&
				lStructure.GetEngineCount() == 1);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
	}
	if (lOk)
	{
		lContext = _T("world save");
		Lepra::String lFilename(_T("world_01.str"));

		static const float lRoadWidth = 5;
		static const float lRoadHeight = 7;
		static const float lUphillLength = 70;
		static const float lUphillOrthogonalLength = lUphillLength*::sin(Lepra::PIF/4);
		static const float lPlateauLength = 15;
		static const float lPlateauLengthCompensation = lRoadHeight/1.5f;
		static const float lFloorSize = 500;
		Lepra::TransformationF lTransformation;

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_WORLD2LOCAL, TBC::ChunkyStructure::STATIC);
		lStructure.SetBoneCount(6);
		TBC::ChunkyBoneGeometry::BodyData lBodyData(0.0f, 1.0f, 0.6f);

		Lepra::Vector3DF lDimensions(lFloorSize, lFloorSize, lFloorSize);
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure.AddBoneGeometry(lTransformation, lGeometry);

		Lepra::RotationMatrixF lRotation;
		// Place lower climb.
		lDimensions.Set(lRoadWidth, lUphillLength, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength+lRoadWidth/2,
			0,
			lUphillOrthogonalLength/2+lFloorSize/2-lRoadHeight/2/1.5f));
		lRotation.RotateAroundOwnX(Lepra::PIF/4);
		lTransformation.SetOrientation(lRotation);
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);
		// Upper climb.
		lDimensions.Set(lUphillLength, lRoadWidth, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(lPlateauLength+lUphillOrthogonalLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength*3/2+lFloorSize/2-lRoadHeight/2/1.5f));
		lRotation = Lepra::RotationMatrixF();
		lRotation.RotateAroundOwnY(Lepra::PIF/4);
		lTransformation.SetOrientation(lRotation);
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);
		// First plateau.
		lDimensions.Set(lRoadWidth, lPlateauLength+lPlateauLengthCompensation, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength+lRoadWidth/2,
			lUphillOrthogonalLength/2+lPlateauLength/2,
			lUphillOrthogonalLength+lFloorSize/2-lRoadHeight/2));
		lTransformation.SetOrientation(Lepra::RotationMatrixF());
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);
		// Second plateau.
		lDimensions.Set(lPlateauLength, lRoadWidth, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength+lFloorSize/2-lRoadHeight/2));
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);
		// Top plateau.
		lDimensions.Set(lPlateauLength+lPlateauLengthCompensation, lRoadWidth, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(lPlateauLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength*2+lFloorSize/2-lRoadHeight/2));
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure.AddBoneGeometry(lTransformation, lGeometry, lBodyData.mParent);

		lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, 0,  0, 0);
		if (lOk)
		{
			lOk = WriteStructure(lFilename, lStructure);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("world load");
		Lepra::String lFilename(_T("world_01.str"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_LOCAL2WORLD);
		lOk = ReadStructure(lFilename, lStructure);
		assert(lOk);
		if (lOk)
		{
			Lepra::TransformationF lTransformation;
			lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, &lTransformation, 0, 0);
			assert(lOk);
		}
		if (lOk)
		{
			lOk = (lStructure.GetBoneCount() == 6 &&
				lStructure.GetPhysicsType() == TBC::ChunkyStructure::STATIC &&
				lStructure.GetBoneGeometry(0) != 0 &&
				lStructure.GetBoneGeometry(0)->GetBodyId() != TBC::INVALID_BODY &&
				lStructure.GetEngineCount() == 0);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
	}
	if (lOk)
	{
		lContext = _T("tracktor load");
		Lepra::String lFilename(_T("tractor_01.phys"));

		TBC::ChunkyStructure lStructure(TBC::BoneHierarchy::TRANSFORM_LOCAL2WORLD);
		lOk = ReadStructure(lFilename, lStructure);
		assert(lOk);
		if (lOk)
		{
			Lepra::TransformationF lTransformation;
			lOk = lStructure.FinalizeInit(lPhysics, lPhysicsFps, &lTransformation, 0, 0);
			assert(lOk);
		}
		if (lOk)
		{
			lOk = (lStructure.GetBoneCount() == 12 &&
				lStructure.GetPhysicsType() == TBC::ChunkyStructure::DYNAMIC &&
				lStructure.GetBoneGeometry(0) != 0 &&
				lStructure.GetBoneGeometry(0)->GetBodyId() != TBC::INVALID_BODY &&
				lStructure.GetEngineCount() == 3);
			assert(lOk);
		}
		lStructure.ClearAll(lPhysics);
	}

	delete (lPhysics);

	ReportTestResult(gTbcLog, _T("ExportStructure"), lContext, lOk);
	return (lOk);
}
