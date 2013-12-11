
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../TBC/Include/PhysicsManager.h"
#include "../../Lepra/Include/Unordered.h"
#include <list>
#include <vector>
#include "../../Lepra/Include/String.h"
#include "Cure.h"
#include "PositionalData.h"
#include "PositionHauler.h"
#include "PhysicsSharedResource.h"



namespace TBC
{
class ChunkyPhysics;
class ChunkyBoneGeometry;
class PhysicsEngine;
class PhysicsSpawner;
class PhysicsTrigger;
}



namespace Cure
{



class ContextManager;
class ContextObjectAttribute;
class ResourceManager;


class ContextObject
{
public:
	typedef std::vector<ContextObject*> Array;
	typedef std::vector<ContextObjectAttribute*> AttributeArray;

	ContextObject(ResourceManager* pResourceManager, const str& pClassId);
	virtual ~ContextObject();

	ContextManager* GetManager() const;
	virtual void SetManager(ContextManager* pManager);

	GameObjectId GetInstanceId() const;
	void SetInstanceId(GameObjectId pInstanceId);
	const str& GetClassId() const;
	GameObjectId GetOwnerInstanceId() const;
	void SetOwnerInstanceId(GameObjectId pInstanceId);
	GameObjectId GetBorrowerInstanceId() const;
	void SetBorrowerInstanceId(GameObjectId pInstanceId);

	NetworkObjectType GetNetworkObjectType() const;
	void SetNetworkObjectType(NetworkObjectType pType);

	void* GetExtraData() const;
	void SetExtraData(void* pData);

	bool IsLoaded() const;
	void SetLoadResult(bool pOk);

	void SetAllowMoveRoot(bool pAllow);
	void AttachToObject(TBC::PhysicsManager::BodyID pBody1, ContextObject* pObject2, TBC::PhysicsManager::BodyID pBody2);
	void AttachToObject(unsigned pBody1Index, ContextObject* pObject2, unsigned pBody2Index);
	bool DetachFromObject(ContextObject* pObject);

	void AddAttribute(ContextObjectAttribute* pAttribute);
	void DeleteAttribute(const str& pName);
	ContextObjectAttribute* GetAttribute(const str& pName) const;
	const AttributeArray& GetAttributes() const;
	float GetAttributeFloatValue(const str& pAttributeName) const;
	void QuerySetChildishness(float pChildishness);
	bool IsAttributeTrue(const str& pAttributeName) const;
	void OnAttributeUpdated(ContextObjectAttribute* pAttribute);

	void AddTrigger(TBC::PhysicsManager::TriggerID pTriggerId, const void*);
	virtual void FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger);
	const void* GetTrigger(TBC::PhysicsManager::TriggerID pTriggerId) const;
	size_t GetTriggerCount(const void*& pTrigger) const;

	virtual void SetSpawner(const TBC::PhysicsSpawner* pSpawner);
	const TBC::PhysicsSpawner* GetSpawner() const;

	void AddChild(ContextObject* pChild);
	const Array& GetChildArray() const;

	bool UpdateFullPosition(const ObjectPositionalData*& pPositionalData);	// Fetch full phys position (and update object graph as necessary).
	static bool UpdateFullPosition(ObjectPositionalData& pPosition, TBC::PhysicsManager* pPhysicsManager, TBC::ChunkyPhysics* pStructure);
	void SetFullPosition(const ObjectPositionalData& pPositionalData, float pDeltaThreshold);	// Sets full phys position if structure or significant difference seen.
	void SetInitialTransform(const TransformationF& pTransformation);
	TransformationF GetInitialTransform() const;
	void SetInitialPositionalData(const ObjectPositionalData& pPositionalData);
	Vector3DF GetPosition() const;
	void SetRootPosition(const Vector3DF& pPosition);
	Vector3DF GetRootPosition() const;
	void SetRootOrientation(const QuaternionF& pOrientation);
	void SetRootVelocity(const Vector3DF& pVelocity);
	QuaternionF GetOrientation() const;
	Vector3DF GetVelocity() const;
	Vector3DF GetAngularVelocity() const;
	Vector3DF GetAcceleration() const;
	Vector3DF GetForwardDirection() const;
	float GetForwardSpeed() const;
	float GetMass() const;
	ObjectPositionalData* GetNetworkOutputGhost();
	void DeleteNetworkOutputGhost();

