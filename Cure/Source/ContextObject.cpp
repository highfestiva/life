
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ContextObject.h"
#include <algorithm>
#include <assert.h>
#include <math.h>
#include "../../Lepra/Include/HashUtil.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/RotationMatrix.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../TBC/Include/PhysicsSpawner.h"
#include "../../TBC/Include/PhysicsTrigger.h"
#include "../Include/ContextManager.h"
#include "../Include/ContextObjectAttribute.h"
#include "../Include/Cure.h"
#include "../Include/FloatAttribute.h"
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



ContextObject::ContextObject(Cure::ResourceManager* pResourceManager, const str& pClassId):
	mManager(0),
	mResourceManager(pResourceManager),
	mInstanceId(0),
	mOwnerInstanceId(0),
	mClassId(pClassId),
	mNetworkObjectType(NETWORK_OBJECT_LOCAL_ONLY),
	mParent(0),
	mExtraData(0),
	mSpawner(0),
	mIsLoaded(false),
	mPhysics(0),
	mPhysicsOverride(PHYSICS_OVERRIDE_NORMAL),
	mTotalMass(0),
	mLastSendTime(0),
	mNetworkOutputGhost(0),
	mSendCount(0),
	mAllowMoveSelf(true)
{
}

ContextObject::~ContextObject()
{
	log_volatile(mLog.Tracef(_T("Destructing context object %s."), mClassId.c_str()));

	DeleteNetworkOutputGhost();

	if (mParent)
	{
		mParent->RemoveChild(this);
		mParent = 0;
	}

	for (ChildList::iterator x = mChildList.begin(); x != mChildList.end(); ++x)
	{
		(*x)->SetParent(0);
		delete (*x);
	}
	mChildList.clear();

	mTriggerMap.clear();
	mSpawner = 0;

	if (mManager)
	{
		mManager->RemoveObject(this);
	}

	// Detach from other context objects.
	{
		while (!mConnectionList.empty())
		{
			ContextObject* lObject2 = mConnectionList.front().mObject;
			DetachFromObject(lObject2);
		}
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

	if (mManager)
	{
		mManager->FreeGameObjectId(mNetworkObjectType, mInstanceId);
	}
	mInstanceId = 0;
}



ContextManager* ContextObject::GetManager() const
{
	return (mManager);
}

void ContextObject::SetManager(ContextManager* pManager)
{
	assert(mManager == 0 || pManager == 0);
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

const str& ContextObject::GetClassId() const
{
	return (mClassId);
}

GameObjectId ContextObject::GetOwnerInstanceId() const
{
	return (mOwnerInstanceId);
}

void ContextObject::SetOwnerInstanceId(GameObjectId pInstanceId)
{
	mOwnerInstanceId = pInstanceId;
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



void* ContextObject::GetExtraData() const
{
	return (mExtraData);
}

void ContextObject::SetExtraData(void* pData)
{
	mExtraData = pData;
}



bool ContextObject::IsLoaded() const
{
	return (mIsLoaded);
}

void ContextObject::SetLoadResult(bool pOk)
{
	assert(!mIsLoaded);
	mIsLoaded = true;
	if (GetManager())
	{
		GetManager()->GetGameManager()->OnLoadCompleted(this, pOk);
	}
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
	AttachToObject(mPhysics->GetBoneGeometry(pBody1Index), pObject2, pObject2->GetStructureGeometry(pBody2Index), false);
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
				const int lBoneCount = mPhysics->GetBoneCount();
				for (int x = 0; x < lBoneCount; ++x)
				{
					TBC::ChunkyBoneGeometry* lStructureGeometry = mPhysics->GetBoneGeometry(x);
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

void ContextObject::DeleteAttribute(const str& pName)
{
	AttributeArray::iterator x = mAttributeArray.begin();
	while (x != mAttributeArray.end())
	{
		if ((*x)->GetName() == pName)
		{
			delete (*x);
			x = mAttributeArray.erase(x);
		}
		else
		{
			++x;
		}
	}
}

ContextObjectAttribute* ContextObject::GetAttribute(const str& pName) const
{
	AttributeArray::const_iterator x = mAttributeArray.begin();
	for (; x != mAttributeArray.end(); ++x)
	{
		if ((*x)->GetName() == pName)
		{
			return *x;
		}
	}
	return 0;
}

const ContextObject::AttributeArray& ContextObject::GetAttributes() const
{
	return mAttributeArray;
}

float ContextObject::GetAttributeFloatValue(const str& pAttributeName) const
{
	const FloatAttribute* lFloatAttribute = (const FloatAttribute*)GetAttribute(pAttributeName);
	if (!lFloatAttribute)
	{
		return 0;
	}
	return lFloatAttribute->GetValue();
}

bool ContextObject::IsAttributeTrue(const str& pAttributeName) const
{
	return (GetAttributeFloatValue(pAttributeName) > 0.5f);
}

void ContextObject::OnAttributeUpdated(ContextObjectAttribute*)
{
	mManager->AddAttributeSenderObject(this);
}



void ContextObject::AddTrigger(TBC::PhysicsManager::TriggerID pTriggerId, const void* pTrigger)
{
	mTriggerMap.insert(TriggerMap::value_type(pTriggerId, pTrigger));
}

void ContextObject::FinalizeTrigger(const TBC::PhysicsTrigger*)
{
}

const void* ContextObject::GetTrigger(TBC::PhysicsManager::TriggerID pTriggerId) const
{
	return HashUtil::FindMapObject(mTriggerMap, pTriggerId);
}

size_t ContextObject::GetTriggerCount(const void*& pTrigger) const
{
	if (mTriggerMap.empty())
	{
		return (0);
	}
	pTrigger = mTriggerMap.begin()->second;
	return (mTriggerMap.size());
}



void ContextObject::SetSpawner(const TBC::PhysicsSpawner* pSpawner)
{
	mSpawner = pSpawner;
}

const TBC::PhysicsSpawner* ContextObject::GetSpawner() const
{
	return mSpawner;
}



void ContextObject::AddChild(ContextObject* pChild)
{
	assert(pChild->GetInstanceId() != 0);
	if (std::find(mChildList.begin(), mChildList.end(), pChild) != mChildList.end())
	{
		// Already added.
		return;
	}
	mChildList.push_back(pChild);
	pChild->SetParent(this);
}



bool ContextObject::UpdateFullPosition(const ObjectPositionalData*& pPositionalData)
{
	if (!mPhysics)
	{
		return (false);
	}

	TBC::ChunkyBoneGeometry* lStructureGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
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

	const int lGeometryCount = mPhysics->GetBoneCount();
	size_t y = 0;
	for (int x = 0; x < lGeometryCount; ++x)
	{
		// TODO: add support for parent ID??? (JB 2009-07-07: Don't know anymore what this comment might mean.)
		// ??? Could it be when connected to something else, like a car connected to a crane?
		const TBC::ChunkyBoneGeometry* lStructureGeometry = mPhysics->GetBoneGeometry(x);
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
			case TBC::ChunkyBoneGeometry::JOINT_SLIDER:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData1, lData, PositionalData::TYPE_POSITION_1, 1);
				++y;
				TBC::PhysicsManager::Joint1Diff lDiff;
				if (!lPhysicsManager->GetJoint1Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get hinge!");
					return (false);
				}
				lData->mTransformation = lDiff.mValue;
				lData->mVelocity = lDiff.mVelocity;
				lData->mAcceleration = lDiff.mAcceleration;
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

	const int lEngineCount = mPhysics->GetEngineCount();
	for (int z = 0; z != lEngineCount; ++z)
	{
		// TODO: add support for parent ID??????????? JB 2009-07-08: don't know what this is anymore.
		const TBC::PhysicsEngine* lEngine = mPhysics->GetEngine(z);
		switch (lEngine->GetEngineType())
		{
			case TBC::PhysicsEngine::ENGINE_CAMERA_FLAT_PUSH:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, RealData4, lData, PositionalData::TYPE_REAL_4, 100);
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
			case TBC::PhysicsEngine::ENGINE_SLIDER_FORCE:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, RealData1, lData, PositionalData::TYPE_REAL_1, 100);
				++y;
				lData->mValue = lEngine->GetValue();
				assert(lData->mValue >= -1 && lData->mValue <= 1);
			}
			break;
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

	return (true);
}

void ContextObject::SetFullPosition(const ObjectPositionalData& pPositionalData)
{
	if (!IsLoaded())
	{
		// TODO: store movement until loaded. Movement of this object
		// was received from server before we were ready.
		return;
	}

	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
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

	ForceSetFullPosition(pPositionalData, lGeometry);
}

void ContextObject::SetInitialTransform(const TransformationF& pTransformation)
{
	mPosition.mPosition.mTransformation = pTransformation;
}

TransformationF ContextObject::GetInitialTransform() const
{
	return TransformationF(GetOrientation(), GetPosition());
}

Vector3DF ContextObject::GetPosition() const
{
	if (mPhysics && mPhysicsOverride != PHYSICS_OVERRIDE_BONES)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
		{
			return (mManager->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lGeometry->GetBodyId()));
		}
		//assert(false);
	}
	return mPosition.mPosition.mTransformation.GetPosition();
}

void ContextObject::SetRootPosition(const Vector3DF& pPosition)
{
	assert(mPhysicsOverride == PHYSICS_OVERRIDE_BONES);
	mPosition.mPosition.mTransformation.SetPosition(pPosition);

	if (mPhysics && mPhysics->GetBoneCount() > 0)
	{
		mPhysics->GetBoneTransformation(0).SetPosition(pPosition);
	}
}

void ContextObject::SetRootOrientation(const QuaternionF& pOrientation)
{
	assert(mPhysicsOverride == PHYSICS_OVERRIDE_BONES);
	mPosition.mPosition.mTransformation.SetOrientation(pOrientation);

	if (mPhysics && mPhysics->GetBoneCount() > 0)
	{
		mPhysics->GetBoneTransformation(0).SetOrientation(pOrientation);
	}
}

QuaternionF ContextObject::GetOrientation() const
{
	if (mPhysics)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
		{
			return mManager->GetGameManager()->GetPhysicsManager()->GetBodyOrientation(lGeometry->GetBodyId()) *
				mPhysics->GetOriginalBoneTransformation(0).GetOrientation();
		}
		//assert(false);
	}
	return (mPosition.mPosition.mTransformation.GetOrientation());
}

Vector3DF ContextObject::GetVelocity() const
{
	Vector3DF lVelocity;
	if (mPhysics)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
		{
			mManager->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
		}
	}
	return (lVelocity);
}

