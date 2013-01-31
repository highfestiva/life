
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/PositionHauler.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../Include/PositionalData.h"
#include <assert.h>



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



bool PositionHauler::Get(ObjectPositionalData& pPosition, const TBC::PhysicsManager* pPhysicsManager, const TBC::ChunkyPhysics* pStructure, float pTotalMass)
{
	if (!pStructure)
	{
		return false;
	}

	TBC::ChunkyBoneGeometry* lStructureGeometry = pStructure->GetBoneGeometry(pStructure->GetRootBone());
	TBC::PhysicsManager::BodyID lBody = lStructureGeometry->GetBodyId();
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
		const TBC::ChunkyBoneGeometry* lStructureGeometry = pStructure->GetBoneGeometry(x);
		if (!lStructureGeometry)
		{
			mLog.AError("Could not get positional update (for streaming), since *WHOLE* physical object not loaded!");
			return false;
		}
		lBody = lStructureGeometry->GetBodyId();
		TBC::PhysicsManager::JointID lJoint = lStructureGeometry->GetJointId();
		switch (lStructureGeometry->GetJointType())
		{
			case TBC::ChunkyBoneGeometry::JOINT_SUSPEND_HINGE:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, PositionalData2, lData, PositionalData::TYPE_POSITION_2, 1);
				++y;
				TBC::PhysicsManager::Joint3Diff lDiff;
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
			case TBC::ChunkyBoneGeometry::JOINT_HINGE2:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, PositionalData3, lData, PositionalData::TYPE_POSITION_3, 1);
				++y;
				TBC::PhysicsManager::Joint3Diff lDiff;
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
			case TBC::ChunkyBoneGeometry::JOINT_HINGE:
			case TBC::ChunkyBoneGeometry::JOINT_SLIDER:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, PositionalData1, lData, PositionalData::TYPE_POSITION_1, 1);
				++y;
				TBC::PhysicsManager::Joint1Diff lDiff;
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
			case TBC::ChunkyBoneGeometry::JOINT_BALL:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, PositionalData3, lData, PositionalData::TYPE_POSITION_3, 0.00001f);
				++y;
				TBC::PhysicsManager::Joint3Diff lDiff;
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
			case TBC::ChunkyBoneGeometry::JOINT_UNIVERSAL:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, PositionalData2, lData, PositionalData::TYPE_POSITION_2, 1);
				++y;
				TBC::PhysicsManager::Joint2Diff lDiff;
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
			case TBC::ChunkyBoneGeometry::JOINT_EXCLUDE:
			{
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}
	}

	const int lEngineCount = pStructure->GetEngineCount();
	for (int z = 0; z != lEngineCount; ++z)
	{
		// TODO: add support for parent ID??????????? JB 2009-07-08: don't know what this is anymore.
		const TBC::PhysicsEngine* lEngine = pStructure->GetEngine(z);
		switch (lEngine->GetEngineType())
		{
			case TBC::PhysicsEngine::ENGINE_CAMERA_FLAT_PUSH:
			case TBC::PhysicsEngine::ENGINE_CAMERA_3D_PUSH:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, RealData3, lData, PositionalData::TYPE_REAL_3, 100);
				++y;
				::memcpy(lData->mValue, lEngine->GetValues(), sizeof(float)*TBC::PhysicsEngine::ASPECT_MAX_REMOTE_COUNT);
			}
			break;
			case TBC::PhysicsEngine::ENGINE_HOVER:
			case TBC::PhysicsEngine::ENGINE_HINGE_ROLL:
			case TBC::PhysicsEngine::ENGINE_HINGE_GYRO:
			case TBC::PhysicsEngine::ENGINE_HINGE_BRAKE:
			case TBC::PhysicsEngine::ENGINE_HINGE_TORQUE:
			case TBC::PhysicsEngine::ENGINE_HINGE2_TURN:
			case TBC::PhysicsEngine::ENGINE_ROTOR:
			case TBC::PhysicsEngine::ENGINE_TILTER:
			case TBC::PhysicsEngine::ENGINE_JET:
			case TBC::PhysicsEngine::ENGINE_SLIDER_FORCE:
			case TBC::PhysicsEngine::ENGINE_YAW_BRAKE:
			{
				GETSET_OBJECT_POSITIONAL_AT(pPosition, y, RealData1, lData, PositionalData::TYPE_REAL_1, 100);
				++y;
				lData->mValue = lEngine->GetValue();
				assert(lData->mValue >= -1 && lData->mValue <= 1);
			}
			break;
			case TBC::PhysicsEngine::ENGINE_GLUE:
			case TBC::PhysicsEngine::ENGINE_BALL_BRAKE:
			{
				// Unsynchronized "engine".
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}
	}

	pPosition.Trunkate(y);

	return true;
}

