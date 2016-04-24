
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/ContextObject.h"
#include <algorithm>
#include <math.h>
#include "../../Lepra/Include/HashUtil.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/ResourceTracker.h"
#include "../../Lepra/Include/RotationMatrix.h"
#include "../../Tbc/Include/ChunkyBoneGeometry.h"
#include "../../Tbc/Include/ChunkyPhysics.h"
#include "../../Tbc/Include/PhysicsEngine.h"
#include "../../Tbc/Include/PhysicsManager.h"
#include "../../Tbc/Include/PhysicsSpawner.h"
#include "../../Tbc/Include/PhysicsTrigger.h"
#include "../Include/ContextManager.h"
#include "../Include/ContextObjectAttribute.h"
#include "../Include/Cure.h"
#include "../Include/FloatAttribute.h"
#include "../Include/GameManager.h"
#include "../Include/PositionHauler.h"
#include "../Include/TimeManager.h"



namespace Cure
{



ContextObject::ContextObject(Cure::ResourceManager* pResourceManager, const str& pClassId):
	mManager(0),
	mResourceManager(pResourceManager),
	mInstanceId(0),
	mOwnerInstanceId(0),
	mBorrowerInstanceId(0),
	mClassId(pClassId),
	mNetworkObjectType(NETWORK_OBJECT_LOCAL_ONLY),
	mParent(0),
	mExtraData(0),
	mSpawner(0),
	mIsLoaded(false),
	mPhysics(0),
	mPhysicsOverride(PHYSICS_OVERRIDE_NORMAL),
	mTotalMass(0),
	mLastSendTime(-10000.0f),
	mNetworkOutputGhost(0),
	mSendCount(0),
	mAllowMoveRoot(true)
{
	deb_assert(!pClassId.empty());
	LEPRA_ACQUIRE_RESOURCE(ContextObject);
}

ContextObject::~ContextObject()
{
	deb_assert(Thread::GetCurrentThread()->GetThreadName() == "MainThread");

	log_volatile(mLog.Tracef("Destructing context object %s.", mClassId.c_str()));

	DeleteNetworkOutputGhost();

	if (mParent)
	{
		mParent->RemoveChild(this);
		mParent = 0;
	}

	for (Array::iterator x = mChildArray.begin(); x != mChildArray.end(); ++x)
	{
		(*x)->SetParent(0);
		delete (*x);
	}
	mChildArray.clear();

	mTriggerMap.clear();
	mSpawner = 0;

	deb_assert(mManager);
	if (mManager)
	{
		mManager->RemoveObject(this);
	}

	ClearPhysics();

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

	LEPRA_RELEASE_RESOURCE(ContextObject);
}



ContextManager* ContextObject::GetManager() const
{
	return (mManager);
}

void ContextObject::SetManager(ContextManager* pManager)
{
	deb_assert(mManager == 0 || pManager == 0);
	mManager = pManager;
}

GameObjectId ContextObject::GetInstanceId() const
{
	return (mInstanceId);
}

void ContextObject::SetInstanceId(GameObjectId pInstanceId)
{
	deb_assert(mInstanceId == 0);
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

GameObjectId ContextObject::GetBorrowerInstanceId() const
{
	return mBorrowerInstanceId;
}

void ContextObject::SetBorrowerInstanceId(GameObjectId pInstanceId)
{
	mBorrowerInstanceId = pInstanceId;
}



NetworkObjectType ContextObject::GetNetworkObjectType() const
{
	return (mNetworkObjectType);
}

void ContextObject::SetNetworkObjectType(NetworkObjectType pType)
{
	deb_assert((mNetworkObjectType == pType) ||
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
	deb_assert(!mIsLoaded);
	mIsLoaded = true;
	if (pOk)
	{
		OnLoaded();
	}
	if (GetManager())
	{
		GetManager()->GetGameManager()->OnLoadCompleted(this, pOk);
	}
}



void ContextObject::SetAllowMoveRoot(bool pAllow)
{
	mAllowMoveRoot = pAllow;
}

void ContextObject::AttachToObjectByBodyIds(Tbc::PhysicsManager::BodyID pBody1, ContextObject* pObject2, Tbc::PhysicsManager::BodyID pBody2)
{
	if (IsAttachedTo(pObject2))
	{
		return;
	}
	AttachToObject(GetStructureGeometry(pBody1), pObject2, pObject2->GetStructureGeometry(pBody2), true);
}

void ContextObject::AttachToObjectByBodyIndices(unsigned pBody1Index, ContextObject* pObject2, unsigned pBody2Index)
{
	if (IsAttachedTo(pObject2))
	{
		mLog.Warningf("Object %i already attached to object %i!", GetInstanceId(), pObject2->GetInstanceId());
		return;
	}
	AttachToObject(mPhysics->GetBoneGeometry(pBody1Index), pObject2, pObject2->GetStructureGeometry(pBody2Index), false);
}

void ContextObject::DetachAll()
{
	while (!mConnectionList.empty())
	{
		ContextObject* lObject2 = mConnectionList.front().mObject;
		DetachFromObject(lObject2);
	}
}

bool ContextObject::DetachFromObject(ContextObject* pObject)
{
	bool lRemoved = false;

	ConnectionList::iterator x = mConnectionList.begin();
	for (; x != mConnectionList.end(); ++x)
	{
		if (pObject == x->mObject)
		{
			Tbc::PhysicsManager::JointID lJointId = x->mJointId;
			EngineList lEngineList = x->mEngineList;
			mConnectionList.erase(x);
			pObject->DetachFromObject(this);
			if (lJointId != Tbc::INVALID_JOINT)
			{
				mManager->GetGameManager()->GetPhysicsManager()->DeleteJoint(lJointId);
				mManager->GetGameManager()->SendDetach(this, pObject);
			}
			const int lBoneCount = pObject->GetPhysics()->GetBoneCount();
			for (int x = 0; x < lBoneCount; ++x)
			{
				if (pObject->GetPhysics()->GetBoneGeometry(x)->GetJointId() == lJointId)
				{
					pObject->GetPhysics()->GetBoneGeometry(x)->GetBodyData().mParent = 0;
					pObject->GetPhysics()->GetBoneGeometry(x)->SetJointId(Tbc::INVALID_JOINT);
					pObject->GetPhysics()->GetBoneGeometry(x)->SetJointType(Tbc::ChunkyBoneGeometry::JOINT_EXCLUDE);
					break;
				}
			}
			EngineList::iterator x = lEngineList.begin();
			for (; x != lEngineList.end(); ++x)
			{
				Tbc::PhysicsEngine* lEngine = *x;
				lEngine->RemoveControlledGeometry(pObject->GetPhysics()->GetBoneGeometry(0));
			}
			lRemoved = true;
			break;
		}
	}
	return (lRemoved);
}

ContextObject::Array ContextObject::GetAttachedObjects() const
{
	Array lObjects;
	ConnectionList::const_iterator x = mConnectionList.begin();
	for (; x != mConnectionList.end(); ++x)
	{
		lObjects.push_back(x->mObject);
	}
	return lObjects;
}

void ContextObject::AddAttachedObjectEngine(ContextObject* pAttachedObject, Tbc::PhysicsEngine* pEngine)
{
	ConnectionList::iterator x = mConnectionList.begin();
	for (; x != mConnectionList.end(); ++x)
	{
		if (x->mObject == pAttachedObject)
		{
			x->mEngineList.push_back(pEngine);
		}
	}
}



void ContextObject::AddAttribute(ContextObjectAttribute* pAttribute)
{
	mAttributeArray.push_back(pAttribute);
	if (mManager)
	{
		mManager->AddAttributeSenderObject(this);
	}
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

void ContextObject::QuerySetChildishness(float pChildishness)
{
	const str lName = "float_childishness";
	Cure::FloatAttribute* lAttribute = (Cure::FloatAttribute*)GetAttribute(lName);
	if (!lAttribute)
	{
		lAttribute = new Cure::FloatAttribute(this, lName, 0);
	}
	lAttribute->SetValue(pChildishness);
}

bool ContextObject::IsAttributeTrue(const str& pAttributeName) const
{
	return (GetAttributeFloatValue(pAttributeName) > 0.5f);
}

void ContextObject::OnAttributeUpdated(ContextObjectAttribute*)
{
	if (mManager)
	{
		mManager->AddAttributeSenderObject(this);
	}
}



void ContextObject::AddTrigger(Tbc::PhysicsManager::BodyID pTriggerId, const void* pTrigger)
{
	mTriggerMap.insert(TriggerMap::value_type(pTriggerId, pTrigger));
}

void ContextObject::FinalizeTrigger(const Tbc::PhysicsTrigger*)
{
}

const void* ContextObject::GetTrigger(Tbc::PhysicsManager::BodyID pTriggerId) const
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



void ContextObject::SetSpawner(const Tbc::PhysicsSpawner* pSpawner)
{
	mSpawner = pSpawner;
}

const Tbc::PhysicsSpawner* ContextObject::GetSpawner() const
{
	return mSpawner;
}



void ContextObject::AddChild(ContextObject* pChild)
{
	deb_assert(pChild->GetInstanceId() != 0);
	if (std::find(mChildArray.begin(), mChildArray.end(), pChild) != mChildArray.end())
	{
		// Already added. This may for instance happen when another path for a level is added.
		return;
	}
	mChildArray.push_back(pChild);
	pChild->SetParent(this);
}

const ContextObject::Array& ContextObject::GetChildArray() const
{
	return mChildArray;
}



bool ContextObject::UpdateFullPosition(const ObjectPositionalData*& pPositionalData)
{
	if (!mPhysics)
	{
		return false;
	}
	Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (!lGeometry || lGeometry->GetBodyId() == Tbc::INVALID_BODY)
	{
		mLog.Errorf("Could not get positional update (for streaming), since %i/%s not loaded yet!",
			GetInstanceId(), GetClassId().c_str());
		return false;
	}

	Tbc::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	bool lOk = PositionHauler::Get(mPosition, lPhysicsManager, mPhysics, mTotalMass);
	if (lOk)
	{
		pPositionalData = &mPosition;
	}
	return lOk;
}

void ContextObject::SetFullPosition(const ObjectPositionalData& pPositionalData, float pDeltaThreshold)
{
	if (!IsLoaded())
	{
		// Movement of this object was received (from remote host) before we were ready.
		mPosition.CopyData(&pPositionalData);
		return;
	}

	const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (!lGeometry || lGeometry->GetBodyId() == Tbc::INVALID_BODY)
	{
		return;
	}

	if (mPosition.IsSameStructure(pPositionalData))
	{
		const bool lPositionOnly = (pDeltaThreshold != 0);	// If there is a threshold at all, it's always about positions, never about engines.
		const float lScaledDiff = mPosition.GetBiasedTypeDifference(&pPositionalData, lPositionOnly);
		if (lScaledDiff <= pDeltaThreshold)
		{
			return;
		}
		//if (pDeltaThreshold > 0)
		//{
		//	mLog.Infof("Positional diff is %f.", lScaledDiff);
		//}
	}

	ForceSetFullPosition(pPositionalData);
}

void ContextObject::SetPositionFinalized()
{
}

void ContextObject::SetInitialTransform(const xform& pTransformation)
{
	//const quat& q = pTransformation.GetOrientation();
	//mLog.Infof("Setting initial quaternion (%f;%f;%f;%f for class %s."), q.a, q.b, q.c, q.d, GetClassId().c_str());
	mPosition.mPosition.mTransformation = pTransformation;
}

xform ContextObject::GetInitialTransform() const
{
	return xform(GetOrientation(), GetPosition());
}

void ContextObject::SetInitialPositionalData(const ObjectPositionalData& pPositionalData)
{
	//const quat& q = pPositionalData.mPosition.mTransformation.GetOrientation();
	//mLog.Infof("Setting initial quaternion/pos (%f;%f;%f;%f for class %s."), q.a, q.b, q.c, q.d, GetClassId().c_str());
	mPosition.CopyData(&pPositionalData);
}

vec3 ContextObject::GetPosition() const
{
	if (mPhysics && mPhysicsOverride != PHYSICS_OVERRIDE_BONES)
	{
		const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		if (lGeometry)
		{
			Tbc::PhysicsManager::BodyID lBodyId = lGeometry->GetBodyId();
			return (mManager->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lBodyId));
		}
		//deb_assert(false);
	}
	return mPosition.mPosition.mTransformation.GetPosition();
}

void ContextObject::SetRootPosition(const vec3& pPosition)
{
	//deb_assert(mPhysicsOverride == PHYSICS_OVERRIDE_BONES);
	mPosition.mPosition.mTransformation.SetPosition(pPosition);

	if (mPhysics && mPhysics->GetBoneCount() > 0)
	{
		mPhysics->GetBoneTransformation(0).SetPosition(pPosition);
	}
}

vec3 ContextObject::GetRootPosition() const
{
	return mPosition.mPosition.mTransformation.GetPosition();
}

void ContextObject::SetRootOrientation(const quat& pOrientation)
{
	//deb_assert(mPhysicsOverride == PHYSICS_OVERRIDE_BONES);
	mPosition.mPosition.mTransformation.SetOrientation(pOrientation);

	if (mPhysics && mPhysics->GetBoneCount() > 0)
	{
		mPhysics->GetBoneTransformation(0).SetOrientation(pOrientation);
	}
}

void ContextObject::SetRootVelocity(const vec3& pVelocity)
{
	mPosition.mPosition.mVelocity = pVelocity;
}

quat ContextObject::GetOrientation() const
{
	if (mPhysics)
	{
		const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		if (lGeometry && lGeometry->GetBodyId() != Tbc::INVALID_BODY && lGeometry->GetBoneType() == Tbc::ChunkyBoneGeometry::BONE_BODY)
		{
			return mManager->GetGameManager()->GetPhysicsManager()->GetBodyOrientation(lGeometry->GetBodyId()) *
				mPhysics->GetOriginalBoneTransformation(0).GetOrientation();
		}
		//deb_assert(false);
	}
	return (mPosition.mPosition.mTransformation.GetOrientation());
}

vec3 ContextObject::GetVelocity() const
{
	if (mPhysics)
	{
		const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		if (lGeometry && lGeometry->GetBodyId() != Tbc::INVALID_BODY && lGeometry->GetBoneType() == Tbc::ChunkyBoneGeometry::BONE_BODY)
		{
			vec3 lVelocity;
			mManager->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
			return lVelocity;
		}
	}
	return mPosition.mPosition.mVelocity;
}

vec3 ContextObject::GetAngularVelocity() const
{
	if (mPhysics)
	{
		vec3 lAngularVelocity;
		const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		if (lGeometry && lGeometry->GetBodyId() != Tbc::INVALID_BODY && lGeometry->GetBoneType() == Tbc::ChunkyBoneGeometry::BONE_BODY)
		{
			mManager->GetGameManager()->GetPhysicsManager()->GetBodyAngularVelocity(lGeometry->GetBodyId(), lAngularVelocity);
		}
		return lAngularVelocity;
	}
	else
	{
		return mPosition.mPosition.mAngularVelocity;
	}
}

vec3 ContextObject::GetAcceleration() const
{
	vec3 lAcceleration;
	const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != Tbc::INVALID_BODY && lGeometry->GetBoneType() == Tbc::ChunkyBoneGeometry::BONE_BODY)
	{
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyForce(lGeometry->GetBodyId(), lAcceleration);
	}
	return lAcceleration;
}

vec3 ContextObject::GetForwardDirection() const
{
	const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != Tbc::INVALID_BODY && lGeometry->GetBoneType() == Tbc::ChunkyBoneGeometry::BONE_BODY)
	{
		const xform& lOriginalTransform =
			mPhysics->GetOriginalBoneTransformation(0);
		const vec3 lForwardAxis = lOriginalTransform.GetOrientation().GetConjugate() * vec3(0, 1, 0);	// Assumes original quaternion normalized.
		xform lTransform;
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lGeometry->GetBodyId(), lTransform);
		return (lTransform.GetOrientation() * lForwardAxis);
	}
	return vec3(0, 1, 0);
}