Vector3DF ContextObject::GetAngularVelocity() const
{
	Vector3DF lAngularVelocity;
	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
	{
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyAngularVelocity(lGeometry->GetBodyId(), lAngularVelocity);
	}
	return lAngularVelocity;
}

Vector3DF ContextObject::GetAcceleration() const
{
	Vector3DF lAcceleration;
	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
	{
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyAcceleration(lGeometry->GetBodyId(), lAcceleration);
	}
	return lAcceleration;
}

Vector3DF ContextObject::GetForwardDirection() const
{
	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
	{
		const TransformationF& lOriginalTransform =
			mPhysics->GetOriginalBoneTransformation(0);
		const Vector3DF lForwardAxis = lOriginalTransform.GetOrientation().GetConjugate() * Vector3DF(0, 1, 0);	// Assumes original quaternion normalized.
		TransformationF lTransform;
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lGeometry->GetBodyId(), lTransform);
		return (lTransform.GetOrientation() * lForwardAxis);
	}
	return Vector3DF(0, 1, 0);
}

float ContextObject::GetForwardSpeed() const
{
	float lSpeed = 0;
	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
	{
		Vector3DF lVelocity;
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
		lSpeed = lVelocity * GetForwardDirection();
	}
	/*else
	{
		assert(false);
	}*/
	return (lSpeed);
}