void PositionHauler::Set(const ObjectPositionalData& pPosition, TBC::PhysicsManager* pPhysicsManager, TBC::ChunkyPhysics* pStructure, float pTotalMass, bool pAllowMoveRoot)
{
	if (pAllowMoveRoot)
	{
		if (pStructure->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC)
		{
			const TBC::ChunkyBoneGeometry* lRootGeometry = pStructure->GetBoneGeometry(pStructure->GetRootBone());
			TBC::PhysicsManager::BodyID lBody = lRootGeometry->GetBodyId();
			pPhysicsManager->SetBodyTransform(lBody, pPosition.mPosition.mTransformation);
			pPhysicsManager->SetBodyVelocity(lBody, pPosition.mPosition.mVelocity);
			pPhysicsManager->SetBodyAcceleration(lBody, pTotalMass, pPosition.mPosition.mAcceleration);
			pPhysicsManager->SetBodyAngularVelocity(lBody, pPosition.mPosition.mAngularVelocity);
			pPhysicsManager->SetBodyAngularAcceleration(lBody, pTotalMass, pPosition.mPosition.mAngularAcceleration);
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
		const TBC::ChunkyBoneGeometry* lStructureGeometry = pStructure->GetBoneGeometry(x);
		TBC::PhysicsManager::BodyID lBody = lStructureGeometry->GetBodyId();
		TBC::PhysicsManager::JointID lJoint = lStructureGeometry->GetJointId();
		switch (lStructureGeometry->GetJointType())
		{
			case TBC::ChunkyBoneGeometry::JOINT_SUSPEND_HINGE:
			{
				assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_2);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const PositionalData2, lData, PositionalData::TYPE_POSITION_2);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsManager::Joint3Diff lDiff(lData->mTransformation[0], lData->mTransformation[1], 100000,
					lData->mVelocity[0], lData->mVelocity[1], 100000,
					lData->mAcceleration[0], lData->mAcceleration[1], 100000);
				if (!pPhysicsManager->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge-2!");
					return;
				}
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_HINGE2:
			{
				assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_3);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const PositionalData3, lData, PositionalData::TYPE_POSITION_3);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsManager::Joint3Diff lDiff(lData->mTransformation[0], lData->mTransformation[1], lData->mTransformation[2],
					lData->mVelocity[0], lData->mVelocity[1], lData->mVelocity[2],
					lData->mAcceleration[0], lData->mAcceleration[1], lData->mAcceleration[2]);
				if (!pPhysicsManager->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge-2!");
					return;
				}
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_HINGE:
			case TBC::ChunkyBoneGeometry::JOINT_SLIDER:
			{
				assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_1);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const PositionalData1, lData, PositionalData::TYPE_POSITION_1);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsManager::Joint1Diff lDiff(lData->mTransformation,
					lData->mVelocity, lData->mAcceleration);
				if (!pPhysicsManager->SetJoint1Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge!");
					return;
				}
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_BALL:
			{
				assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_3);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const PositionalData3, lData, PositionalData::TYPE_POSITION_3);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsManager::Joint3Diff lDiff(lData->mTransformation[0], lData->mTransformation[1], lData->mTransformation[2],
					lData->mVelocity[0], lData->mVelocity[1], lData->mVelocity[2],
					lData->mAcceleration[0], lData->mAcceleration[1], lData->mAcceleration[2]);
				if (!pPhysicsManager->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set ball!");
					return;
				}
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_UNIVERSAL:
			{
				assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_2);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const PositionalData2, lData, PositionalData::TYPE_POSITION_2);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsManager::Joint2Diff lDiff(lData->mTransformation[0], lData->mTransformation[1],
					lData->mVelocity[0], lData->mVelocity[1],
					lData->mAcceleration[0], lData->mAcceleration[1]);
				if (!pPhysicsManager->SetJoint2Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set universal!");
					return;
				}
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_EXCLUDE:
			{
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}
	}

	const int lEngineCount = pStructure->GetEngineCount();
	for (int z = 0; z != lEngineCount; ++z)
	{
		// TODO: add support for parent ID??????????? JB 2009-07-08: don't know what this is anymore.
		const TBC::PhysicsEngine* lEngine = pStructure->GetEngine(z);
		switch (lEngine->GetEngineType())
		{
			case TBC::PhysicsEngine::ENGINE_CAMERA_FLAT_PUSH:
			case TBC::PhysicsEngine::ENGINE_CAMERA_3D_PUSH:
			{
				assert(pPosition.mBodyPositionArray.size() > y);
				assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_3);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const RealData3, lData, PositionalData::TYPE_REAL_3);
				++y;
				assert(lData);
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
			case TBC::PhysicsEngine::ENGINE_HOVER:
			case TBC::PhysicsEngine::ENGINE_HINGE_ROLL:
			case TBC::PhysicsEngine::ENGINE_HINGE_GYRO:
			case TBC::PhysicsEngine::ENGINE_HINGE_BRAKE:
			case TBC::PhysicsEngine::ENGINE_HINGE_TORQUE:
			case TBC::PhysicsEngine::ENGINE_HINGE2_TURN:
			case TBC::PhysicsEngine::ENGINE_ROTOR:
			case TBC::PhysicsEngine::ENGINE_TILTER:
			case TBC::PhysicsEngine::ENGINE_JET:
			case TBC::PhysicsEngine::ENGINE_SLIDER_FORCE:
			case TBC::PhysicsEngine::ENGINE_YAW_BRAKE:
			{
				assert(pPosition.mBodyPositionArray.size() > y);
				assert(pPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_1);
				GET_OBJECT_POSITIONAL_AT(pPosition, y, const RealData1, lData, PositionalData::TYPE_REAL_1);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				assert(lData->mValue >= -1 && lData->mValue <= 1);
				pStructure->SetEnginePower(lEngine->GetControllerIndex(), lData->mValue);
			}
			break;
			case TBC::PhysicsEngine::ENGINE_GLUE:
			case TBC::PhysicsEngine::ENGINE_BALL_BRAKE:
			{
				// Unsynchronized "engine".
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT, PositionHauler);



}
