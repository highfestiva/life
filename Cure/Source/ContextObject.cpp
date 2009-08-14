
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/ContextObject.h"
#include <algorithm>
#include <assert.h>
#include <math.h>
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/RotationMatrix.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../Include/ContextManager.h"
#include "../Include/ContextObjectAttribute.h"
#include "../Include/Cure.h"
#include "../Include/GameManager.h"
#include "../Include/TimeManager.h"



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



ContextObject::ContextObject(const Lepra::String& pClassId):
	mManager(0),
	mInstanceId(0),
	mClassId(pClassId),
	mNetworkObjectType(NETWORK_OBJECT_LOCAL_ONLY),
	mStructure(0),
	mLastSendTime(0),
	mSendCount(0),
	mAllowMoveSelf(true)
{
}

ContextObject::~ContextObject()
{
	log_volatile(mLog.Debugf(_T("Destructing context object %s."), mClassId.c_str()));

	mManager->RemoveObject(this);

	// Detach from other context objects.
	{
		while (!mConnectionList.empty())
		{
			ContextObject* lObject2 = mConnectionList.front().mObject;
			DetachFromObject(lObject2);
		}
	}

	// Removes bodies from manager, then destroys all physical stuff.
	if (mStructure)
	{
		const int lBoneCount = mStructure->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			TBC::ChunkyBoneGeometry* lStructureGeometry = mStructure->GetBoneGeometry(x);
			mManager->RemovePhysicsBody(lStructureGeometry->GetBodyId());
		}
		mStructure->ClearAll(mManager->GetGameManager()->GetPhysicsManager());
	}

	// Fuck off, attributes.
	{
		AttributeArray::iterator x = mAttributeArray.begin();
		for (; x != mAttributeArray.end(); ++x)
		{
			delete (*x);
		}
		mAttributeArray.clear();
	}

	mManager->FreeGameObjectId(mNetworkObjectType, mInstanceId);
	mInstanceId = 0;
}



ContextManager* ContextObject::GetManager() const
{
	return (mManager);
}

void ContextObject::SetManager(ContextManager* pManager)
{
	assert(mManager == 0);
	mManager = pManager;
}

GameObjectId ContextObject::GetInstanceId() const
{
	return (mInstanceId);
}

void ContextObject::SetInstanceId(GameObjectId pInstanceId)
{
	assert(mInstanceId == 0);
	mInstanceId = pInstanceId;
}

const Lepra::String& ContextObject::GetClassId() const
{
	return (mClassId);
}



NetworkObjectType ContextObject::GetNetworkObjectType() const
{
	return (mNetworkObjectType);
}

void ContextObject::SetNetworkObjectType(NetworkObjectType pType)
{
	assert((mNetworkObjectType == pType) ||
		(mNetworkObjectType == NETWORK_OBJECT_LOCALLY_CONTROLLED && pType == NETWORK_OBJECT_REMOTE_CONTROLLED) ||
		(mNetworkObjectType == NETWORK_OBJECT_REMOTE_CONTROLLED && pType == NETWORK_OBJECT_LOCALLY_CONTROLLED) ||
		(mNetworkObjectType == NETWORK_OBJECT_LOCAL_ONLY));
	mNetworkObjectType = pType;
}



void ContextObject::SetAllowMoveSelf(bool pAllow)
{
	mAllowMoveSelf = pAllow;
}

void ContextObject::AttachToObject(TBC::PhysicsManager::BodyID pBody1, ContextObject* pObject2, TBC::PhysicsManager::BodyID pBody2)
{
	if (IsAttachedTo(pObject2))
	{
		return;
	}
	AttachToObject(GetStructureGeometry(pBody1), pObject2, pObject2->GetStructureGeometry(pBody2), true);
}

