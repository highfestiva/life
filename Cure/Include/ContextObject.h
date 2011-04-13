
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../TBC/Include/PhysicsManager.h"
#include <hash_map>
#include <list>
#include <vector>
#include "../../Lepra/Include/String.h"
#include "Cure.h"
#include "PositionalData.h"



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


class ContextObject: public TBC::PhysicsManager::TriggerListener, public TBC::PhysicsManager::ForceFeedbackListener
{
public:
	enum PhysicsOverride
	{
		PHYSICS_OVERRIDE_NORMAL = 1,
		PHYSICS_OVERRIDE_STATIC,
		PHYSICS_OVERRIDE_BONES,
	};

	typedef std::vector<ContextObject*> Array;
	typedef std::vector<ContextObjectAttribute*> AttributeArray;

	ContextObject(ResourceManager* pResourceManager, const str& pClassId);
	virtual ~ContextObject();

	ContextManager* GetManager() const;
	void SetManager(ContextManager* pManager);

	GameObjectId GetInstanceId() const;
	void SetInstanceId(GameObjectId pInstanceId);
	const str& GetClassId() const;
	GameObjectId GetOwnerInstanceId() const;
	void SetOwnerInstanceId(GameObjectId pInstanceId);

	NetworkObjectType GetNetworkObjectType() const;
	void SetNetworkObjectType(NetworkObjectType pType);

	void* GetExtraData() const;
	void SetExtraData(void* pData);

	bool IsLoaded() const;
	void SetLoadResult(bool pOk);

	void SetAllowMoveSelf(bool pAllow);
	void AttachToObject(TBC::PhysicsManager::BodyID pBody1, ContextObject* pObject2, TBC::PhysicsManager::BodyID pBody2);
	void AttachToObject(unsigned pBody1Index, ContextObject* pObject2, unsigned pBody2Index);
	bool DetachFromObject(ContextObject* pObject);

	void AddAttribute(ContextObjectAttribute* pAttribute);
	void DeleteAttribute(const str& pName);
	ContextObjectAttribute* GetAttribute(const str& pName) const;
	const AttributeArray& GetAttributes() const;
	float GetAttributeFloatValue(const str& pAttributeName) const;
	bool IsAttributeTrue(const str& pAttributeName) const;
	void OnAttributeUpdated(ContextObjectAttribute* pAttribute);

	void AddTrigger(TBC::PhysicsManager::TriggerID pTriggerId, const void*);
	virtual void FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger);
	const void* GetTrigger(TBC::PhysicsManager::TriggerID pTriggerId) const;
	size_t GetTriggerCount(const void*& pTrigger) const;

	void SetSpawner(const TBC::PhysicsSpawner* pSpawner);
	const TBC::PhysicsSpawner* GetSpawner() const;

	void AddChild(ContextObject* pChild);

	bool UpdateFullPosition(const ObjectPositionalData*& pPositionalData);
	void SetFullPosition(const ObjectPositionalData& pPositionalData);
	void SetInitialTransform(const TransformationF& pTransformation);
	TransformationF GetInitialTransform() const;
	Vector3DF GetPosition() const;
	void SetRootPosition(const Vector3DF& pPosition);
	void SetRootOrientation(const QuaternionF& pOrientation);
	QuaternionF GetOrientation() const;
	Vector3DF GetVelocity() const;
	Vector3DF GetAngularVelocity() const;
	Vector3DF GetAcceleration() const;
	float GetForwardSpeed() const;
	float GetMass() const;
	ObjectPositionalData* GetNetworkOutputGhost();
	void DeleteNetworkOutputGhost();

	bool SetPhysics(TBC::ChunkyPhysics* pStructure);
	void ClearPhysics();
	TBC::ChunkyPhysics* GetPhysics() const;
	void SetPhysicsTypeOverride(PhysicsOverride pPhysicsOverride);
	TBC::ChunkyBoneGeometry* GetStructureGeometry(unsigned pIndex) const;
	TBC::ChunkyBoneGeometry* GetStructureGeometry(TBC::PhysicsManager::BodyID pBodyId) const;
	bool SetEnginePower(unsigned pAspect, float pPower, float pAngle);
	float GetImpact(const Vector3DF& pGravity, const Vector3DF& pForce, const Vector3DF& pTorque, float pExtraMass = 0, float pSidewaysFactor = 1) const;

	void ForceSend();
	bool QueryResendTime(float pDeltaTime, bool pUnblockDelta);
	int PopSendCount();
	void SetSendCount(int pCount);

	virtual void StartLoading() = 0;
	virtual void OnLoaded();
	virtual void OnMicroTick(float pFrameTime) = 0;
	virtual void OnAlarm(int pAlarmId, void* pExtraData) = 0;
	virtual void OnTick();

protected:
	void ForceSetFullPosition(const ObjectPositionalData& pPositionalData, const TBC::ChunkyBoneGeometry* pGeometry);
	void AttachToObject(TBC::ChunkyBoneGeometry* pBoneGeometry1, ContextObject* pObject2, TBC::ChunkyBoneGeometry* pBoneGeometry2, bool pSend);
	bool IsAttachedTo(ContextObject* pObject) const;
	void AddAttachment(ContextObject* pObject, TBC::PhysicsManager::JointID pJoint, TBC::PhysicsEngine* pEngine);

	void RemoveChild(ContextObject* pChild);
	void SetParent(ContextObject* pParent);
	void SetupChildHandlers();

	virtual bool IsSameInstance(TBC::PhysicsManager::ForceFeedbackListener* pOther);

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
	typedef std::hash_map<TBC::PhysicsManager::TriggerID, const void*> TriggerMap;
	typedef std::list<ContextObject*> ChildList;

	ContextManager* mManager;
	ResourceManager* mResourceManager;
	GameObjectId mInstanceId;
	GameObjectId mOwnerInstanceId;
	str mClassId;
	NetworkObjectType mNetworkObjectType;
	ContextObject* mParent;
	void* mExtraData;
	ChildList mChildList;
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
	bool mAllowMoveSelf;	// This is set to false when attached to someone/something else.
	ConnectionList mConnectionList;

	LOG_CLASS_DECLARE();
};



}
