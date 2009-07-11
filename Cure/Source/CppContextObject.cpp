
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/CppContextObject.h"
#include <assert.h>
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/ChunkyStructure.h"
#include "../../TBC/Include/StructureEngine.h"
#include "../Include/ContextManager.h"
#include "../Include/ContextObjectAttribute.h"
#include "../Include/GameManager.h"



namespace Cure
{



CppContextObject::CppContextObject(const Lepra::String& pClassId):
	ContextObject(pClassId)
{
}

CppContextObject::~CppContextObject()
{
}



void CppContextObject::OnTick(float pFrameTime)
{
	if (mStructure)
	{
		mStructure->OnTick(GetManager()->GetGameManager()->GetPhysicsManager(), pFrameTime);
	}
}

void CppContextObject::OnAlarm(int)// pAlarmId)
{
}

void CppContextObject::OnTrigger(TBC::PhysicsEngine::BodyID pBody1, TBC::PhysicsEngine::BodyID pBody2)
{
	ContextObject* lObject2 = (ContextObject*)mManager->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListener(pBody2);
	if (mManager->GetGameManager()->IsConnectAuthorized() && lObject2)
	{
		AttachToObject(pBody1, lObject2, pBody2);
	}
}



void CppContextObject::OnForceApplied(TBC::PhysicsEngine::ForceFeedbackListener* pOtherObject,
	const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque)
{
	if (!IsAttachedTo((ContextObject*)pOtherObject))
	{
		// TODO: replace by sensible values. Like dividing by mass, for instance.
		//if (pForce.GetLengthSquared() > 100 || pTorque.GetLengthSquared() > 10)
		{
			mManager->GetGameManager()->OnCollision(pForce, pTorque, this, (ContextObject*)pOtherObject);
		}
	}
}



CppContextObjectFactory::CppContextObjectFactory(unsigned pPhysicsFps):
	mPhysicsFps(pPhysicsFps)
{
}

CppContextObjectFactory::~CppContextObjectFactory()
{
}

ContextObject* CppContextObjectFactory::Create(const Lepra::String& pClassId) const
{
	return (new CppContextObject(pClassId));
}

bool CppContextObjectFactory::CreatePhysics(ContextObject* pObject) const
{
	// This hack will all go away as soon as disk physics loading is avail.

	// Pretend we're able to create these hard-coded objects.
	TBC::ChunkyStructure* lStructure = 0;
	Lepra::TransformationF lTransformation;
	static const float lRoadWidth = 5;
	static const float lRoadHeight = 7;
	static const float lUphillLength = 70;
	static const float lUphillOrthogonalLength = lUphillLength*::sin(Lepra::PIF/4);
	static const float lPlateauLength = 15;
	static const float lPlateauLengthCompensation = lRoadHeight/1.5f;
	static const float lFloorSize = 500;
	if (pObject->GetClassId().find(_T("box_002")) != Lepra::String::npos)
	{
		lStructure = new TBC::ChunkyStructure(TBC::ChunkyStructure::DYNAMIC);
		lStructure->SetBoneCount(1);

		Lepra::Vector3DF lDimensions(2.0f, 1.0f, 3.5f);
		lTransformation.SetPosition(Lepra::Vector3DF(lPlateauLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength*2+lFloorSize/2-lRoadHeight/2+10));
		lTransformation.GetOrientation().RotateAroundOwnX(0.1f);
		lTransformation.GetOrientation().RotateAroundWorldZ(0.7f);

		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(
			TBC::ChunkyBoneGeometry::BodyData(1.0f,  0.2f, 1.0f), lDimensions);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);
	}
	else if (pObject->GetClassId().find(_T("sphere_002")) != Lepra::String::npos)
	{
		lStructure = new TBC::ChunkyStructure(TBC::ChunkyStructure::DYNAMIC);
		lStructure->SetBoneCount(1);

		const float lRadius = 1.0f;
		const float lX = (float)Lepra::Random::Uniform(-240, 240);
		const float lY = (float)Lepra::Random::Uniform(-240, 240);
		lTransformation.SetPosition(Lepra::Vector3DF(lX, lY, 250+15));
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneSphere(
			TBC::ChunkyBoneGeometry::BodyData(1.0f, 0.2f, 1.0f), lRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		TBC::StructureEngine* lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_CAMERA_FLAT_PUSH,
			55, 50, 0, 0);
		lEngine->AddControlledGeometry(lGeometry, 1);
		lStructure->AddStructureEngine(lEngine);
	}
	else if (pObject->GetClassId().find(_T("car_001")) != Lepra::String::npos)
	{
		lStructure = new TBC::ChunkyStructure(TBC::ChunkyStructure::DYNAMIC);
		lStructure->SetBoneCount(6);

		const float lCarWeight = 1800;
		const float lWheelRadius = 0.3f;
		const float lWheelXOffset = -0.15f;
		const float lWheelZOffset = 0.3f;
		const float lBackWheelYOffset = -0.8f;
		const float lWheelYDistance = 3.6f;
		const Lepra::Vector3DF lBodyDimensions(1.9f, 4.9f, 0.6f);
		const Lepra::Vector3DF lTopDimensions(1.6f, 2.9f, 0.6f);
		const float lX = (float)Lepra::Random::Uniform(-200, 200);
		const float lY = (float)Lepra::Random::Uniform(-200, 200);
		lTransformation.SetPosition(Lepra::Vector3DF(lX, lY, 250+5));

		// Body.
		TBC::ChunkyBoneGeometry::BodyData lBodyData(lCarWeight, 0.1f, 1.0f, 0,
			TBC::ChunkyBoneGeometry::TYPE_EXCLUDE, TBC::ChunkyBoneGeometry::CONNECTEE_3DOF);
		TBC::ChunkyBoneGeometry* lBodyGeometry = new TBC::ChunkyBoneBox(lBodyData, lBodyDimensions);
		lStructure->AddBoneGeometry(lTransformation, lBodyGeometry);
		// Top of body.
		Lepra::TransformationF lTopTransform(lTransformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		lTopTransform.MoveForward(0.9f);
		lBodyData.mMass = 0.0f;
		lBodyData.mFriction = 0.5f;
		lBodyData.mParent = lBodyGeometry;
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(lBodyData, lTopDimensions);
		lStructure->AddBoneGeometry(lTopTransform, lGeometry);

		// Wheels and suspension.
		const float lFrameTime = 1/(float)mPhysicsFps;
		const float lSpringConstant = lCarWeight*50;
		const float lDamperConstant = lCarWeight/5;

		lTransformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF/2);
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_SUSPEND_HINGE;
		lBodyData.mConnectorType = TBC::ChunkyBoneGeometry::CONNECT_NONE;
		lBodyData.mMass = lCarWeight/50;
		lBodyData.mFriction = 1.0f;
		lBodyData.mBounce = 0.5f;
		lBodyData.mParameter[0] = lFrameTime;
		lBodyData.mParameter[1] = lSpringConstant;
		lBodyData.mParameter[2] = lDamperConstant;
		lBodyData.mParameter[3] = 0;
		lBodyData.mParameter[4] = Lepra::PIF * 0.5f;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[3] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_HINGE2;
		lBodyData.mParameter[3] = 0;
		lBodyData.mParameter[5] = -0.5f;
		lBodyData.mParameter[6] = 0.5f;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[3] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		// Front wheel drive engine.
		TBC::StructureEngine* lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_ROLL, 1*lCarWeight, 300, 20, 0);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(4), -1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(5), +1);
		lStructure->AddStructureEngine(lEngine);
		// Turning front wheels.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_TURN, 1*lCarWeight, 1.0f, 0, 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(4), +1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(5), +1);
		lStructure->AddStructureEngine(lEngine);
		// Normal breaks (all nodes, scaled >= 0) and handbreak (rear wheels, nodes with high scaling values).
		const float lBreakInputThreashold = 0.02f;
		TBC::StructureEngine* lBreak = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_BREAK, 8*lCarWeight, lBreakInputThreashold, 0, 2);
		lBreak->AddControlledGeometry(lStructure->GetBoneGeometry(2), 100);
		lBreak->AddControlledGeometry(lStructure->GetBoneGeometry(3), 100);
		lBreak->AddControlledGeometry(lStructure->GetBoneGeometry(4), 0);
		lBreak->AddControlledGeometry(lStructure->GetBoneGeometry(5), 0);
		lStructure->AddStructureEngine(lBreak);
		// Just a "dummy engine" to keep rear wheel straight at all times.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_ROLL_STRAIGHT, 0, 0, 0, 0);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(2), -1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(3), +1);
		lStructure->AddStructureEngine(lEngine);
	}
	else if (pObject->GetClassId().find(_T("monster_001")) != Lepra::String::npos)
	{
		lStructure = new TBC::ChunkyStructure(TBC::ChunkyStructure::DYNAMIC);
		lStructure->SetBoneCount(6);

		const float lCarWeight = 3000;
		const float lWheelRadius = 1.0f;
		const float lWheelXOffset = 0.5f;
		const float lWheelZOffset = 0.8f;
		const float lBackWheelYOffset = 0.0f;
		const float lWheelYDistance = 5.0f;
		const Lepra::Vector3DF lBodyDimensions(3.0f, 5.0f, 0.7f);
		const Lepra::Vector3DF lTopDimensions(3.0f, 3.0f, 0.7f);
		const float lX = (float)Lepra::Random::Uniform(-200, 200);
		const float lY = (float)Lepra::Random::Uniform(-200, 200);
		lTransformation.SetPosition(Lepra::Vector3DF(lX, lY, 250+15));

		// Body.
		TBC::ChunkyBoneGeometry::BodyData lBodyData(lCarWeight, 0.1f, 0.5f);
		TBC::ChunkyBoneGeometry* lBodyGeometry = new TBC::ChunkyBoneBox(lBodyData, lBodyDimensions);
		lStructure->AddBoneGeometry(lTransformation, lBodyGeometry);
		// Top of body.
		Lepra::TransformationF lTopTransform(lTransformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		lBodyData.mMass = 0.0f;
		lBodyData.mFriction = 0.8f;
		lBodyData.mParent = lBodyGeometry;
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(lBodyData, lTopDimensions);
		lStructure->AddBoneGeometry(lTopTransform, lGeometry);

		// Wheels and suspension.
		const float lFrameTime = 1/(float)mPhysicsFps;
		const float lSpringConstant = lCarWeight*5;
		const float lDamperConstant = lCarWeight/10;

		lTransformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF/2);
		lBodyData.mMass = lCarWeight/50;
		lBodyData.mFriction = 1.0f;
		lBodyData.mBounce = 0.5f;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_SUSPEND_HINGE;
		lBodyData.mConnectorType = TBC::ChunkyBoneGeometry::CONNECT_NONE;
		lBodyData.mParameter[0] = lFrameTime;
		lBodyData.mParameter[1] = lSpringConstant;
		lBodyData.mParameter[2] = lDamperConstant;
		lBodyData.mParameter[3] = 0;
		lBodyData.mParameter[4] = Lepra::PIF * 0.5f;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[3] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lBodyData.mParameter[3] = 0;
		lBodyData.mParameter[5] = -0.5f;
		lBodyData.mParameter[6] = 0.5f;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[3] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		// Rear wheel drive engine.
		TBC::StructureEngine* lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_ROLL, 5*lCarWeight, 300, 20, 0);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(2), -1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(3), 1);
		lStructure->AddStructureEngine(lEngine);
		// Turning front wheels.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_TURN, 1*lCarWeight, 0.5f, 0, 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(4), 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(5), 1);
		lStructure->AddStructureEngine(lEngine);
		// Normal breaks (all nodes, scaled >= 0) and handbreak (rear wheels, nodes with high scaling values).
		const float lBreakInputThreashold = 0.02f;
		TBC::StructureEngine* lBreak = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_BREAK, 8*lCarWeight, lBreakInputThreashold, 0, 2);
		lBreak->AddControlledGeometry(lStructure->GetBoneGeometry(2), 100);
		lBreak->AddControlledGeometry(lStructure->GetBoneGeometry(3), 100);
		lBreak->AddControlledGeometry(lStructure->GetBoneGeometry(4), 0);
		lBreak->AddControlledGeometry(lStructure->GetBoneGeometry(5), 0);
		lStructure->AddStructureEngine(lBreak);
		// Just a "dummy engine" to keep rear wheel straight at all times.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_ROLL_STRAIGHT, 0, 0, 0, 0);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(2), -1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(3), 1);
		lStructure->AddStructureEngine(lEngine);
	}
	else if (pObject->GetClassId().find(_T("excavator_703")) != Lepra::String::npos)
	{
		lStructure = new TBC::ChunkyStructure(TBC::ChunkyStructure::DYNAMIC);
		lStructure->SetBoneCount(13);

		const float lCarWeight = 18000;
		const float lWheelRadius = 0.5f;
		const float lWheelXOffset = 0.2f;
		const float lWheelZOffset = 0.6f;
		const float lBackWheelYOffset = 0.5f;
		const float lWheelYDistance = 2.0f;
		const Lepra::Vector3DF lBodyDimensions(2.9f, 3.0f, 1.5f);
		const Lepra::Vector3DF lTopDimensions(1.0f, 1.5f, 0.6f);
		const float lX = (float)Lepra::Random::Uniform(-200, 200);
		const float lY = (float)Lepra::Random::Uniform(-200, 200);
		lTransformation.SetPosition(Lepra::Vector3DF(lX, lY, 250+1));

		// Body.
		const Lepra::TransformationF lBodyTransformation(lTransformation);
		TBC::ChunkyBoneGeometry::BodyData lBodyData(lCarWeight, 0.5f, 0.5f);
		TBC::ChunkyBoneGeometry* lBodyGeometry = new TBC::ChunkyBoneBox(lBodyData, lBodyDimensions);
		lStructure->AddBoneGeometry(lTransformation, lBodyGeometry);
		// Top of body.
		Lepra::TransformationF lTopTransform(lTransformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		lTopTransform.MoveRight(1.0f);
		lTopTransform.MoveBackward(0.75f);
		lBodyData.mMass = 0.0f;
		lBodyData.mBounce = 1.0f;
		lBodyData.mParent = lBodyGeometry;
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(lBodyData, lTopDimensions);
		lStructure->AddBoneGeometry(lTopTransform, lGeometry);

		// Wheels and suspension.
		const float lDamperConstant = lCarWeight/5;
		const float lSpringConstant = lCarWeight*100;
		const float lFrameTime = 1/(float)mPhysicsFps;

		lTransformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF/2);
		lBodyData.mMass = lCarWeight/50;
		lBodyData.mFriction = 4.0f;
		lBodyData.mBounce = 0.2f;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_SUSPEND_HINGE;
		lBodyData.mConnectorType = TBC::ChunkyBoneGeometry::CONNECT_NONE;
		lBodyData.mParameter[0] = lFrameTime;
		lBodyData.mParameter[1] = lSpringConstant;
		lBodyData.mParameter[2] = lDamperConstant;
		lBodyData.mParameter[3] = 0;
		lBodyData.mParameter[4] = Lepra::PIF * 0.5f;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[3] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lBodyData.mParameter[3] = 0;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[3] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lBodyData.mParameter[3] = 0;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lBodyData.mParameter[3] = Lepra::PIF;
		lGeometry = new TBC::ChunkyBoneSphere(lBodyData, lWheelRadius);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		// Boom.
		const Lepra::Vector3DF lBoom1Dimensions(0.6f, 0.6f, 2.5f);
		const Lepra::Vector3DF lBoom2Dimensions(0.6f, 0.6f, 3.2f);
		lTransformation = lBodyTransformation;
		lTransformation.MoveUp(lBodyDimensions.z/2);
		lTransformation.MoveLeft(0.5f);
		lTransformation.MoveBackward(0.75f);
		const Lepra::TransformationF lBoom1AnchorTransform(lTransformation);
		lTransformation.MoveUp(lBoom1Dimensions.z/2);
		lBodyData.mMass = lCarWeight/12;
		lBodyData.mFriction = 1.0f;
		lBodyData.mBounce = 0.2f;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_HINGE;
		lBodyData.mParameter[3] = 0;
		lBodyData.mParameter[4] = Lepra::PIF * 0.5f;
		lBodyData.mParameter[5] = -1.2f;
		lBodyData.mParameter[6] = 0.5f;
		lBodyData.mParameter[7] = lBoom1AnchorTransform.GetPosition().x;
		lBodyData.mParameter[8] = lBoom1AnchorTransform.GetPosition().y;
		lBodyData.mParameter[9] = lBoom1AnchorTransform.GetPosition().z;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lBoom1Dimensions);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);
		// Boom, part 2.
		lTransformation.MoveUp(lBoom1Dimensions.z/2);
		const float lBoom2Angle = Lepra::PIF/4;
		lTransformation.MoveBackward(::sin(lBoom2Angle)*lBoom2Dimensions.z/2);
		lTransformation.MoveUp(::cos(lBoom2Angle)*lBoom2Dimensions.z/2);
		Lepra::TransformationF lBoom2Transform(lTransformation);
		lBoom2Transform.RotatePitch(lBoom2Angle);
		lBodyData.mMass = 0.0f;
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_EXCLUDE;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lBoom2Dimensions);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		// Arm.
		lTransformation.MoveBackward(::sin(lBoom2Angle)*lBoom2Dimensions.z/2);
		lTransformation.MoveUp(::cos(lBoom2Angle)*lBoom2Dimensions.z/2);
		const Lepra::TransformationF lArmAnchorTransform(lTransformation);
		const Lepra::Vector3DF lArmDimensions(0.4f, 3.0f, 0.4f);
		lTransformation.MoveBackward(lArmDimensions.y/2);
		lBodyData.mMass = lCarWeight/25;
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_HINGE;
		lBodyData.mParameter[5] = -1.0f;
		lBodyData.mParameter[6] = 0.5f;
		lBodyData.mParameter[7] = lArmAnchorTransform.GetPosition().x;
		lBodyData.mParameter[8] = lArmAnchorTransform.GetPosition().y;
		lBodyData.mParameter[9] = lArmAnchorTransform.GetPosition().z;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lArmDimensions);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		// Bucket.
		lTransformation.MoveBackward(lArmDimensions.y/2);
		const Lepra::TransformationF lBucketAnchorTransform(lTransformation);
		const Lepra::Vector3DF lBucketBackDimensions(1.5f, 0.8f, 0.1f);
		const float lBucketBackAngle = Lepra::PIF/4;
		lTransformation.MoveBackward(::cos(lBucketBackAngle)*lBucketBackDimensions.y/2);
		lTransformation.MoveUp(::sin(lBucketBackAngle)*lBucketBackDimensions.y/2);
		Lepra::TransformationF lBucketBackTransform(lTransformation);
		lBucketBackTransform.RotatePitch(-lBucketBackAngle);
		lBodyData.mMass = lCarWeight/50;
		lBodyData.mFriction = 10.0f;
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_HINGE;
		lBodyData.mParameter[5] = -1.0f;
		lBodyData.mParameter[6] = 0.5f;
		lBodyData.mParameter[7] = lBucketAnchorTransform.GetPosition().x;
		lBodyData.mParameter[8] = lBucketAnchorTransform.GetPosition().y;
		lBodyData.mParameter[9] = lBucketAnchorTransform.GetPosition().z;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lBucketBackDimensions);
		lStructure->AddBoneGeometry(lBucketBackTransform, lGeometry);
		// Bucket, floor part.
		lTransformation.MoveBackward(::cos(lBucketBackAngle)*lBucketBackDimensions.y/2);
		lTransformation.MoveUp(::sin(lBucketBackAngle)*lBucketBackDimensions.y/2);
		const Lepra::Vector3DF lBucketFloorDimensions(1.5f, 1.0f, 0.1f);
		const float lBucketFloorAngle = Lepra::PIF/4;
		lTransformation.MoveBackward(::cos(lBucketFloorAngle)*lBucketFloorDimensions.y/2);
		lTransformation.MoveDown(::sin(lBucketFloorAngle)*lBucketFloorDimensions.y/2);
		Lepra::TransformationF lBucketFloorTransform(lTransformation);
		lBucketFloorTransform.RotatePitch(lBucketFloorAngle);
		lBodyData.mMass = 0.0f;
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_EXCLUDE;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lBucketBackDimensions);
		lStructure->AddBoneGeometry(lBucketFloorTransform, lGeometry);

		// All wheel drive engine.
		TBC::StructureEngine* lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_ROLL, 0.5f*lCarWeight, 25, 25, 0);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(2), -1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(3), 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(4), -1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(5), 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(6), -1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(7), 1);
		lStructure->AddStructureEngine(lEngine);
		// Turning with tracks are controlled by rolling of the wheels.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE2_ROLL, lCarWeight, 20, 20, 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(2), 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(3), 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(4), 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(5), 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(6), 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(7), 1);
		lStructure->AddStructureEngine(lEngine);
		// The boom, arm and bucket are hinge-controlled.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE, 20*lCarWeight, 2.0f, 1.0f, 3);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(8), 5);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(10), 2, TBC::StructureEngine::MODE_HALF_LOCK);	// MODE_HALF_LOCK = has neutral/freeze position outside mid value.
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(11), 1, TBC::StructureEngine::MODE_HALF_LOCK);	// MODE_HALF_LOCK = has neutral/freeze position outside mid value.
		lStructure->AddStructureEngine(lEngine);
		// Just a "dummy engine" to keep wheels straight at all times.
		lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_ROLL_STRAIGHT, 0, 0, 0, 0);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(2), -1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(3), 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(4), -1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(5), 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(6), -1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(7), 1);
		lStructure->AddStructureEngine(lEngine);
	}
	else if (pObject->GetClassId().find(_T("crane_whatever")) != Lepra::String::npos)
	{
		lStructure = new TBC::ChunkyStructure(TBC::ChunkyStructure::DYNAMIC);
		lStructure->SetBoneCount(7);

		const float lTowerWeight = 50000;
		const float lTowerHeight = 25.0f;
		const float lThickness = 2.5f;
		const Lepra::Vector3DF lTowerDimensions(lThickness, lThickness, lTowerHeight);
		const float lX = (float)Lepra::Random::Uniform(-200, 200);
		const float lY = (float)Lepra::Random::Uniform(-200, 200);
		lTransformation.SetPosition(Lepra::Vector3DF(lX, lY, 250+lTowerHeight/2));

		// Body.
		TBC::ChunkyBoneGeometry::BodyData lBodyData(lTowerWeight, 1.0f, 0.5f);
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(lBodyData, lTowerDimensions);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

		// Jib.
		const float lJibWeight = lTowerWeight/3.5f;
		const float lJibPlacementHeight = 20.0f;
		const float lJibLength = 15.0f;
		const float lCounterJibLength = 5.0f;
		const float lTotalJibLength = lJibLength+lCounterJibLength;

		Lepra::TransformationF lJibTransform(lTransformation);
		lJibTransform.GetPosition().Add(0, 0, -lTowerHeight/2 + lJibPlacementHeight);
		const Lepra::Vector3DF lJibAnchorPosition(lJibTransform.GetPosition());
		lJibTransform.GetPosition().Add(0, lTotalJibLength/2-lCounterJibLength, 0);
		const Lepra::Vector3DF lJibDimensions(lThickness, lTotalJibLength, lThickness);
		lBodyData.mMass = lJibWeight;
		lBodyData.mParent = lGeometry;
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_HINGE;
		lBodyData.mParameter[5] = -1e10f;
		lBodyData.mParameter[6] = 1e10f;
		lBodyData.mParameter[7] = lJibAnchorPosition.x;
		lBodyData.mParameter[8] = lJibAnchorPosition.y;
		lBodyData.mParameter[9] = lJibAnchorPosition.z;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lJibDimensions);
		lStructure->AddBoneGeometry(lJibTransform, lGeometry);

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
			lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_UNIVERSAL;
			lBodyData.mParameter[4] = Lepra::PIF * 0.5f;
			lBodyData.mParameter[7] = lWireAnchor.x;
			lBodyData.mParameter[8] = lWireAnchor.y;
			lBodyData.mParameter[9] = lWireAnchor.z;
			lGeometry = new TBC::ChunkyBoneCapsule(lBodyData, lWireThickness/2, lWireLength);
			lStructure->AddBoneGeometry(lWireTransform, lGeometry);

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
		lBodyData.mJointType = TBC::ChunkyBoneGeometry::TYPE_UNIVERSAL;
		lBodyData.mParameter[4] = Lepra::PIF * 0.5f;
		lBodyData.mParameter[7] = lHookAnchor.x;
		lBodyData.mParameter[8] = lHookAnchor.y;
		lBodyData.mParameter[9] = lHookAnchor.z;
		lBodyData.mConnectorType = TBC::ChunkyBoneGeometry::CONNECTOR_3DOF;
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, Lepra::Vector3DF(lHookWidth, lHookWidth, lHookHeight));
		lStructure->AddBoneGeometry(lHookTransform, lGeometry);

		// Jib rotational engine.
		TBC::StructureEngine* lEngine = new TBC::StructureEngine(TBC::StructureEngine::ENGINE_HINGE, lJibWeight, 2.0f, 1.0f, 1);
		lEngine->AddControlledGeometry(lStructure->GetBoneGeometry(1), 1.0f);
		lStructure->AddStructureEngine(lEngine);
	}
	else if (pObject->GetClassId().find(_T("ground_002")) != Lepra::String::npos)
	{
		lStructure = new TBC::ChunkyStructure(TBC::ChunkyStructure::STATIC);
		lStructure->SetBoneCount(6);
		TBC::ChunkyBoneGeometry::BodyData lBodyData(0.0f, 1.0f, 0.6f);

		Lepra::Vector3DF lDimensions(lFloorSize, lFloorSize, lFloorSize);
		TBC::ChunkyBoneGeometry* lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);

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
		lStructure->AddBoneGeometry(lTransformation, lGeometry);
		// Upper climb.
		lDimensions.Set(lUphillLength, lRoadWidth, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(lPlateauLength+lUphillOrthogonalLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength*3/2+lFloorSize/2-lRoadHeight/2/1.5f));
		lRotation = Lepra::RotationMatrixF();
		lRotation.RotateAroundOwnY(Lepra::PIF/4);
		lTransformation.SetOrientation(lRotation);
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);
		// First plateau.
		lDimensions.Set(lRoadWidth, lPlateauLength+lPlateauLengthCompensation, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength+lRoadWidth/2,
			lUphillOrthogonalLength/2+lPlateauLength/2,
			lUphillOrthogonalLength+lFloorSize/2-lRoadHeight/2));
		lTransformation.SetOrientation(Lepra::RotationMatrixF());
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);
		// Second plateau.
		lDimensions.Set(lPlateauLength, lRoadWidth, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength+lFloorSize/2-lRoadHeight/2));
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);
		// Top plateau.
		lDimensions.Set(lPlateauLength+lPlateauLengthCompensation, lRoadWidth, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(lPlateauLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength*2+lFloorSize/2-lRoadHeight/2));
		lGeometry = new TBC::ChunkyBoneBox(lBodyData, lDimensions);
		lStructure->AddBoneGeometry(lTransformation, lGeometry);
	}
	else
	{
		mLog.Error(_T("Unknown context object type."));
	}

	bool lOk = (lStructure != 0);
	if (lOk)
	{
		lOk = pObject->SetStructure(lStructure);
		if (lOk)
		{
			pObject->GetManager()->EnablePhysicsUpdateCallback(pObject);
		}
	}

	return (lOk);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObject);
LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObjectFactory);



}