void ContextObject::AttachToObject(unsigned pBody1Index, ContextObject* pObject2, unsigned pBody2Index)
{
	if (IsAttachedTo(pObject2))
	{
		assert(false);
		return;
	}
	AttachToObject(mStructure->GetBoneGeometry(pBody1Index), pObject2, pObject2->GetStructureGeometry(pBody2Index), false);
}

bool ContextObject::DetachFromObject(ContextObject* /*pObject*/)
{
	bool lRemoved = false;

	// TODO: implement when new design is in place.

	/*ConnectionList::iterator x = mConnectionList.begin();
	for (; x != mConnectionList.end(); ++x)
	{
		if (pObject == x->mObject)
		{
			TBC::PhysicsManager::JointID lJointId = x->mJointId;
			TBC::PhysicsEngine* lEngine = x->mEngine;
			mConnectionList.erase(x);
			pObject->DetachFromObject(this);
			if (lJointId != TBC::INVALID_JOINT)
			{
				const int lBoneCount = mStructure->GetBoneCount();
				for (int x = 0; x < lBoneCount; ++x)
				{
					TBC::ChunkyBoneGeometry* lStructureGeometry = mStructure->GetBoneGeometry(x);
					if (lStructureGeometry->GetJointId() == lJointId)
					{
						mPhysicsNodeArray.erase(x);
						break;
					}
				}
				mManager->GetGameManager()->GetPhysicsManager()->DeleteJoint(lJointId);
				mManager->GetGameManager()->SendDetach(this, pObject);
			}
			if (lEngine)
			{
				RemoveAttribute(lEngine);
				delete (lEngine);
			}
			lRemoved = true;
			break;
		}
	}*/
	return (lRemoved);
}



void ContextObject::AddAttribute(ContextObjectAttribute* pAttribute)
{
	mAttributeArray.push_back(pAttribute);
}

void ContextObject::RemoveAttribute(ContextObjectAttribute* pAttribute)
{
	std::remove(mAttributeArray.begin(), mAttributeArray.end(), pAttribute);
}