float ContextObject::GetMass() const
{
	return mTotalMass;
}

ObjectPositionalData* ContextObject::GetNetworkOutputGhost()
{
	if (!mNetworkOutputGhost)
	{
		mNetworkOutputGhost = new ObjectPositionalData;
	}
	return mNetworkOutputGhost;
}

void ContextObject::DeleteNetworkOutputGhost()
{
	delete mNetworkOutputGhost;
	mNetworkOutputGhost = 0;
}

bool ContextObject::SetPhysics(TBC::ChunkyPhysics* pStructure)
{
	const TransformationF& lTransformation = mPosition.mPosition.mTransformation;
	if (mPhysicsOverride == PHYSICS_OVERRIDE_BONES)
	{
		bool lOk = pStructure->FinalizeInit(0, 0, &lTransformation.GetPosition(), 0, 0);
		if (lOk)
		{
			mPhysics = pStructure;
		}
		return (lOk);
	}
	else if (mPhysicsOverride == PHYSICS_OVERRIDE_STATIC)
	{
		pStructure->SetPhysicsType(TBC::ChunkyPhysics::STATIC);
	}

	TBC::PhysicsManager* lPhysics = mManager->GetGameManager()->GetPhysicsManager();
	const int lPhysicsFps = mManager->GetGameManager()->GetTimeManager()->GetDesiredMicroSteps();
	bool lOk = (mPhysics == 0 && pStructure->FinalizeInit(lPhysics, lPhysicsFps, &lTransformation.GetPosition(), this, this));
	assert(lOk);
	if (lOk)
	{
		mPhysics = pStructure;
		const int lBoneCount = mPhysics->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(x);
			mManager->AddPhysicsBody(this, lGeometry->GetBodyId());
		}
	}

	// Set orienation (as given in initial transform). The orientation in initial transform
	// is relative to the initial root bone orientation.
	if (lOk)
	{
		if (mPhysics->GetPhysicsType() != TBC::ChunkyPhysics::STATIC)
		{
			const QuaternionF lPhysOrientation(lPhysics->GetBodyOrientation(mPhysics->GetBoneGeometry(0)->GetBodyId()));
			const Cure::ObjectPositionalData* lPlacement;
			lOk = UpdateFullPosition(lPlacement);
			assert(lOk);
			if (lOk)
			{
				Cure::ObjectPositionalData* lNewPlacement = (Cure::ObjectPositionalData*)lPlacement->Clone();
				lNewPlacement->mPosition.mTransformation =
					TransformationF(lTransformation.GetOrientation() * lPhysOrientation,
						lTransformation.GetPosition());
				ForceSetFullPosition(*lNewPlacement, mPhysics->GetBoneGeometry(mPhysics->GetRootBone()));
				delete lNewPlacement;
			}
		}
	}
	return (lOk);
}

