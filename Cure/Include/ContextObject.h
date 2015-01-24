
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Tbc/Include/PhysicsManager.h"
#include "../../Lepra/Include/Unordered.h"
#include <list>
#include <vector>
#include "../../Lepra/Include/String.h"
#include "Cure.h"
#include "PositionalData.h"



namespace Tbc
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
	void AttachToObjectByBodyIds(Tbc::PhysicsManager::BodyID pBody1, ContextObject* pObject2, Tbc::PhysicsManager::BodyID pBody2);
	void AttachToObjectByBodyIndices(unsigned pBody1Index, ContextObject* pObject2, unsigned pBody2Index);
	void DetachAll();
	bool DetachFromObject(ContextObject* pObject);
	Array GetAttachedObjects() const;
	void AddAttachedObjectEngine(ContextObject* pAttachedObject, Tbc::PhysicsEngine* pEngine);

	void AddAttribute(ContextObjectAttribute* pAttribute);
	void DeleteAttribute(const str& pName);
	ContextObjectAttribute* GetAttribute(const str& pName) const;
	const AttributeArray& GetAttributes() const;
	float GetAttributeFloatValue(const str& pAttributeName) const;
	void QuerySetChildishness(float pChildishness);
	bool IsAttributeTrue(const str& pAttributeName) const;
	void OnAttributeUpdated(ContextObjectAttribute* pAttribute);

	void AddTrigger(Tbc::PhysicsManager::TriggerID pTriggerId, const void*);
	virtual void FinalizeTrigger(const Tbc::PhysicsTrigger* pTrigger);
	const void* GetTrigger(Tbc::PhysicsManager::TriggerID pTriggerId) const;
	size_t GetTriggerCount(const void*& pTrigger) const;

	virtual void SetSpawner(const Tbc::PhysicsSpawner* pSpawner);
	const Tbc::PhysicsSpawner* GetSpawner() const;

	void AddChild(ContextObject* pChild);
	const Array& GetChildArray() const;

	bool UpdateFullPosition(const ObjectPositionalData*& pPositionalData);	// Fetch full phys position (and update object graph as necessary).
	static bool UpdateFullPosition(ObjectPositionalData& pPosition, Tbc::PhysicsManager* pPhysicsManager, Tbc::ChunkyPhysics* pStructure);
	void SetFullPosition(const ObjectPositionalData& pPositionalData, float pDeltaThreshold);	// Sets full phys position if structure or significant difference seen.
	virtual void SetPositionFinalized();
	void SetInitialTransform(const xform& pTransformation);
	xform GetInitialTransform() const;
	void SetInitialPositionalData(const ObjectPositionalData& pPositionalData);
	vec3 GetPosition() const;
	void SetRootPosition(const vec3& pPosition);
	vec3 GetRootPosition() const;
	void SetRootOrientation(const quat& pOrientation);
	void SetRootVelocity(const vec3& pVelocity);
	quat GetOrientation() const;
	vec3 GetVelocity() const;
	vec3 GetAngularVelocity() const;
	vec3 GetAcceleration() const;
	vec3 GetForwardDirection() const;
	float GetForwardSpeed() const;
	float GetMass() const;
	float QueryMass();
	void SetMass(float pMass);
	ObjectPositionalData* GetNetworkOutputGhost();
	void DeleteNetworkOutputGhost();

	void SetPhysics(Tbc::ChunkyPhysics* pStructure);
	void ClearPhysics();
	Tbc::ChunkyPhysics* GetPhysics() const;
	void SetPhysicsTypeOverride(PhysicsOverride pPhysicsOverride);
	Tbc::ChunkyBoneGeometry* GetStructureGeometry(unsigned pIndex) const;
	Tbc::ChunkyBoneGeometry* GetStructureGeometry(Tbc::PhysicsManager::BodyID pBodyId) const;
	bool SetEnginePower(unsigned pAspect, float pPower);
	float GetImpact(const vec3& pGravity, const vec3& pForce, const vec3& pTorque, float pExtraMass = 0, float pSidewaysFactor = 1) const;

	void ForceSend();
	bool QueryResendTime(float pDeltaTime, bool pUnblockDelta);
	int PopSendCount();
	void SetSendCount(int pCount);

	virtual void StartLoading() = 0;
	virtual void OnLoaded();
	virtual void OnMicroTick(float pFrameTime) = 0;
	virtual void OnAlarm(int pAlarmId, void* pExtraData) = 0;
	virtual void OnTrigger(Tbc::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pNormal) = 0;
	virtual void OnForceApplied(ContextObject* pOtherObject,
		 Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		 const vec3& pForce, const vec3& pTorque,
		 const vec3& pPosition, const vec3& pRelativeVelocity) = 0;
	virtual void OnTick();

protected:
	void ForceSetFullPosition(const ObjectPositionalData& pPositionalData);
	void AttachToObject(Tbc::ChunkyBoneGeometry* pBoneGeometry1, ContextObject* pObject2, Tbc::ChunkyBoneGeometry* pBoneGeometry2, bool pSend);
	bool IsAttachedTo(ContextObject* pObject) const;
	void AddAttachment(ContextObject* pObject, Tbc::PhysicsManager::JointID pJoint, Tbc::PhysicsEngine* pEngine);

	void RemoveChild(ContextObject* pChild);
	void SetParent(ContextObject* pParent);
	virtual void SetupChildHandlers();

	ResourceManager* GetResourceManager() const;

	typedef std::vector<Tbc::PhysicsEngine*> EngineList;
	struct Connection
	{
		Connection(ContextObject* pObject, Tbc::PhysicsManager::JointID pJointId, Tbc::PhysicsEngine* pEngine=0):
			mObject(pObject),
			mJointId(pJointId)
		{
			if (pEngine)
			{
				mEngineList.push_back(pEngine);
			}
		}
		ContextObject* mObject;
		Tbc::PhysicsManager::JointID mJointId;
		EngineList mEngineList;
	};
	typedef std::list<Connection> ConnectionList;
	typedef std::unordered_map<Tbc::PhysicsManager::TriggerID, const void*> TriggerMap;

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
	const Tbc::PhysicsSpawner* mSpawner;
	bool mIsLoaded;
	AttributeArray mAttributeArray;
	Tbc::ChunkyPhysics* mPhysics;
	PhysicsOverride mPhysicsOverride;
	float mTotalMass;
	float mLastSendTime;
	ObjectPositionalData mPosition;
	ObjectPositionalData* mNetworkOutputGhost;
	int mSendCount;
	bool mAllowMoveRoot;	// This is set to false when attached to someone/something else.
	ConnectionList mConnectionList;

	logclass();
};



}