bool ContextObject::UpdateFullPosition(const ObjectPositionalData*& pPositionalData)
{
	TBC::ChunkyBoneGeometry* lStructureGeometry = mStructure->GetBoneGeometry(mStructure->GetRootBone());
	if (!lStructureGeometry || lStructureGeometry->GetBodyId() == TBC::INVALID_BODY)
	{
		mLog.Errorf(_T("Could not get positional update (for streaming), since %i/%s not loaded yet!"),
			GetInstanceId(), GetClassId().c_str());
		return (false);
	}

	TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	TBC::PhysicsManager::BodyID lBody = lStructureGeometry->GetBodyId();
	lPhysicsManager->GetBodyTransform(lBody, mPosition.mPosition.mTransformation);
	lPhysicsManager->GetBodyVelocity(lBody, mPosition.mPosition.mVelocity);
	lPhysicsManager->GetBodyAcceleration(lBody, mPosition.mPosition.mAcceleration);
	lPhysicsManager->GetBodyAngularVelocity(lBody, mPosition.mPosition.mAngularVelocity);
	lPhysicsManager->GetBodyAngularAcceleration(lBody, mPosition.mPosition.mAngularAcceleration);

	const int lGeometryCount = mStructure->GetBoneCount();
	size_t y = 0;
	for (int x = 0; x < lGeometryCount; ++x)
	{
		// TODO: add support for parent ID??? JB 2009-07-07: Don't know anymore what this might mean.
		const TBC::ChunkyBoneGeometry* lStructureGeometry = mStructure->GetBoneGeometry(x);
		if (!lStructureGeometry)
		{
			mLog.Errorf(_T("Could not get positional update (for streaming), since *WHOLE* %i/%s not loaded yet!"),
				GetInstanceId(), GetClassId().c_str());
			return (false);
		}
		lBody = lStructureGeometry->GetBodyId();
		TBC::PhysicsManager::JointID lJoint = lStructureGeometry->GetJointId();
		switch (lStructureGeometry->GetJointType())
		{
			case TBC::ChunkyBoneGeometry::JOINT_SUSPEND_HINGE:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData2, lData, PositionalData::TYPE_POSITION_2, 1);
				++y;
				TBC::PhysicsManager::Joint3Diff lDiff;
				if (!lPhysicsManager->GetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get hinge-2!");
					return (false);
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
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData3, lData, PositionalData::TYPE_POSITION_3, 1);
				++y;
				TBC::PhysicsManager::Joint3Diff lDiff;
				if (!lPhysicsManager->GetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get hinge-2!");
					return (false);
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
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData1, lData, PositionalData::TYPE_POSITION_1, 1);
				++y;
				TBC::PhysicsManager::Joint1Diff lDiff;
				if (!lPhysicsManager->GetJoint1Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get hinge!");
					return (false);
				}
				lData->mTransformation = lDiff.mAngle;
				lData->mVelocity = lDiff.mAngleVelocity;
				lData->mAcceleration = lDiff.mAngleAcceleration;
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_BALL:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData3, lData, PositionalData::TYPE_POSITION_3, 0.00001f);
				++y;
				TBC::PhysicsManager::Joint3Diff lDiff;
				if (!lPhysicsManager->GetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get ball!");
					return (false);
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
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData2, lData, PositionalData::TYPE_POSITION_2, 1);
				++y;
				TBC::PhysicsManager::Joint2Diff lDiff;
				if (!lPhysicsManager->GetJoint2Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get universal!");
					return (false);
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

	const int lEngineCount = mStructure->GetEngineCount();
	for (int z = 0; z != lEngineCount; ++z)
	{
		// TODO: add support for parent ID??????????? JB 2009-07-08: don't know what this is anymore.
		const TBC::PhysicsEngine* lEngine = mStructure->GetEngine(z);
		switch (lEngine->GetEngineType())
		{
			case TBC::PhysicsEngine::ENGINE_CAMERA_FLAT_PUSH:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, RealData4, lData, PositionalData::TYPE_REAL_4, 1);
				++y;
				::memcpy(lData->mValue, lEngine->GetValues(), sizeof(float)*4);
			}
			break;
			case TBC::PhysicsEngine::ENGINE_HINGE2_ROLL:
			case TBC::PhysicsEngine::ENGINE_HINGE2_TURN:
			case TBC::PhysicsEngine::ENGINE_HINGE2_BREAK:
			case TBC::PhysicsEngine::ENGINE_HINGE:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, RealData1, lData, PositionalData::TYPE_REAL_1, 1);
				++y;
				lData->mValue = lEngine->GetValue();
			}
			break;
			case TBC::PhysicsEngine::ENGINE_ROLL_STRAIGHT:
			case TBC::PhysicsEngine::ENGINE_GLUE:
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

	mPosition.Trunkate(y);
	pPositionalData = &mPosition;

	++mSendCount;

	return (true);
}