void ContextObject::ClearPhysics()
{
	// Removes bodies from manager, then destroys all physical stuff.
	if (mManager && mPhysics && mPhysicsOverride != PHYSICS_OVERRIDE_BONES)
	{
		const int lBoneCount = mPhysics->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			TBC::ChunkyBoneGeometry* lStructureGeometry = mPhysics->GetBoneGeometry(x);
			mManager->RemovePhysicsBody(lStructureGeometry->GetBodyId());
		}
		mPhysics->ClearAll(mManager->GetGameManager()->GetPhysicsManager());
		mPhysics = 0;
	}
}

TBC::ChunkyPhysics* ContextObject::GetPhysics() const
{
	return (mPhysics);
}

void ContextObject::SetPhysicsTypeOverride(PhysicsOverride pPhysicsOverride)
{
	mPhysicsOverride = pPhysicsOverride;
}

TBC::ChunkyBoneGeometry* ContextObject::GetStructureGeometry(unsigned pIndex) const
{
	return (mPhysics->GetBoneGeometry(pIndex));
}

TBC::ChunkyBoneGeometry* ContextObject::GetStructureGeometry(TBC::PhysicsManager::BodyID pBodyId) const
{
	return (mPhysics->GetBoneGeometry(pBodyId));
}

bool ContextObject::SetEnginePower(unsigned pAspect, float pPower, float pAngle)
{
	return mPhysics->SetEnginePower(pAspect, pPower, pAngle);
}