	void SetPhysics(TBC::ChunkyPhysics* pStructure);
	void ClearPhysics();
	TBC::ChunkyPhysics* GetPhysics() const;
	void SetPhysicsTypeOverride(PhysicsOverride pPhysicsOverride);
	TBC::ChunkyBoneGeometry* GetStructureGeometry(unsigned pIndex) const;
	TBC::ChunkyBoneGeometry* GetStructureGeometry(TBC::PhysicsManager::BodyID pBodyId) const;
	bool SetEnginePower(unsigned pAspect, float pPower);
	float GetImpact(const Vector3DF& pGravity, const Vector3DF& pForce, const Vector3DF& pTorque, float pExtraMass = 0, float pSidewaysFactor = 1) const;

	void ForceSend();
	bool QueryResendTime(float pDeltaTime, bool pUnblockDelta);
	int PopSendCount();
	void SetSendCount(int pCount);

	virtual void StartLoading() = 0;
	virtual void OnLoaded();
	virtual void OnMicroTick(float pFrameTime) = 0;
	virtual void OnAlarm(int pAlarmId, void* pExtraData) = 0;
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal) = 0;
	virtual void OnForceApplied(ContextObject* pOtherObject,
		 TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		 const Vector3DF& pForce, const Vector3DF& pTorque,
		 const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity) = 0;
	virtual void OnTick();

protected:
	void ForceSetFullPosition(const ObjectPositionalData& pPositionalData);
	void AttachToObject(TBC::ChunkyBoneGeometry* pBoneGeometry1, ContextObject* pObject2, TBC::ChunkyBoneGeometry* pBoneGeometry2, bool pSend);
	bool IsAttachedTo(ContextObject* pObject) const;
	void AddAttachment(ContextObject* pObject, TBC::PhysicsManager::JointID pJoint, TBC::PhysicsEngine* pEngine);

	void RemoveChild(ContextObject* pChild);
	void SetParent(ContextObject* pParent);
	virtual void SetupChildHandlers();

	ResourceManager* GetResourceManager() const;

	struct Connection
	{
		Connection(ContextObject* pObject, TBC::PhysicsManager::JointID pJointId, TBC::PhysicsEngine* pEngine):
			mObject(pObject),
			mJointId(pJointId),
			mEngine(pEngine)
		{
		}
		ContextObject* mObject;
		TBC::PhysicsManager::JointID mJointId;
		TBC::PhysicsEngine* mEngine;
	};
	typedef std::list<Connection> ConnectionList;
	typedef std::unordered_map<TBC::PhysicsManager::TriggerID, const void*> TriggerMap;

	ContextManager* mManager;
	ResourceManager* mResourceManager;
	GameObjectId mInstanceId;
	GameObjectId mOwnerInstanceId;
	GameObjectId mBorrowerInstanceId;
	str mClassId;
	NetworkObjectType mNetworkObjectType;
	ContextObject* mParent;
	void* mExtraData;
	Array mChildArray;
	TriggerMap mTriggerMap;
	const TBC::PhysicsSpawner* mSpawner;
	bool mIsLoaded;
	AttributeArray mAttributeArray;
	TBC::ChunkyPhysics* mPhysics;
	PhysicsOverride mPhysicsOverride;
	float mTotalMass;
	float mLastSendTime;
	ObjectPositionalData mPosition;
	ObjectPositionalData* mNetworkOutputGhost;
	int mSendCount;
	bool mAllowMoveRoot;	// This is set to false when attached to someone/something else.
	ConnectionList mConnectionList;

	LOG_CLASS_DECLARE();
};



}