void ContextObject::SetFullPosition(const ObjectPositionalData& pPositionalData)
{
	const TBC::ChunkyBoneGeometry* lGeometry = mStructure->GetBoneGeometry(mStructure->GetRootBone());
	if (!lGeometry || lGeometry->GetBodyId() == TBC::INVALID_BODY)
	{
		return;
	}

	if (mPosition.IsSameStructure(pPositionalData))
	{
		if (mPosition.GetScaledDifference(&pPositionalData) == 0)
		{
			return;
		}
	}

	mPosition.CopyData(&pPositionalData);

	TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	TBC::PhysicsManager::BodyID lBody;
	if (mAllowMoveSelf)
	{
		lBody = lGeometry->GetBodyId();
		lPhysicsManager->SetBodyTransform(lBody, pPositionalData.mPosition.mTransformation);
		lPhysicsManager->SetBodyVelocity(lBody, pPositionalData.mPosition.mVelocity);
		lPhysicsManager->SetBodyAcceleration(lBody, pPositionalData.mPosition.mAcceleration);
		lPhysicsManager->SetBodyAngularVelocity(lBody, pPositionalData.mPosition.mAngularVelocity);
		lPhysicsManager->SetBodyAngularAcceleration(lBody, pPositionalData.mPosition.mAngularAcceleration);
	}
	else
	{
		mLog.AInfo("Skipping setting of main body; we're owned by someone else.");
	}

	if (mPosition.mBodyPositionArray.size() <= 0)
	{
		return;
	}

	//mLog.AInfo("Setting full position.");

	const int lBoneCount = mStructure->GetBoneCount();
	size_t y = 0;
	for (int x = 0; x < lBoneCount; ++x)
	{
		assert(mPosition.mBodyPositionArray.size() > y);
		// TODO: add support for parent ID.
		const TBC::ChunkyBoneGeometry* lStructureGeometry = mStructure->GetBoneGeometry(x);
		lBody = lStructureGeometry->GetBodyId();
		TBC::PhysicsManager::JointID lJoint = lStructureGeometry->GetJointId();
		switch (lStructureGeometry->GetJointType())
		{
			case TBC::ChunkyBoneGeometry::JOINT_SUSPEND_HINGE:
			{
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_2);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const PositionalData2, lData, PositionalData::TYPE_POSITION_2);
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
				if (!lPhysicsManager->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge-2!");
					return;
				}
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_HINGE2:
			{
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_3);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const PositionalData3, lData, PositionalData::TYPE_POSITION_3);
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
				if (!lPhysicsManager->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge-2!");
					return;
				}
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_HINGE:
			{
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_1);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const PositionalData1, lData, PositionalData::TYPE_POSITION_1);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsManager::Joint1Diff lDiff(lData->mTransformation,
					lData->mVelocity, lData->mAcceleration);
				if (!lPhysicsManager->SetJoint1Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge!");
					return;
				}
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_BALL:
			{
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_3);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const PositionalData3, lData, PositionalData::TYPE_POSITION_3);
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
				if (!lPhysicsManager->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set ball!");
					return;
				}
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_UNIVERSAL:
			{
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_2);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const PositionalData2, lData, PositionalData::TYPE_POSITION_2);
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
				if (!lPhysicsManager->SetJoint2Diff(lBody, lJoint, lDiff))
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

	const int lEngineCount = mStructure->GetEngineCount();
	for (int z = 0; z != lEngineCount; ++z)
	{
		// TODO: add support for parent ID??????????? JB 2009-07-08: don't know what this is anymore.
		const TBC::PhysicsEngine* lEngine = mStructure->GetEngine(z);
		switch (lEngine->GetEngineType())
		{
			case TBC::PhysicsEngine::ENGINE_CAMERA_FLAT_PUSH:
			{
				assert(mPosition.mBodyPositionArray.size() > y);
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_4);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const RealData4, lData, PositionalData::TYPE_REAL_4);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				SetEnginePower(0, lData->mValue[0], lData->mValue[3]);
				SetEnginePower(1, lData->mValue[1], lData->mValue[3]);
				SetEnginePower(3, lData->mValue[2], lData->mValue[3]);	// TRICKY: specialcasing.
			}
			break;
			case TBC::PhysicsEngine::ENGINE_HINGE2_ROLL:
			case TBC::PhysicsEngine::ENGINE_HINGE2_TURN:
			case TBC::PhysicsEngine::ENGINE_HINGE2_BREAK:
			case TBC::PhysicsEngine::ENGINE_HINGE:
			{
				assert(mPosition.mBodyPositionArray.size() > y);
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_1);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const RealData1, lData, PositionalData::TYPE_REAL_1);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				assert(lData->mValue >= -1 && lData->mValue <= 1);
				SetEnginePower(lEngine->GetControllerIndex(), lData->mValue, 0);
			}
			break;
			case TBC::PhysicsEngine::ENGINE_ROLL_STRAIGHT:
			case TBC::PhysicsEngine::ENGINE_GLUE:
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

