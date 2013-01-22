
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
	assert(!pClassId.empty());
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
					TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(x);
					if (lGeometry->GetJointId() == lJointId)
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
		return false;
	}
	TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (!lGeometry || (lGeometry->GetBodyId() == TBC::INVALID_BODY && lGeometry->GetTriggerId() == TBC::INVALID_TRIGGER))
	{
		mLog.Errorf(_T("Could not get positional update (for streaming), since %i/%s not loaded yet!"),
			GetInstanceId(), GetClassId().c_str());
		return false;
	}

	TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
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
		// TODO: store movement until loaded. Movement of this object
		// was received from server before we were ready.
		return;
	}

	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (!lGeometry || (lGeometry->GetBodyId() == TBC::INVALID_BODY && lGeometry->GetBodyId() == TBC::INVALID_TRIGGER))
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
		//	mLog.Infof(_T("Positional diff is %f."), lScaledDiff);
		//}
	}

	ForceSetFullPosition(pPositionalData);
}

void ContextObject::SetInitialTransform(const TransformationF& pTransformation)
{
	//const QuaternionF& q = pTransformation.GetOrientation();
	//mLog.Infof(_T("Setting initial quaternion (%f;%f;%f;%f) for class %s."), q.GetA(), q.GetB(), q.GetC(), q.GetD(), GetClassId().c_str());
	mPosition.mPosition.mTransformation = pTransformation;
}

TransformationF ContextObject::GetInitialTransform() const
{
	return TransformationF(GetOrientation(), GetPosition());
}

void ContextObject::SetInitialPositionalData(const ObjectPositionalData& pPositionalData)
{
	//const QuaternionF& q = pPositionalData.mPosition.mTransformation.GetOrientation();
	//mLog.Infof(_T("Setting initial quaternion/pos (%f;%f;%f;%f) for class %s."), q.GetA(), q.GetB(), q.GetC(), q.GetD(), GetClassId().c_str());
	mPosition.CopyData(&pPositionalData);
}

Vector3DF ContextObject::GetPosition() const
{
	if (mPhysics && mPhysicsOverride != PHYSICS_OVERRIDE_BONES)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		if (lGeometry)
		{
			TBC::PhysicsManager::BodyID lBodyId = lGeometry->GetBodyId();
			if (!lBodyId)
			{
				lBodyId = lGeometry->GetTriggerId();
			}
			return (mManager->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lBodyId));
		}
		//assert(false);
	}
	return mPosition.mPosition.mTransformation.GetPosition();
}

void ContextObject::SetRootPosition(const Vector3DF& pPosition)
{
	//assert(mPhysicsOverride == PHYSICS_OVERRIDE_BONES);
	mPosition.mPosition.mTransformation.SetPosition(pPosition);

	if (mPhysics && mPhysics->GetBoneCount() > 0)
	{
		mPhysics->GetBoneTransformation(0).SetPosition(pPosition);
	}
}

void ContextObject::SetRootOrientation(const QuaternionF& pOrientation)
{
	//assert(mPhysicsOverride == PHYSICS_OVERRIDE_BONES);
	mPosition.mPosition.mTransformation.SetOrientation(pOrientation);

	if (mPhysics && mPhysics->GetBoneCount() > 0)
	{
		mPhysics->GetBoneTransformation(0).SetOrientation(pOrientation);
	}
}

void ContextObject::SetRootVelocity(const Vector3DF& pVelocity)
{
	mPosition.mPosition.mVelocity = pVelocity;
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
	if (mPhysics)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
		{
			Vector3DF lVelocity;
			mManager->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
			return lVelocity;
		}
	}
	return mPosition.mPosition.mVelocity;
}

Vector3DF ContextObject::GetAngularVelocity() const
{
	if (mPhysics)
	{
		Vector3DF lAngularVelocity;
		const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
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

Vector3DF ContextObject::GetAcceleration() const
{
	Vector3DF lAcceleration;
	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
	{
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyForce(lGeometry->GetBodyId(), lAcceleration);
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

void ContextObject::SetPhysics(TBC::ChunkyPhysics* pStructure)
{
	mPhysics = pStructure;
	if (mPhysicsOverride == PHYSICS_OVERRIDE_BONES)
	{
		const int lBoneCount = mPhysics->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(x);
			mManager->AddPhysicsBody(this, lGeometry->GetBodyId());
		}
	}
}

void ContextObject::ClearPhysics()
{
	// Removes bodies from manager, then destroys all physical stuff.
	if (mManager && mPhysics && mPhysicsOverride != PHYSICS_OVERRIDE_BONES)
	{
		const int lBoneCount = mPhysics->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(x);
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

bool ContextObject::SetEnginePower(unsigned pAspect, float pPower)
{
	return mPhysics->SetEnginePower(pAspect, pPower);
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
	if (GetPhysics() && GetManager())
	{
		OnTick();

		// Calculate total mass.
		assert(mTotalMass == 0);
		TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
		mTotalMass = mPhysics->QueryTotalMass(lPhysicsManager);
		PositionHauler::Set(mPosition, lPhysicsManager, mPhysics, mTotalMass, mAllowMoveRoot);

		GetManager()->EnableTickCallback(this);
	}
}

void ContextObject::OnTick()
{
}



void ContextObject::ForceSetFullPosition(const ObjectPositionalData& pPositionalData)
{
	mPosition.CopyData(&pPositionalData);
	assert(mTotalMass != 0);
	PositionHauler::Set(mPosition, mManager->GetGameManager()->GetPhysicsManager(), mPhysics, mTotalMass, mAllowMoveRoot);
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
		pObject2->SetAllowMoveRoot(false);

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



ResourceManager* ContextObject::GetResourceManager() const
{
	return (mResourceManager);
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ContextObject);



}