float ContextObject::GetImpact(const Vector3DF& pGravity, const Vector3DF& pForce, const Vector3DF& pTorque, float pExtraMass, float pSidewaysFactor) const
{
	const float lMassFactor = 1/(GetMass() + pExtraMass);
	const float lGravityInvertFactor = 1/pGravity.GetLength();
	const Vector3DF lGravityDirection(pGravity * lGravityInvertFactor);
	// High angle against direction of gravity means high impact.
	const float lOpposingGravityFactor = -(pForce*lGravityDirection) * lGravityInvertFactor * lMassFactor;
	const float lSidewaysFactor = pForce.Cross(lGravityDirection).GetLength() * lMassFactor;
	const float lTorqueFactor = pTorque.GetLength() * lMassFactor;
	float lImpact = 0;
	lImpact = std::max(lImpact, lOpposingGravityFactor * 0.2f);
	lImpact = std::max(lImpact, lOpposingGravityFactor * -0.8f);
	lImpact = std::max(lImpact, lSidewaysFactor * pSidewaysFactor * 0.01f);
	lImpact = std::max(lImpact, lTorqueFactor * 0.03f);
	if (lImpact >= 1.0f)
	{
		log_volatile(mLog.Tracef(_T("Collided hard with something dynamic.")));
	}
	return (lImpact);
}



void ContextObject::ForceSend()
{
	GetManager()->AddPhysicsSenderObject(this);	// Put us in send list.
	mLastSendTime -= 10;	// Make sure we send immediately.
}