Lepra::Vector3DF ContextObject::GetPosition() const
{
	Lepra::Vector3DF lPosition;
	const TBC::ChunkyBoneGeometry* lGeometry = mStructure->GetBoneGeometry(mStructure->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
	{
		Lepra::TransformationF lTransform;
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lGeometry->GetBodyId(), lTransform);
		lPosition.x = lTransform.GetPosition().x;
		lPosition.y = lTransform.GetPosition().y;
		lPosition.z = lTransform.GetPosition().z;
	}
	else
	{
		assert(false);
		// TODO: throw something here...
	}
	return (lPosition);
}

float ContextObject::GetForwardSpeed() const
{
	float lSpeed = 0;
	const TBC::ChunkyBoneGeometry* lGeometry = mStructure->GetBoneGeometry(mStructure->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
	{
		Lepra::TransformationF lTransform;
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lGeometry->GetBodyId(), lTransform);
		Lepra::Vector3DF lVelocity;
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
		Lepra::Vector3DF lAxis = lTransform.GetOrientation().GetAxisY();
		lAxis.Normalize();
		lSpeed = -(lVelocity*lAxis);	// TODO: negate when objects created in the right direction.
	}
	else
	{
		assert(false);
	}
	return (lSpeed);
}

float ContextObject::GetMass() const
{
	float lTotalMass = 0;
	TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	const int lBoneCount = mStructure->GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = mStructure->GetBoneGeometry(x);
		lTotalMass += lPhysicsManager->GetBodyMass(lGeometry->GetBodyId());
	}
	return (lTotalMass);
}



bool ContextObject::SetStructure(TBC::ChunkyPhysics* pStructure)
{
	TBC::PhysicsManager* lPhysics = mManager->GetGameManager()->GetPhysicsManager();
	const int lPhysicsFps = mManager->GetGameManager()->GetConstTimeManager()->GetDesiredPhysicsFps();

	// TODO: drop hard-coding, this should come from world loader or spawn engine?
	Lepra::TransformationF lTransformation;
	if (GetNetworkObjectType() == NETWORK_OBJECT_REMOTE_CONTROLLED)
	{
		const float lX = (float)Lepra::Random::Uniform(-200, 200);
		const float lY = (float)Lepra::Random::Uniform(-200, 200);
		lTransformation.SetPosition(Lepra::Vector3DF(lX, lY, 250+3));
	}

	bool lOk = (mStructure == 0 && pStructure->FinalizeInit(lPhysics, lPhysicsFps, &lTransformation, 0, this));
	assert(lOk);
	if (lOk)
	{
		mStructure = pStructure;
		const int lBoneCount = mStructure->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			const TBC::ChunkyBoneGeometry* lGeometry = mStructure->GetBoneGeometry(x);
			mManager->AddPhysicsBody(this, lGeometry->GetBodyId());
		}
	}
	return (lOk);
}

TBC::ChunkyBoneGeometry* ContextObject::GetStructureGeometry(unsigned pIndex) const
{
	return (mStructure->GetBoneGeometry(pIndex));
}

TBC::ChunkyBoneGeometry* ContextObject::GetStructureGeometry(TBC::PhysicsManager::BodyID pBodyId) const
{
	return (mStructure->GetBoneGeometry(pBodyId));
}

void ContextObject::SetEnginePower(unsigned pAspect, float pPower, float pAngle)
{
	mStructure->SetEnginePower(pAspect, pPower, pAngle);
}



bool ContextObject::QueryResendTime(float pDeltaTime, bool pUnblockDelta)
{
	bool lOkToSend = false;
	const float lAbsoluteTime = GetManager()->GetGameManager()->GetConstTimeManager()->GetAbsoluteTime();
	if (mLastSendTime+pDeltaTime <= lAbsoluteTime)
	{
		lOkToSend = true;
		mLastSendTime = lAbsoluteTime - (pUnblockDelta? pDeltaTime+Lepra::MathTraits<float>::FullEps() : 0);
	}
	return (lOkToSend);
}

