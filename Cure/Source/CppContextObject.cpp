
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include <assert.h>
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../Include/CppContextObject.h"
#include "../Include/ContextManager.h"
#include "../Include/ContextObjectEngine.h"
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
	AttributeArray::const_iterator z = mAttributeArray.begin();
	for (; z != mAttributeArray.end(); ++z)
	{
		// TODO: add support for parent ID.
		ContextObjectAttribute* lAttribute = *z;
		if (lAttribute->GetType() == ContextObjectAttribute::TYPE_ENGINE)
		{
			ContextObjectEngine* lEngine = (ContextObjectEngine*)lAttribute;
			lEngine->OnTick(pFrameTime);
		}
	}
}

void CppContextObject::OnAlarm(int pAlarmId)
{
	pAlarmId;
}

void CppContextObject::OnTrigger(TBC::PhysicsEngine::BodyID pBody1, TBC::PhysicsEngine::BodyID pBody2)
{
	ContextObject* lObject2 = (ContextObject*)mManager->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListener(pBody2);
	if (mManager->GetGameManager()->IsConnectAuthorized() && lObject2)
	{
		ConnectObjects(pBody1, lObject2, pBody2);
	}
}



void CppContextObject::OnForceApplied(TBC::PhysicsEngine::ForceFeedbackListener* pOtherObject,
	const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque)
{
	if (!IsConnectedTo((ContextObject*)pOtherObject))
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

bool CppContextObjectFactory::CreatePhysics(ContextObject* pObject, ContextObject* pTriggerListener) const
{
	// This hack will all go away as soon as disk physics loading is avail.

	TBC::PhysicsEngine* lPhysicsManager = pObject->GetManager()->GetGameManager()->GetPhysicsManager();
	// Pretend we're able to create these hard-coded objects.
	bool lOk = false;
	TBC::PhysicsEngine::BodyID lPhysicsObjectId = TBC::INVALID_BODY;
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
		Lepra::Vector3DF lDimensions(2.0f, 1.0f, 3.5f);
		lTransformation.SetPosition(Lepra::Vector3DF(lPlateauLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength*2+lFloorSize/2-lRoadHeight/2+10));
		lTransformation.GetOrientation().RotateAroundOwnX(0.1f);
		lTransformation.GetOrientation().RotateAroundWorldZ(0.7f);
		lPhysicsObjectId = lPhysicsManager->CreateBox(lTransformation, 1, lDimensions,
			TBC::PhysicsEngine::DYNAMIC, 0.5f, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		pObject->AddPhysicsObject(PhysicsNode(0, 1, lPhysicsObjectId));

		pObject->GetManager()->EnablePhysicsUpdateCallback(pObject);

		lOk = true;
	}
	else if (pObject->GetClassId().find(_T("sphere_002")) != Lepra::String::npos)
	{
		const float lRadius = 1.0f;
		const float lX = (float)Lepra::Random::Uniform(-240, 240);
		const float lY = (float)Lepra::Random::Uniform(-240, 240);
		lTransformation.SetPosition(Lepra::Vector3DF(lX, lY, 250+15));
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, 2, lRadius,
			TBC::PhysicsEngine::DYNAMIC, 0.5f, 1.0f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		pObject->AddPhysicsObject(PhysicsNode(0, 1, lPhysicsObjectId));

		ContextObjectEngine* lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_CAMERA_FLAT_PUSH, 50, 0, 50, 0);
		lEngine->AddControlledNode(1, 1);
		pObject->AddAttribute(lEngine);

		pObject->GetManager()->EnablePhysicsUpdateCallback(pObject);

		lOk = true;
	}
	else if (pObject->GetClassId().find(_T("car_001")) != Lepra::String::npos)
	{
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
		TBC::PhysicsEngine::BodyID lBodyId = lPhysicsManager->CreateBox(lTransformation, lCarWeight, lBodyDimensions,
			TBC::PhysicsEngine::DYNAMIC, 0.1f, 1.0f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lBodyId);
		pObject->AddPhysicsObject(PhysicsNode(0, 1, lBodyId, PhysicsNode::TYPE_EXCLUDE, TBC::INVALID_JOINT,
			PhysicsNode::CONNECTEE_3));
		Lepra::TransformationF lTopTransform(lTransformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		lTopTransform.MoveForward(0.9f);
		TBC::PhysicsEngine::BodyID lTopId = lPhysicsManager->CreateBox(lTopTransform, 0, lTopDimensions,
			TBC::PhysicsEngine::STATIC, 0.5f, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->Attach(lTopId, lBodyId);
		pObject->AddPhysicsObject(PhysicsNode(1, 2, lTopId, PhysicsNode::TYPE_EXCLUDE, TBC::INVALID_JOINT,
			PhysicsNode::CONNECTEE_3));

		// Wheels and suspension.
		const float lDamperConstant = lCarWeight/20;
		const float lSpringConstant = lCarWeight*20;
		const float lFrameTime = 1/(float)mPhysicsFps;
		TBC::PhysicsEngine::JointID lJoint;

		lTransformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF/2);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 1, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, 0, 0, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 3, lPhysicsObjectId, PhysicsNode::TYPE_SUSPEND_HINGE, lJoint));

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 1, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(-1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, 0, 0, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 4, lPhysicsObjectId, PhysicsNode::TYPE_SUSPEND_HINGE, lJoint));

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 1, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, -0.5f, 0.5f, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 5, lPhysicsObjectId, PhysicsNode::TYPE_HINGE2, lJoint));

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 1, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(-1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, -0.5f, 0.5f, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 6, lPhysicsObjectId, PhysicsNode::TYPE_HINGE2, lJoint));

		// Front wheel drive engine.
		ContextObjectEngine* lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_HINGE2_ROLL, 1*lCarWeight, 300, 20, 0);
		lEngine->AddControlledNode(5, -1);
		lEngine->AddControlledNode(6, 1);
		pObject->AddAttribute(lEngine);
		// Turning front wheels.
		lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_HINGE2_TURN, 1*lCarWeight, 1.0f, 0, 1);
		lEngine->AddControlledNode(5, 1);
		lEngine->AddControlledNode(6, 1);
		pObject->AddAttribute(lEngine);
		// Normal breaks (all nodes, scaled >= 0) and handbreak (rear wheels, nodes with high scaling values).
		const float lBreakInputThreashold = 0.02f;
		ContextObjectEngine* lBreak = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_HINGE2_BREAK, 8*lCarWeight, lBreakInputThreashold, 0, 2);
		lBreak->AddControlledNode(3, 100);
		lBreak->AddControlledNode(4, 100);
		lBreak->AddControlledNode(5, 0);
		lBreak->AddControlledNode(6, 0);
		pObject->AddAttribute(lBreak);
		// Just a "dummy engine" to keep rear wheel straight at all times.
		lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_ROLL_STRAIGHT, 0, 0, 0, 0);
		lEngine->AddControlledNode(3, -1);
		lEngine->AddControlledNode(4, 1);
		pObject->AddAttribute(lEngine);

		pObject->GetManager()->EnablePhysicsUpdateCallback(pObject);

		lOk = true;
	}
	else if (pObject->GetClassId().find(_T("monster_001")) != Lepra::String::npos)
	{
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
		TBC::PhysicsEngine::BodyID lBodyId = lPhysicsManager->CreateBox(lTransformation, lCarWeight, lBodyDimensions,
			TBC::PhysicsEngine::DYNAMIC, 0.5f, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lBodyId);
		pObject->AddPhysicsObject(PhysicsNode(0, 1, lBodyId));
		Lepra::TransformationF lTopTransform(lTransformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		TBC::PhysicsEngine::BodyID lTopId = lPhysicsManager->CreateBox(lTopTransform, 0, lTopDimensions,
			TBC::PhysicsEngine::STATIC, 0.5f, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->Attach(lTopId, lBodyId);
		pObject->AddPhysicsObject(PhysicsNode(1, 2, lTopId, PhysicsNode::TYPE_EXCLUDE));

		// Wheels and suspension.
		const float lDamperConstant = lCarWeight/10;
		const float lSpringConstant = lCarWeight*5;
		const float lFrameTime = 1/(float)mPhysicsFps;
		TBC::PhysicsEngine::JointID lJoint;

		lTransformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF/2);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 1, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, 0, 0, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 3, lPhysicsObjectId, PhysicsNode::TYPE_SUSPEND_HINGE, lJoint));

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 1, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(-1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, 0, 0, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 4, lPhysicsObjectId, PhysicsNode::TYPE_SUSPEND_HINGE, lJoint));

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 1, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, -0.5f, 0.5f, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 5, lPhysicsObjectId, PhysicsNode::TYPE_HINGE2, lJoint));

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 1, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(-1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, -0.5f, 0.5f, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 6, lPhysicsObjectId, PhysicsNode::TYPE_HINGE2, lJoint));

		// Rear wheel drive engine.
		ContextObjectEngine* lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_HINGE2_ROLL, 2*lCarWeight, 300, 20, 0);
		lEngine->AddControlledNode(3, -1);
		lEngine->AddControlledNode(4, 1);
		pObject->AddAttribute(lEngine);
		// Turning front wheels.
		lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_HINGE2_TURN, 1*lCarWeight, 0.5f, 0, 1);
		lEngine->AddControlledNode(5, 1);
		lEngine->AddControlledNode(6, 1);
		pObject->AddAttribute(lEngine);
		// Normal breaks (all nodes, scaled >= 0) and handbreak (rear wheels, nodes with high scaling values).
		const float lBreakInputThreashold = 0.02f;
		ContextObjectEngine* lBreak = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_HINGE2_BREAK, 8*lCarWeight, lBreakInputThreashold, 0, 2);
		lBreak->AddControlledNode(3, 100);
		lBreak->AddControlledNode(4, 100);
		lBreak->AddControlledNode(5, 0);
		lBreak->AddControlledNode(6, 0);
		pObject->AddAttribute(lBreak);
		// Just a "dummy engine" to keep rear wheel straight at all times.
		lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_ROLL_STRAIGHT, 0, 0, 0, 0);
		lEngine->AddControlledNode(3, -1);
		lEngine->AddControlledNode(4, 1);
		pObject->AddAttribute(lEngine);

		pObject->GetManager()->EnablePhysicsUpdateCallback(pObject);

		lOk = true;
	}
	else if (pObject->GetClassId().find(_T("excavator_703")) != Lepra::String::npos)
	{
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
		TBC::PhysicsEngine::BodyID lBodyId = lPhysicsManager->CreateBox(lTransformation, lCarWeight, lBodyDimensions,
			TBC::PhysicsEngine::DYNAMIC, 0.5f, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lBodyId);
		pObject->AddPhysicsObject(PhysicsNode(0, 1, lBodyId));
		Lepra::TransformationF lTopTransform(lTransformation);
		lTopTransform.MoveUp(lBodyDimensions.z/2+lTopDimensions.z/2);
		lTopTransform.MoveRight(1.0f);
		lTopTransform.MoveBackward(0.75f);
		TBC::PhysicsEngine::BodyID lTopId = lPhysicsManager->CreateBox(lTopTransform, 0, lTopDimensions,
			TBC::PhysicsEngine::STATIC, 0.5f, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->Attach(lTopId, lBodyId);
		pObject->AddPhysicsObject(PhysicsNode(1, 2, lTopId, PhysicsNode::TYPE_EXCLUDE));

		// Wheels and suspension.
		const float lDamperConstant = lCarWeight/5;
		const float lSpringConstant = lCarWeight*100;
		const float lFrameTime = 1/(float)mPhysicsFps;
		TBC::PhysicsEngine::JointID lJoint;

		lTransformation.GetPosition().Add(-lBodyDimensions.x/2-lWheelXOffset,
			lBodyDimensions.y/2+lBackWheelYOffset, -lWheelZOffset);
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF/2);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 4, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, 0, 0, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 3, lPhysicsObjectId, PhysicsNode::TYPE_SUSPEND_HINGE, lJoint));

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 4, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(-1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, 0, 0, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 4, lPhysicsObjectId, PhysicsNode::TYPE_SUSPEND_HINGE, lJoint));

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 4, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, 0, 0, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 5, lPhysicsObjectId, PhysicsNode::TYPE_SUSPEND_HINGE, lJoint));

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 4, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(-1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, 0, 0, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 6, lPhysicsObjectId, PhysicsNode::TYPE_SUSPEND_HINGE, lJoint));

		lTransformation.GetPosition().x -= lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetPosition().y -= lWheelYDistance;
		lTransformation.GetOrientation().RotateAroundOwnZ(Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 4, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, 0, 0, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 7, lPhysicsObjectId, PhysicsNode::TYPE_SUSPEND_HINGE, lJoint));

		lTransformation.GetPosition().x += lBodyDimensions.x+lWheelXOffset*2;
		lTransformation.GetOrientation().RotateAroundOwnZ(-Lepra::PIF);
		lPhysicsObjectId = lPhysicsManager->CreateSphere(lTransformation, lCarWeight/50, lWheelRadius,
			TBC::PhysicsEngine::DYNAMIC, 4, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		lJoint = lPhysicsManager->CreateHinge2Joint(lBodyId, lPhysicsObjectId,
			lTransformation.GetPosition(), Lepra::Vector3DF(0, 0, 1), Lepra::Vector3DF(-1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, 0, 0, 0);
		lPhysicsManager->SetSuspension(lJoint, lFrameTime, lSpringConstant, lDamperConstant);
		lPhysicsManager->SetAngularMotorRoll(lJoint, 0, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 8, lPhysicsObjectId, PhysicsNode::TYPE_SUSPEND_HINGE, lJoint));

		// Boom.
		const Lepra::Vector3DF lBoom1Dimensions(0.6f, 0.6f, 2.5f);
		const Lepra::Vector3DF lBoom2Dimensions(0.6f, 0.6f, 3.2f);
		lTransformation = lBodyTransformation;
		lTransformation.MoveUp(lBodyDimensions.z/2);
		lTransformation.MoveLeft(0.5f);
		lTransformation.MoveBackward(0.75f);
		const Lepra::TransformationF lBoom1AnchorTransform(lTransformation);
		lTransformation.MoveUp(lBoom1Dimensions.z/2);
		TBC::PhysicsEngine::BodyID lBoomId = lPhysicsManager->CreateBox(lTransformation, lCarWeight/12,
			lBoom1Dimensions, TBC::PhysicsEngine::DYNAMIC, 1, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lBoomId);
		lJoint = lPhysicsManager->CreateHingeJoint(lBodyId, lBoomId,
			lBoom1AnchorTransform.GetPosition(), Lepra::Vector3DF(1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, -1.2f, 0.5f, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 20, lBoomId, PhysicsNode::TYPE_HINGE, lJoint));
		// Boom, part 2.
		lTransformation.MoveUp(lBoom1Dimensions.z/2);
		const float lBoom2Angle = Lepra::PIF/4;
		lTransformation.MoveBackward(::sin(lBoom2Angle)*lBoom2Dimensions.z/2);
		lTransformation.MoveUp(::cos(lBoom2Angle)*lBoom2Dimensions.z/2);
		Lepra::TransformationF lBoom2Transform(lTransformation);
		lBoom2Transform.RotatePitch(lBoom2Angle);
		lBodyId = lPhysicsManager->CreateBox(lBoom2Transform, 0, lBoom2Dimensions,
			TBC::PhysicsEngine::STATIC, 1, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->Attach(lBodyId, lBoomId);
		pObject->AddPhysicsObject(PhysicsNode(20, 21, lBodyId, PhysicsNode::TYPE_EXCLUDE));

		// Arm.
		lTransformation.MoveBackward(::sin(lBoom2Angle)*lBoom2Dimensions.z/2);
		lTransformation.MoveUp(::cos(lBoom2Angle)*lBoom2Dimensions.z/2);
		const Lepra::TransformationF lArmAnchorTransform(lTransformation);
		const Lepra::Vector3DF lArmDimensions(0.4f, 3.0f, 0.4f);
		lTransformation.MoveBackward(lArmDimensions.y/2);
		TBC::PhysicsEngine::BodyID lArmId = lPhysicsManager->CreateBox(lTransformation, lCarWeight/25, lArmDimensions,
			TBC::PhysicsEngine::DYNAMIC, 1, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lArmId);
		lJoint = lPhysicsManager->CreateHingeJoint(lBoomId, lArmId,
			lArmAnchorTransform.GetPosition(), Lepra::Vector3DF(1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, -1.0f, 0.5f, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(20, 22, lArmId, PhysicsNode::TYPE_HINGE, lJoint));

		// Bucket.
		lTransformation.MoveBackward(lArmDimensions.y/2);
		const Lepra::TransformationF lBucketAnchorTransform(lTransformation);
		const Lepra::Vector3DF lBucketBackDimensions(1.5f, 0.8f, 0.1f);
		const float lBucketBackAngle = Lepra::PIF/4;
		lTransformation.MoveBackward(::cos(lBucketBackAngle)*lBucketBackDimensions.y/2);
		lTransformation.MoveUp(::sin(lBucketBackAngle)*lBucketBackDimensions.y/2);
		Lepra::TransformationF lBucketBackTransform(lTransformation);
		lBucketBackTransform.RotatePitch(-lBucketBackAngle);
		TBC::PhysicsEngine::BodyID lBucketBackId = lPhysicsManager->CreateBox(lBucketBackTransform, lCarWeight/50, lBucketBackDimensions,
			TBC::PhysicsEngine::DYNAMIC, 0.1f, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lBucketBackId);
		lJoint = lPhysicsManager->CreateHingeJoint(lArmId, lBucketBackId,
			lBucketAnchorTransform.GetPosition(), Lepra::Vector3DF(1, 0, 0));
		lPhysicsManager->SetJointParams(lJoint, -1.0f, 0.5f, 0);
		lPhysicsManager->SetAngularMotorTurn(lJoint, 0, 0);
		pObject->AddPhysicsObject(PhysicsNode(22, 23, lBucketBackId, PhysicsNode::TYPE_HINGE, lJoint));
		// Bucket, floor part.
		lTransformation.MoveBackward(::cos(lBucketBackAngle)*lBucketBackDimensions.y/2);
		lTransformation.MoveUp(::sin(lBucketBackAngle)*lBucketBackDimensions.y/2);
		const Lepra::Vector3DF lBucketFloorDimensions(1.5f, 1.0f, 0.1f);
		const float lBucketFloorAngle = Lepra::PIF/4;
		lTransformation.MoveBackward(::cos(lBucketFloorAngle)*lBucketFloorDimensions.y/2);
		lTransformation.MoveDown(::sin(lBucketFloorAngle)*lBucketFloorDimensions.y/2);
		Lepra::TransformationF lBucketFloorTransform(lTransformation);
		lBucketFloorTransform.RotatePitch(lBucketFloorAngle);
		lPhysicsObjectId = lPhysicsManager->CreateBox(lBucketFloorTransform, 0, lBucketFloorDimensions,
			TBC::PhysicsEngine::STATIC, 0.1f, 0.2f, pTriggerListener, pObject);
		lPhysicsManager->Attach(lPhysicsObjectId, lBucketBackId);
		pObject->AddPhysicsObject(PhysicsNode(23, 24, lPhysicsObjectId, PhysicsNode::TYPE_EXCLUDE));

		// All wheel drive engine.
		ContextObjectEngine* lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_HINGE2_ROLL, 0.5f*lCarWeight, 20, 20, 0);
		lEngine->AddControlledNode(3, -1);
		lEngine->AddControlledNode(4, 1);
		lEngine->AddControlledNode(5, -1);
		lEngine->AddControlledNode(6, 1);
		lEngine->AddControlledNode(7, -1);
		lEngine->AddControlledNode(8, 1);
		pObject->AddAttribute(lEngine);
		// Turning with tracks are controlled by rolling of the wheels.
		lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_HINGE2_ROLL, 0.5f*lCarWeight, 10, 10, 1);
		lEngine->AddControlledNode(3, 1);
		lEngine->AddControlledNode(4, 1);
		lEngine->AddControlledNode(5, 1);
		lEngine->AddControlledNode(6, 1);
		lEngine->AddControlledNode(7, 1);
		lEngine->AddControlledNode(8, 1);
		pObject->AddAttribute(lEngine);
		// The boom, arm and bucket are hinge-controlled.
		lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_HINGE, 20*lCarWeight, 2.0f, 1.0f, 2);
		lEngine->AddControlledNode(20, 5);
		lEngine->AddControlledNode(22, 2, ContextObjectEngine::MODE_HALF_LOCK);
		lEngine->AddControlledNode(23, 1, ContextObjectEngine::MODE_HALF_LOCK);
		pObject->AddAttribute(lEngine);
		// Just a "dummy engine" to keep wheels straight at all times.
		lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_ROLL_STRAIGHT, 0, 0, 0, 0);
		lEngine->AddControlledNode(3, -1);
		lEngine->AddControlledNode(4, 1);
		lEngine->AddControlledNode(5, -1);
		lEngine->AddControlledNode(6, 1);
		lEngine->AddControlledNode(7, -1);
		lEngine->AddControlledNode(8, 1);
		pObject->AddAttribute(lEngine);

		pObject->GetManager()->EnablePhysicsUpdateCallback(pObject);

		lOk = true;
	}
	else if (pObject->GetClassId().find(_T("crane_whatever")) != Lepra::String::npos)
	{
		const float lTowerWeight = 50000;
		const float lTowerHeight = 25.0f;
		const float lThickness = 2.5f;
		const Lepra::Vector3DF lTowerDimensions(lThickness, lThickness, lTowerHeight);
		const float lX = (float)Lepra::Random::Uniform(-200, 200);
		const float lY = (float)Lepra::Random::Uniform(-200, 200);
		lTransformation.SetPosition(Lepra::Vector3DF(lX, lY, 250+lTowerHeight/2));

		// Body.
		TBC::PhysicsEngine::BodyID lTowerId = lPhysicsManager->CreateBox(lTransformation, lTowerWeight, lTowerDimensions,
			TBC::PhysicsEngine::DYNAMIC, 0.5f, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lTowerId);
		pObject->AddPhysicsObject(PhysicsNode(0, 1, lTowerId));

		// Jib.
		const float lJibWeight = lTowerWeight/3.5f;
		const float lJibPlacementHeight = 20.0f;
		const float lJibLength = 15.0f;
		const float lCounterJibLength = 5.0f;
		const float lTotalJibLength = lJibLength+lCounterJibLength;
		TBC::PhysicsEngine::JointID lJoint;

		Lepra::TransformationF lJibTransform(lTransformation);
		lJibTransform.GetPosition().Add(0, 0, -lTowerHeight/2 + lJibPlacementHeight);
		const Lepra::Vector3DF lJibAnchorPosition(lJibTransform.GetPosition());
		lJibTransform.GetPosition().Add(0, lTotalJibLength/2-lCounterJibLength, 0);
		const Lepra::Vector3DF lJibDimensions(lThickness, lTotalJibLength, lThickness);
		TBC::PhysicsEngine::BodyID lJibId = lPhysicsManager->CreateBox(lJibTransform, lJibWeight, lJibDimensions,
			TBC::PhysicsEngine::DYNAMIC, 0.5f, 0.5f, pTriggerListener, pObject);
		lPhysicsManager->ActivateGravity(lJibId);
		lJoint = lPhysicsManager->CreateHingeJoint(lTowerId, lJibId,
			lJibAnchorPosition, Lepra::Vector3DF(0, 0, 1));
		pObject->AddPhysicsObject(PhysicsNode(1, 2, lJibId, PhysicsNode::TYPE_HINGE, lJoint));

		// Wire.
		const float lWireWeight = 50;
		const int lWireSegmentCount = 4;
		const float lWireSegmentWeight = lWireWeight/lWireSegmentCount;
		const float lWireLength = (lJibPlacementHeight-1)/lWireSegmentCount;
		const float lWireThickness = 0.05f;
		Lepra::TransformationF lWireTransform(lJibTransform);
		lWireTransform.GetPosition().Add(0, lTotalJibLength/4, 0);
		unsigned lParentIndex = 2;
		TBC::PhysicsEngine::BodyID lParentId = lJibId;
		for (int x = 0; x < lWireSegmentCount; ++x)
		{
			Lepra::Vector3DF lWireAnchor(lWireTransform.GetPosition());
			lWireTransform.GetPosition().Add(0, 0, -lWireLength/2);
			lPhysicsObjectId = lPhysicsManager->CreateCapsule(lWireTransform, lWireSegmentWeight, lWireThickness/2,
				lWireLength, TBC::PhysicsEngine::DYNAMIC, 0.5f, 0.5f, pTriggerListener, pObject);
			lPhysicsManager->ActivateGravity(lPhysicsObjectId);
			//lJoint = lPhysicsManager->CreateBallJoint(lParentId, lPhysicsObjectId, lWireAnchor);
			lJoint = lPhysicsManager->CreateUniversalJoint(lParentId, lPhysicsObjectId, lWireAnchor, Lepra::Vector3DF(1, 0, 0), Lepra::Vector3DF(0, 1, 0));
			pObject->AddPhysicsObject(PhysicsNode(lParentIndex, lParentIndex+1, lPhysicsObjectId, PhysicsNode::TYPE_UNIVERSAL, lJoint));

			lWireTransform.GetPosition().Add(0, 0, -lWireLength/2);
			++lParentIndex;
			lParentId = lPhysicsObjectId;
		}

		// Hook.
		const float lHookWeight = 20;
		const float lHookWidth = 0.25f;
		const float lHookHeight = 0.5f;
		Lepra::Vector3DF lHookAnchor(lWireTransform.GetPosition());
		Lepra::TransformationF lHookTransform = lWireTransform;
		lHookTransform.GetPosition().Add(0, 0, -lHookHeight/2);
		lPhysicsObjectId = lPhysicsManager->CreateBox(lHookTransform, lHookWeight, Lepra::Vector3DF(lHookWidth, lHookWidth, lHookHeight),
			TBC::PhysicsEngine::DYNAMIC, 0.5f, 0.5f, pObject, pObject);
		lPhysicsManager->ActivateGravity(lPhysicsObjectId);
		//lJoint = lPhysicsManager->CreateBallJoint(lParentId, lPhysicsObjectId, lHookAnchor);
		lJoint = lPhysicsManager->CreateUniversalJoint(lParentId, lPhysicsObjectId, lHookAnchor, Lepra::Vector3DF(1, 0, 0), Lepra::Vector3DF(0, 1, 0));
		pObject->AddPhysicsObject(PhysicsNode(lParentIndex, lParentIndex+1, lPhysicsObjectId,
			PhysicsNode::TYPE_UNIVERSAL, lJoint, PhysicsNode::CONNECTOR_3));

		// Jib rotational engine.
		ContextObjectEngine* lEngine = new ContextObjectEngine(pObject, ContextObjectEngine::ENGINE_HINGE, lJibWeight, 2.0f, 1.0f, 1);
		lEngine->AddControlledNode(2, 1.0f);
		pObject->AddAttribute(lEngine);

		pObject->GetManager()->EnablePhysicsUpdateCallback(pObject);

		lOk = true;
	}
	else if (pObject->GetClassId().find(_T("ground_002")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lDimensions(lFloorSize, lFloorSize, lFloorSize);
		lPhysicsObjectId = lPhysicsManager->CreateBox(lTransformation, 0, lDimensions,
			TBC::PhysicsEngine::STATIC, 2, 0.5f, 0);
		pObject->AddPhysicsObject(PhysicsNode(0, 1, lPhysicsObjectId));

		Lepra::RotationMatrixF lRotation;
		// Place lower climb.
		lDimensions.Set(lRoadWidth, lUphillLength, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength+lRoadWidth/2,
			0,
			lUphillOrthogonalLength/2+lFloorSize/2-lRoadHeight/2/1.5f));
		lRotation.RotateAroundOwnX(Lepra::PIF/4);
		lTransformation.SetOrientation(lRotation);
		lPhysicsObjectId = lPhysicsManager->CreateBox(lTransformation, 0, lDimensions, TBC::PhysicsEngine::STATIC, 0.5f, 1.0f, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 2, lPhysicsObjectId));
		// Upper climb.
		lDimensions.Set(lUphillLength, lRoadWidth, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(lPlateauLength+lUphillOrthogonalLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength*3/2+lFloorSize/2-lRoadHeight/2/1.5f));
		lRotation = Lepra::RotationMatrixF();
		lRotation.RotateAroundOwnY(Lepra::PIF/4);
		lTransformation.SetOrientation(lRotation);
		lPhysicsObjectId = lPhysicsManager->CreateBox(lTransformation, 0, lDimensions, TBC::PhysicsEngine::STATIC, 0.5f, 1.0f, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 3, lPhysicsObjectId));
		// First plateau.
		lDimensions.Set(lRoadWidth, lPlateauLength+lPlateauLengthCompensation, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength+lRoadWidth/2,
			lUphillOrthogonalLength/2+lPlateauLength/2,
			lUphillOrthogonalLength+lFloorSize/2-lRoadHeight/2));
		lTransformation.SetOrientation(Lepra::RotationMatrixF());
		lPhysicsObjectId = lPhysicsManager->CreateBox(lTransformation, 0, lDimensions, TBC::PhysicsEngine::STATIC, 0.5f, 1.0f, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 4, lPhysicsObjectId));
		// Second plateau.
		lDimensions.Set(lPlateauLength, lRoadWidth, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(
			lPlateauLength+lUphillOrthogonalLength+lPlateauLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength+lFloorSize/2-lRoadHeight/2));
		lPhysicsObjectId = lPhysicsManager->CreateBox(lTransformation, 0, lDimensions, TBC::PhysicsEngine::STATIC, 0.5f, 1.0f, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 5, lPhysicsObjectId));
		// Top plateau.
		lDimensions.Set(lPlateauLength+lPlateauLengthCompensation, lRoadWidth, lRoadHeight);
		lTransformation.SetPosition(Lepra::Vector3DF(lPlateauLength/2,
			lUphillOrthogonalLength/2+lPlateauLength-lRoadWidth/2,
			lUphillOrthogonalLength*2+lFloorSize/2-lRoadHeight/2));
		lPhysicsObjectId = lPhysicsManager->CreateBox(lTransformation, 0, lDimensions, TBC::PhysicsEngine::STATIC, 0.5f, 1.0f, 0);
		pObject->AddPhysicsObject(PhysicsNode(1, 6, lPhysicsObjectId));

		lOk = true;
	}
	else
	{
		mLog.Error(_T("Unknown context object type."));
	}
	return (lOk);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObject);
LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObjectFactory);



}