bool ContextObject::QueryResendTime(float pDeltaTime, bool pUnblockDelta)
{
	bool lOkToSend = false;
	const float lAbsoluteTime = GetManager()->GetGameManager()->GetTimeManager()->GetAbsoluteTime();
	if (pDeltaTime <= Cure::TimeManager::GetAbsoluteTimeDiff(lAbsoluteTime, mLastSendTime))
	{
		lOkToSend = true;
		mLastSendTime = lAbsoluteTime - (pUnblockDelta? pDeltaTime+MathTraits<float>::FullEps() : 0);
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



void ContextObject::OnLoaded()
{
	OnTick();
	if (GetPhysics() && GetManager())
	{
		// Calculate total mass.
		assert(mTotalMass == 0);
		TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
		const int lBoneCount = mPhysics->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(x);
			if (lGeometry->GetBodyId())
			{
				mTotalMass += lPhysicsManager->GetBodyMass(lGeometry->GetBodyId());
			}
		}

		GetManager()->EnableTickCallback(this);
	}
}

void ContextObject::OnTick()
{
}



void ContextObject::ForceSetFullPosition(const ObjectPositionalData& pPositionalData, const TBC::ChunkyBoneGeometry* pGeometry)
{
	mPosition.CopyData(&pPositionalData);

	TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	if (mAllowMoveSelf)
	{
		if (mPhysics->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC)
		{
			TBC::PhysicsManager::BodyID lBody = pGeometry->GetBodyId();
			lPhysicsManager->SetBodyTransform(lBody, pPositionalData.mPosition.mTransformation);
			lPhysicsManager->SetBodyVelocity(lBody, pPositionalData.mPosition.mVelocity);
			lPhysicsManager->SetBodyAcceleration(lBody, pPositionalData.mPosition.mAcceleration);
			lPhysicsManager->SetBodyAngularVelocity(lBody, pPositionalData.mPosition.mAngularVelocity);
			lPhysicsManager->SetBodyAngularAcceleration(lBody, pPositionalData.mPosition.mAngularAcceleration);
		}
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

	const int lBoneCount = mPhysics->GetBoneCount();
	size_t y = 0;
	for (int x = 0; x < lBoneCount && y < mPosition.mBodyPositionArray.size(); ++x)
	{
		// TODO: add support for parent ID.
		const TBC::ChunkyBoneGeometry* lStructureGeometry = mPhysics->GetBoneGeometry(x);
		TBC::PhysicsManager::BodyID lBody = lStructureGeometry->GetBodyId();
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
			case TBC::ChunkyBoneGeometry::JOINT_SLIDER:
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

	const int lEngineCount = mPhysics->GetEngineCount();
	for (int z = 0; z != lEngineCount; ++z)
	{
		// TODO: add support for parent ID??????????? JB 2009-07-08: don't know what this is anymore.
		const TBC::PhysicsEngine* lEngine = mPhysics->GetEngine(z);
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
				SetEnginePower(6, lData->mValue[2], lData->mValue[3]);	// TRICKY: specialcasing.
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
			case TBC::PhysicsEngine::ENGINE_SLIDER_FORCE:
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
		/*TransformationF lAnchor;
		lPhysicsManager->GetBodyTransform(pBoneGeometry2->GetBodyId(), lAnchor);
		TBC::PhysicsManager::JointID lJoint = lPhysicsManager->CreateBallJoint(pBoneGeometry1->GetBodyId(), lBody2Connectee, lAnchor.GetPosition());
		unsigned lAttachNodeId = mPhysics->GetNextGeometryIndex();
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
		mPhysics->AddEngine(pEngine);
	}
}



void ContextObject::RemoveChild(ContextObject* pChild)
{
	mChildList.remove(pChild);
}

void ContextObject::SetParent(ContextObject* pParent)
{
	mParent = pParent;
}

void ContextObject::SetupChildHandlers()
{
	const int lTriggerCount = mPhysics->GetTriggerCount();
	for (int x = 0; x < lTriggerCount; ++x)
	{
		const TBC::PhysicsTrigger* lTrigger = mPhysics->GetTrigger(x);
		ContextObject* lHandlerChild = GetManager()->GetGameManager()->CreateLogicHandler(lTrigger->GetFunction());
		if (!lHandlerChild)
		{
			continue;
		}
		AddChild(lHandlerChild);
		const int lBoneTriggerCount = lTrigger->GetTriggerGeometryCount();
		for (int x = 0; x < lBoneTriggerCount; ++x)
		{
			AddTrigger(lTrigger->GetPhysicsTriggerId(x), lHandlerChild);
			lHandlerChild->AddTrigger(lTrigger->GetPhysicsTriggerId(x), lTrigger);
		}
		if (lBoneTriggerCount == 0)
		{
			lHandlerChild->AddTrigger(TBC::INVALID_TRIGGER, lTrigger);
		}
		lHandlerChild->FinalizeTrigger(lTrigger);
	}

	const int lSpawnerCount = mPhysics->GetSpawnerCount();
	for (int x = 0; x < lSpawnerCount; ++x)
	{
		const TBC::PhysicsSpawner* lSpawner = mPhysics->GetSpawner(x);
		ContextObject* lHandlerChild = GetManager()->GetGameManager()->CreateLogicHandler(lSpawner->GetFunction());
		if (!lHandlerChild)
		{
			continue;
		}
		AddChild(lHandlerChild);
		lHandlerChild->SetSpawner(lSpawner);
	}
}



bool ContextObject::IsSameInstance(TBC::PhysicsManager::ForceFeedbackListener* pOther)
{
	if (!pOther)
	{
		return false;
	}
	return (((ContextObject*)pOther)->GetInstanceId() == GetInstanceId());
}



ResourceManager* ContextObject::GetResourceManager() const
{
	return (mResourceManager);
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ContextObject);



}