float ContextObject::GetForwardSpeed() const
{
	float lSpeed = 0;
	const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != Tbc::INVALID_BODY && lGeometry->GetBoneType() == Tbc::ChunkyBoneGeometry::BONE_BODY)
	{
		vec3 lVelocity;
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
		lSpeed = lVelocity * GetForwardDirection();
	}
	/*else
	{
		deb_assert(false);
	}*/
	return (lSpeed);
}

float ContextObject::GetMass() const
{
	return mTotalMass;
}

float ContextObject::QueryMass()
{
	Tbc::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	mTotalMass = mPhysics->QueryTotalMass(lPhysicsManager);
	return mTotalMass;
}

void ContextObject::SetMass(float pMass)
{
	mTotalMass = pMass;
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

void ContextObject::SetPhysics(Tbc::ChunkyPhysics* pStructure)
{
	mPhysics = pStructure;
	if (mPhysicsOverride == PHYSICS_OVERRIDE_BONES)
	{
		const int lBoneCount = mPhysics->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(x);
			mManager->AddPhysicsBody(this, lGeometry->GetBodyId());
		}
	}
}

void ContextObject::ClearPhysics()
{
	// Removes bodies from manager, then destroys all physical stuff.
	if (mManager && mPhysics && mPhysicsOverride != PHYSICS_OVERRIDE_BONES)
	{
		DetachAll();

		const int lBoneCount = mPhysics->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(x);
			if (lGeometry)
			{
				mManager->RemovePhysicsBody(lGeometry->GetBodyId());
			}
		}
		// Not a good idea for a shared resource:
		//mPhysics->ClearAll(mManager->GetGameManager()->GetPhysicsManager());
		mPhysics = 0;
	}
}