int ContextObject::PopSendCount()
{
	if (mSendCount > 0)
	{
		--mSendCount;
	}
	return (mSendCount);
}

void ContextObject::SetSendCount(int pCount)
{
	mSendCount = pCount;
}



void ContextObject::OnPhysicsTick()
{
}



void ContextObject::AttachToObject(TBC::ChunkyBoneGeometry* pBoneGeometry1, ContextObject* pObject2, TBC::ChunkyBoneGeometry* pBoneGeometry2, bool pSend)
{
	assert(pObject2);
	assert(!IsAttachedTo(pObject2));
	if (!pObject2 || !pBoneGeometry1 || !pBoneGeometry2)
	{
		return;
	}

	pSend;	// TODO: remove when new structure in place!

	TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	if (pBoneGeometry1->IsConnectorType(TBC::ChunkyBoneGeometry::CONNECTOR_3DOF) &&
		pBoneGeometry2->IsConnectorType(TBC::ChunkyBoneGeometry::CONNECTEE_3DOF))
	{
		pObject2->SetAllowMoveSelf(false);

		// Find first parent that is a dynamic body.
		TBC::PhysicsManager::BodyID lBody2Connectee = pBoneGeometry2->GetBodyId();
		TBC::ChunkyBoneGeometry* lNode2Connectee = pBoneGeometry2;
		while (lPhysicsManager->IsStaticBody(lBody2Connectee))
		{
			lNode2Connectee = lNode2Connectee->GetParent();
			if (!lNode2Connectee)
			{
				mLog.AError("Failing to attach to a static object.");
				return;
			}
			lBody2Connectee = lNode2Connectee->GetBodyId();
		}

		mLog.AInfo("Attaching two objects.");
		// TODO: fix algo when new chunky structure in place.
		/*Lepra::TransformationF lAnchor;
		lPhysicsManager->GetBodyTransform(pBoneGeometry2->GetBodyId(), lAnchor);
		TBC::PhysicsManager::JointID lJoint = lPhysicsManager->CreateBallJoint(pBoneGeometry1->GetBodyId(), lBody2Connectee, lAnchor.GetPosition());
		unsigned lAttachNodeId = mStructure->GetNextGeometryIndex();
		AddPhysicsObject(PhysicsNode(pBoneGeometry1->GetId(), lAttachNodeId, TBC::INVALID_BODY, PhysicsNode::TYPE_EXCLUDE, lJoint));
		TBC::PhysicsEngine* lEngine = new TBC::PhysicsEngine(this, TBC::PhysicsEngine::ENGINE_GLUE, 0, 0, 0, 0);
		lEngine->AddControlledNode(lAttachNodeId, 1);
		AddAttachment(pObject2, lJoint, lEngine);
		pObject2->AddAttachment(this, TBC::INVALID_JOINT, 0);

		if (pSend)
		{
			mManager->GetGameManager()->SendAttach(this, pBoneGeometry1->GetId(), pObject2, pBoneGeometry2->GetId());
		}*/
	}
}

bool ContextObject::IsAttachedTo(ContextObject* pObject) const
{
	ConnectionList::const_iterator x = mConnectionList.begin();
	for (; x != mConnectionList.end(); ++x)
	{
		if (pObject == x->mObject)
		{
			return (true);	// TRICKY: RAII.
		}
	}
	return (false);
}

void ContextObject::AddAttachment(ContextObject* pObject, TBC::PhysicsManager::JointID pJoint, TBC::PhysicsEngine* pEngine)
{
	assert(!IsAttachedTo(pObject));
	mConnectionList.push_back(Connection(pObject, pJoint, pEngine));
	if (pEngine)
	{
		mStructure->AddEngine(pEngine);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ContextObject);



}
