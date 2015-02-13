
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/PositionHauler.h"
#include "../../Tbc/Include/ChunkyBoneGeometry.h"
#include "../../Tbc/Include/ChunkyPhysics.h"
#include "../../Tbc/Include/PhysicsEngine.h"
#include "../../Tbc/Include/PhysicsManager.h"
#include "../Include/PositionalData.h"
#include "../../Lepra/Include/LepraAssert.h"



namespace Cure
{



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



bool PositionHauler::Get(ObjectPositionalData& pPosition, const Tbc::PhysicsManager* pPhysicsManager, const Tbc::ChunkyPhysics* pStructure, float pTotalMass)
{
	if (!pStructure)
	{
		return false;
	}

	Tbc::ChunkyBoneGeometry* lStructureGeometry = pStructure->GetBoneGeometry(pStructure->GetRootBone());
	Tbc::PhysicsManager::BodyID lBody = lStructureGeometry->GetBodyId();
	if (!lBody)
	{
		pPhysicsManager->GetBodyTransform(lStructureGeometry->GetTriggerId(), pPosition.mPosition.mTransformation);
		return true;
	}
	pPhysicsManager->GetBodyTransform(lBody, pPosition.mPosition.mTransformation);
	pPhysicsManager->GetBodyVelocity(lBody, pPosition.mPosition.mVelocity);
	pPhysicsManager->GetBodyAcceleration(lBody, pTotalMass, pPosition.mPosition.mAcceleration);
	pPhysicsManager->GetBodyAngularVelocity(lBody, pPosition.mPosition.mAngularVelocity);
	pPhysicsManager->GetBodyAngularAcceleration(lBody, pTotalMass, pPosition.mPosition.mAngularAcceleration);

	const int lGeometryCount = pStructure->GetBoneCount();
	size_t y = 0;
	for (int x = 0; x < lGeometryCount; ++x)
	{
		// TODO: add support for parent ID??? (JB 2009-07-07: Don't know anymore what this comment might mean.)
		// ??? Could it be when connected to something else, like a car connected to a crane?
		const Tbc::ChunkyBoneGeometry* lStructureGeometry = pStructure->GetBoneGeometry(x);
		if (!lStructureGeometry)
		{
			mLog.AError("Could not get positional update (for streaming), since *WHOLE* physical object not loaded!");
			return false;
		}
		lBody = lStructureGeometry->GetBodyId();
		Tbc::PhysicsManager::JointID lJoint = lStructureGeometry->GetJointId();
		switch (lStructureGeometry->GetJointType())
		{
			case Tbc::ChunkyBoneGeometry::JOINT_SUSPEND_HINGE:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, PositionalData2, lData, PositionalData::TYPE_POSITION_2, 1);
				++y;
				Tbc::PhysicsManager::Joint3Diff lDiff;
				if (!pPhysicsManager->GetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get hinge-2!");
					return false;
				}
				lData->mTransformation[0] = lDiff.mValue;
				lData->mTransformation[1] = lDiff.mAngle2;
				lData->mVelocity[0] = lDiff.mValueVelocity;
				lData->mVelocity[1] = lDiff.mAngle2Velocity;
				lData->mAcceleration[0] = lDiff.mValueAcceleration;
				lData->mAcceleration[1] = lDiff.mAngle2Acceleration;
			}
			break;
			case Tbc::ChunkyBoneGeometry::JOINT_HINGE2:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, PositionalData3, lData, PositionalData::TYPE_POSITION_3, 1);
				++y;
				Tbc::PhysicsManager::Joint3Diff lDiff;
				if (!pPhysicsManager->GetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get hinge-2!");
					return false;
				}
				lData->mTransformation[0] = lDiff.mValue;
				lData->mTransformation[1] = lDiff.mAngle1;
				lData->mTransformation[2] = lDiff.mAngle2;
				lData->mVelocity[0] = lDiff.mValueVelocity;
				lData->mVelocity[1] = lDiff.mAngle1Velocity;
				lData->mVelocity[2] = lDiff.mAngle2Velocity;
				lData->mAcceleration[0] = lDiff.mValueAcceleration;
				lData->mAcceleration[1] = lDiff.mAngle1Acceleration;
				lData->mAcceleration[2] = lDiff.mAngle2Acceleration;
			}
			break;
			case Tbc::ChunkyBoneGeometry::JOINT_HINGE:
			case Tbc::ChunkyBoneGeometry::JOINT_SLIDER:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, PositionalData1, lData, PositionalData::TYPE_POSITION_1, 1);
				++y;
				Tbc::PhysicsManager::Joint1Diff lDiff;
				if (!pPhysicsManager->GetJoint1Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get hinge!");
					return false;
				}
				lData->mTransformation = lDiff.mValue;
				lData->mVelocity = lDiff.mVelocity;
				lData->mAcceleration = lDiff.mAcceleration;
			}
			break;
			case Tbc::ChunkyBoneGeometry::JOINT_BALL:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, PositionalData3, lData, PositionalData::TYPE_POSITION_3, 0.00001f);
				++y;
				Tbc::PhysicsManager::Joint3Diff lDiff;
				if (!pPhysicsManager->GetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get ball!");
					return false;
				}
				lData->mTransformation[0] = lDiff.mValue;
				lData->mTransformation[1] = lDiff.mAngle1;
				lData->mTransformation[2] = lDiff.mAngle2;
				lData->mVelocity[0] = lDiff.mValueVelocity;
				lData->mVelocity[1] = lDiff.mAngle1Velocity;
				lData->mVelocity[2] = lDiff.mAngle2Velocity;
				lData->mAcceleration[0] = lDiff.mValueAcceleration;
				lData->mAcceleration[1] = lDiff.mAngle1Acceleration;
				lData->mAcceleration[2] = lDiff.mAngle2Acceleration;
			}
			break;
			case Tbc::ChunkyBoneGeometry::JOINT_UNIVERSAL:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, PositionalData2, lData, PositionalData::TYPE_POSITION_2, 1);
				++y;
				Tbc::PhysicsManager::Joint2Diff lDiff;
				if (!pPhysicsManager->GetJoint2Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get universal!");
					return false;
				}
				lData->mTransformation[0] = lDiff.mValue;
				lData->mTransformation[1] = lDiff.mAngle;
				lData->mVelocity[0] = lDiff.mValueVelocity;
				lData->mVelocity[1] = lDiff.mAngleVelocity;
				lData->mAcceleration[0] = lDiff.mValueAcceleration;
				lData->mAcceleration[1] = lDiff.mAngleAcceleration;
			}
			break;
			case Tbc::ChunkyBoneGeometry::JOINT_EXCLUDE:
			{
			}
			break;
			default:
			{
				deb_assert(false);
			}
			break;
		}
	}

	const int lEngineCount = pStructure->GetEngineCount();
	for (int z = 0; z != lEngineCount; ++z)
	{
		// TODO: add support for parent ID??????????? JB 2009-07-08: don't know what this is anymore.
		const Tbc::PhysicsEngine* lEngine = pStructure->GetEngine(z);
		switch (lEngine->GetEngineType())
		{
			case Tbc::PhysicsEngine::ENGINE_WALK:
			case Tbc::PhysicsEngine::ENGINE_PUSH_RELATIVE:
			case Tbc::PhysicsEngine::ENGINE_PUSH_ABSOLUTE:
			case Tbc::PhysicsEngine::ENGINE_PUSH_TURN_RELATIVE:
			case Tbc::PhysicsEngine::ENGINE_PUSH_TURN_ABSOLUTE:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, RealData3, lData, PositionalData::TYPE_REAL_3, 100);
				++y;
				::memcpy(lData->mValue, lEngine->GetValues(), sizeof(float)*Tbc::PhysicsEngine::ASPECT_MAX_REMOTE_COUNT);
			}
			break;
			case Tbc::PhysicsEngine::ENGINE_HOVER:
			case Tbc::PhysicsEngine::ENGINE_HINGE_ROLL:
			case Tbc::PhysicsEngine::ENGINE_HINGE_GYRO:
			case Tbc::PhysicsEngine::ENGINE_HINGE_BRAKE:
			case Tbc::PhysicsEngine::ENGINE_HINGE_TORQUE:
			case Tbc::PhysicsEngine::ENGINE_HINGE2_TURN:
			case Tbc::PhysicsEngine::ENGINE_ROTOR:
			case Tbc::PhysicsEngine::ENGINE_ROTOR_TILT:
			case Tbc::PhysicsEngine::ENGINE_JET:
			case Tbc::PhysicsEngine::ENGINE_SLIDER_FORCE:
			case Tbc::PhysicsEngine::ENGINE_YAW_BRAKE:
			case Tbc::PhysicsEngine::ENGINE_AIR_BRAKE:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, RealData1, lData, PositionalData::TYPE_REAL_1, 100);
				++y;
				lData->mValue = lEngine->GetValue();
				//deb_assert(lData->mValue >= -1 && lData->mValue <= 1);
			}
			break;
			case Tbc::PhysicsEngine::ENGINE_GLUE:
			case Tbc::PhysicsEngine::ENGINE_BALL_BRAKE:
			{
				// Unsynchronized "engine".
			}
			break;
			default:
			{
				deb_assert(false);
			}
			break;
		}
	}

	pPosition.Trunkate(y);

	return true;
}