Tbc::ChunkyPhysics* ContextObject::GetPhysics() const
{
	return (mPhysics);
}

void ContextObject::SetPhysicsTypeOverride(PhysicsOverride pPhysicsOverride)
{
	mPhysicsOverride = pPhysicsOverride;
}

Tbc::ChunkyBoneGeometry* ContextObject::GetStructureGeometry(unsigned pIndex) const
{
	return (mPhysics->GetBoneGeometry(pIndex));
}

Tbc::ChunkyBoneGeometry* ContextObject::GetStructureGeometry(Tbc::PhysicsManager::BodyID pBodyId) const
{
	return (mPhysics->GetBoneGeometry(pBodyId));
}

bool ContextObject::SetEnginePower(unsigned pAspect, float pPower)
{
	return mPhysics->SetEnginePower(pAspect, pPower);
}

float ContextObject::GetImpact(const vec3& pGravity, const vec3& pForce, const vec3& pTorque, float pExtraMass, float pSidewaysFactor) const
{
	const float lMassFactor = 1 / (GetMass() + pExtraMass);
	const float lGravityInvertFactor = 1/pGravity.GetLength();
	const vec3 lGravityDirection(pGravity * lGravityInvertFactor);
	// High angle against direction of gravity means high impact.
	const float lOpposingGravityFactor = -(pForce*lGravityDirection) * lGravityInvertFactor * lMassFactor;
	const float lSidewaysFactor = pForce.Cross(lGravityDirection).GetLength() * lMassFactor;
	const float lTorqueFactor = pTorque.GetLength() * lMassFactor;
	float lImpact = 0;
	lImpact = std::max(lImpact, lOpposingGravityFactor * 0.1f);
	lImpact = std::max(lImpact, lOpposingGravityFactor * -0.8f);
	lImpact = std::max(lImpact, lSidewaysFactor * pSidewaysFactor * 0.01f);
	lImpact = std::max(lImpact, lTorqueFactor * 0.03f);
	if (lImpact >= 1.0f)
	{
		log_volatile(mLog.Tracef("Collided hard with something dynamic."));
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
	if (GetPhysics() && GetManager())
	{
		// Calculate total mass.
		deb_assert(mTotalMass == 0);
		QueryMass();

		OnTick();

		/*if (mPhysicsOverride != PHYSICS_OVERRIDE_BONES)
		{
			PositionHauler::Set(mPosition, lPhysicsManager, mPhysics, mTotalMass, mAllowMoveRoot);
		}*/

		GetManager()->EnableTickCallback(this);
	}
}

void ContextObject::OnTick()
{
}



void ContextObject::ForceSetFullPosition(const ObjectPositionalData& pPositionalData)
{
	mPosition.CopyData(&pPositionalData);
	deb_assert(mTotalMass != 0 || GetPhysics()->GetBoneGeometry(0)->GetBoneType() == Tbc::ChunkyBoneGeometry::BONE_TRIGGER);
	PositionHauler::Set(mPosition, mManager->GetGameManager()->GetPhysicsManager(), mPhysics, mTotalMass, mAllowMoveRoot);
}

void ContextObject::AttachToObject(Tbc::ChunkyBoneGeometry* pBoneGeometry1, ContextObject* pObject2, Tbc::ChunkyBoneGeometry* pBoneGeometry2, bool pSend)
{
	deb_assert(pObject2);
	deb_assert(!IsAttachedTo(pObject2));
	if (!pObject2 || !pBoneGeometry1 || !pBoneGeometry2)
	{
		return;
	}
	if (!pBoneGeometry2->IsConnectorType(Tbc::ChunkyBoneGeometry::CONNECTEE_3DOF))
	{
		return;
	}

	// Find first parent that is a dynamic body.
	Tbc::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	const int lPhysicsFps = mManager->GetGameManager()->GetTimeManager()->GetDesiredMicroSteps();
	Tbc::PhysicsManager::BodyID lBody2Connectee = pBoneGeometry2->GetBodyId();
	Tbc::ChunkyBoneGeometry* lNode2Connectee = pBoneGeometry2;
	while (lPhysicsManager->IsStaticBody(lBody2Connectee))
	{
		lNode2Connectee = lNode2Connectee->GetParent();
		if (!lNode2Connectee)
		{
			if (pBoneGeometry1->IsConnectorType(Tbc::ChunkyBoneGeometry::CONNECTOR_FIXED))
			{
				break;
			}
			mLog.Error("Failing to attach joint to a static object. Try reversing the attachment!");
			return;
		}
		lBody2Connectee = lNode2Connectee->GetBodyId();
		pBoneGeometry2 = lNode2Connectee;
	}

	pObject2->SetAllowMoveRoot(false);

	if (pBoneGeometry1->IsConnectorType(Tbc::ChunkyBoneGeometry::CONNECTOR_3DOF))
	{
		log_debug("Attaching two objects with ball joint.");
		pBoneGeometry2->SetJointType(Tbc::ChunkyBoneGeometry::JOINT_BALL);
	}
	else if (pBoneGeometry1->IsConnectorType(Tbc::ChunkyBoneGeometry::CONNECTOR_FIXED))
	{
		log_debug("Attaching two objects with fixed joint.");
		pBoneGeometry2->SetJointType(Tbc::ChunkyBoneGeometry::JOINT_FIXED);
	}
	else if (pBoneGeometry1->IsConnectorType(Tbc::ChunkyBoneGeometry::CONNECTOR_HINGE2))
	{
		log_debug("Attaching two objects with hinge-2 joint.");
		pBoneGeometry2->SetJointType(Tbc::ChunkyBoneGeometry::JOINT_HINGE2);
	}
	else if (pBoneGeometry1->IsConnectorType(Tbc::ChunkyBoneGeometry::CONNECTOR_SUSPEND_HINGE))
	{
		log_debug("Attaching two objects with suspend hinge joint.");
		pBoneGeometry2->SetJointType(Tbc::ChunkyBoneGeometry::JOINT_SUSPEND_HINGE);
	}
	else if (pBoneGeometry1->IsConnectorType(Tbc::ChunkyBoneGeometry::CONNECTOR_HINGE))
	{
		log_debug("Attaching two objects with hinge joint.");
		pBoneGeometry2->SetJointType(Tbc::ChunkyBoneGeometry::JOINT_HINGE);
	}
	else if (pBoneGeometry1->IsConnectorType(Tbc::ChunkyBoneGeometry::CONNECTOR_UNIVERSAL))
	{
		log_debug("Attaching two objects with universal joint.");
		pBoneGeometry2->SetJointType(Tbc::ChunkyBoneGeometry::JOINT_UNIVERSAL);
	}
	else if (pBoneGeometry1->IsConnectorType(Tbc::ChunkyBoneGeometry::CONNECTOR_SLIDER))
	{
		log_debug("Attaching two objects with slider joint.");
		pBoneGeometry2->SetJointType(Tbc::ChunkyBoneGeometry::JOINT_SLIDER);
	}
	else
	{
		mLog.Error("Could not find connection type to attach two objects with a joint.");
		return;
	}
	pBoneGeometry2->GetBodyData().mParent = pBoneGeometry1;
	pBoneGeometry2->CreateJoint(pObject2->GetPhysics(), lPhysicsManager, lPhysicsFps);

	AddAttachment(pObject2, pBoneGeometry2->GetJointId(), 0);
	pObject2->AddAttachment(this, Tbc::INVALID_JOINT, 0);

	if (pSend)
	{
		//mManager->GetGameManager()->SendAttach(this, pBoneGeometry1->GetId(), pObject2, pBoneGeometry2->GetId());
	}
}

bool ContextObject::IsAttachedTo(ContextObject* pObject) const
{
	ConnectionList::const_iterator x = mConnectionList.begin();
	for (; x != mConnectionList.end(); ++x)
	{
		if (pObject == x->mObject)
		{
			return (true);
		}
	}
	return (false);
}

void ContextObject::AddAttachment(ContextObject* pObject, Tbc::PhysicsManager::JointID pJoint, Tbc::PhysicsEngine* pEngine)
{
	deb_assert(!IsAttachedTo(pObject));
	mConnectionList.push_back(Connection(pObject, pJoint, pEngine));
	if (pEngine)
	{
		mPhysics->AddEngine(pEngine);
	}
}



void ContextObject::RemoveChild(ContextObject* pChild)
{
	mChildArray.erase(std::remove(mChildArray.begin(), mChildArray.end(), pChild), mChildArray.end());
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
		const Tbc::PhysicsTrigger* lTrigger = mPhysics->GetTrigger(x);
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
			lHandlerChild->AddTrigger(Tbc::INVALID_BODY, lTrigger);
		}
		lHandlerChild->FinalizeTrigger(lTrigger);
	}

	const int lSpawnerCount = mPhysics->GetSpawnerCount();
	for (int x = 0; x < lSpawnerCount; ++x)
	{
		const Tbc::PhysicsSpawner* lSpawner = mPhysics->GetSpawner(x);
		ContextObject* lHandlerChild = GetManager()->GetGameManager()->CreateLogicHandler(lSpawner->GetFunction());
		if (!lHandlerChild)
		{
			continue;
		}
		AddChild(lHandlerChild);
		lHandlerChild->SetSpawner(lSpawner);
	}
}



ResourceManager* ContextObject::GetResourceManager() const
{
	return (mResourceManager);
}



loginstance(GAME_CONTEXT, ContextObject);



}