void PositionHauler::Set(const ObjectPositionalData& pPosition, Tbc::PhysicsManager* pPhysicsManager, Tbc::ChunkyPhysics* pStructure, float pTotalMass, bool pAllowMoveRoot)
{
	if (pAllowMoveRoot)
	{
		const Tbc::ChunkyBoneGeometry* lRootGeometry = pStructure->GetBoneGeometry(pStructure->GetRootBone());
		const Tbc::PhysicsManager::BodyID lBody = lRootGeometry->GetBodyId();
		if (lBody)
		{
			pPhysicsManager->SetBodyTransform(lBody, pPosition.mPosition.mTransformation);
			if (pStructure->GetPhysicsType() == Tbc::ChunkyPhysics::DYNAMIC)
			{
				pPhysicsManager->SetBodyVelocity(lBody, pPosition.mPosition.mVelocity);
				pPhysicsManager->SetBodyAcceleration(lBody, pTotalMass, pPosition.mPosition.mAcceleration);
				pPhysicsManager->SetBodyAngularVelocity(lBody, pPosition.mPosition.mAngularVelocity);
				pPhysicsManager->SetBodyAngularAcceleration(lBody, pTotalMass, pPosition.mPosition.mAngularAcceleration);
			}
		}
	}
	else
	{
		mLog.AInfo("Skipping setting of main body; we're owned by someone else.");
	}

	if (pPosition.mBodyPositionArray.size() <= 0)
	{
		return;
	}

	//mLog.AInfo("Setting full position.");

	const int lBoneCount = pStructure->GetBoneCount();
	size_t y = 0;
	for (int x = 0; x < lBoneCount && y < pPosition.mBodyPositionArray.size(); ++x)
	{
		// TODO: add support for parent ID.
		const Tbc::ChunkyBoneGeometry* lStructureGeometry = pStructure->GetBoneGeometry(x);
		Tbc::PhysicsManager::BodyID lBody = lStructureGeometry->GetBodyId();
		Tbc::PhysicsManager::JointID lJoint = lStructureGeometry->GetJointId();
		switch (lStructureGeometry->GetJointType())
		{
			case Tbc::ChunkyBoneGeometry::JOINT_SUSPEND_HINGE:
			{
				deb_assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_2);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const PositionalData2, lData, PositionalData::TYPE_POSITION_2);
				++y;
				deb_assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const Tbc::PhysicsManager::Joint3Diff lDiff(lData->mTransformation[0], lData->mTransformation[1], 100000,
					lData->mVelocity[0], lData->mVelocity[1], 100000,
					lData->mAcceleration[0], lData->mAcceleration[1], 100000);
				if (!pPhysicsManager->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge-2!");
					return;
				}
			}
			break;
			case Tbc::ChunkyBoneGeometry::JOINT_HINGE2:
			{
				deb_assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_3);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const PositionalData3, lData, PositionalData::TYPE_POSITION_3);
				++y;
				deb_assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const Tbc::PhysicsManager::Joint3Diff lDiff(lData->mTransformation[0], lData->mTransformation[1], lData->mTransformation[2],
					lData->mVelocity[0], lData->mVelocity[1], lData->mVelocity[2],
					lData->mAcceleration[0], lData->mAcceleration[1], lData->mAcceleration[2]);
				if (!pPhysicsManager->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge-2!");
					return;
				}
			}
			break;
			case Tbc::ChunkyBoneGeometry::JOINT_HINGE:
			case Tbc::ChunkyBoneGeometry::JOINT_SLIDER:
			{
				deb_assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_1);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const PositionalData1, lData, PositionalData::TYPE_POSITION_1);
				++y;
				deb_assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const Tbc::PhysicsManager::Joint1Diff lDiff(lData->mTransformation,
					lData->mVelocity, lData->mAcceleration);
				if (!pPhysicsManager->SetJoint1Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge!");
					return;
				}
			}
			break;
			case Tbc::ChunkyBoneGeometry::JOINT_BALL:
			{
				deb_assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_3);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const PositionalData3, lData, PositionalData::TYPE_POSITION_3);
				++y;
				deb_assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const Tbc::PhysicsManager::Joint3Diff lDiff(lData->mTransformation[0], lData->mTransformation[1], lData->mTransformation[2],
					lData->mVelocity[0], lData->mVelocity[1], lData->mVelocity[2],
					lData->mAcceleration[0], lData->mAcceleration[1], lData->mAcceleration[2]);
				if (!pPhysicsManager->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set ball!");
					return;
				}
			}
			break;
			case Tbc::ChunkyBoneGeometry::JOINT_UNIVERSAL:
			{
				deb_assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_2);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const PositionalData2, lData, PositionalData::TYPE_POSITION_2);
				++y;
				deb_assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const Tbc::PhysicsManager::Joint2Diff lDiff(lData->mTransformation[0], lData->mTransformation[1],
					lData->mVelocity[0], lData->mVelocity[1],
					lData->mAcceleration[0], lData->mAcceleration[1]);
				if (!pPhysicsManager->SetJoint2Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set universal!");
					return;
				}
			}
			break;
			case Tbc::ChunkyBoneGeometry::JOINT_EXCLUDE:
			{
			}
			break;
			default:
			{
				deb_assert(false);
			}
			break;
		}
	}

	const int lEngineCount = pStructure->GetEngineCount();
	for (int z = 0; z != lEngineCount; ++z)
	{
		// TODO: add support for parent ID??????????? JB 2009-07-08: don't know what this is anymore.
		const Tbc::PhysicsEngine* lEngine = pStructure->GetEngine(z);
		switch (lEngine->GetEngineType())
		{
			case Tbc::PhysicsEngine::ENGINE_WALK:
			case Tbc::PhysicsEngine::ENGINE_PUSH_RELATIVE:
			case Tbc::PhysicsEngine::ENGINE_PUSH_ABSOLUTE:
			case Tbc::PhysicsEngine::ENGINE_PUSH_TURN_RELATIVE:
			case Tbc::PhysicsEngine::ENGINE_PUSH_TURN_ABSOLUTE:
			{
				deb_assert(pPosition.mBodyPositionArray.size() > y);
				deb_assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_3);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const RealData3, lData, PositionalData::TYPE_REAL_3);
				++y;
				deb_assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				pStructure->SetEnginePower(lEngine->GetControllerIndex()+0, lData->mValue[0]);
				pStructure->SetEnginePower(lEngine->GetControllerIndex()+1, lData->mValue[1]);
				pStructure->SetEnginePower(lEngine->GetControllerIndex()+3, lData->mValue[2]);
			}
			break;
			case Tbc::PhysicsEngine::ENGINE_HOVER:
			case Tbc::PhysicsEngine::ENGINE_HINGE_ROLL:
			case Tbc::PhysicsEngine::ENGINE_HINGE_GYRO:
			case Tbc::PhysicsEngine::ENGINE_HINGE_BRAKE:
			case Tbc::PhysicsEngine::ENGINE_HINGE_TORQUE:
			case Tbc::PhysicsEngine::ENGINE_HINGE2_TURN:
			case Tbc::PhysicsEngine::ENGINE_ROTOR:
			case Tbc::PhysicsEngine::ENGINE_ROTOR_TILT:
			case Tbc::PhysicsEngine::ENGINE_JET:
			case Tbc::PhysicsEngine::ENGINE_SLIDER_FORCE:
			case Tbc::PhysicsEngine::ENGINE_YAW_BRAKE:
			case Tbc::PhysicsEngine::ENGINE_AIR_BRAKE:
			{
				deb_assert(pPosition.mBodyPositionArray.size() > y);
				deb_assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_1);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const RealData1, lData, PositionalData::TYPE_REAL_1);
				++y;
				deb_assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				deb_assert(lData->mValue >= -5 && lData->mValue <= 5);
				pStructure->SetEnginePower(lEngine->GetControllerIndex(), lData->mValue);
			}
			break;
			case Tbc::PhysicsEngine::ENGINE_GLUE:
			case Tbc::PhysicsEngine::ENGINE_BALL_BRAKE:
			{
				// Unsynchronized "engine".
			}
			break;
			default:
			{
				deb_assert(false);
			}
			break;
		}
	}
}



loginstance(GAME_CONTEXT, PositionHauler);



}
