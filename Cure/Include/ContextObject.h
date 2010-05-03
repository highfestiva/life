
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
	typedef std::vector<ContextObject*> Array;

	ContextObject(ResourceManager* pResourceManager, const str& pClassId);
	virtual ~ContextObject();

	ContextManager* GetManager() const;
	void SetManager(ContextManager* pManager);

	GameObjectId GetInstanceId() const;
	void SetInstanceId(GameObjectId pInstanceId);
	const str& GetClassId() const;

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
	void RemoveAttribute(ContextObjectAttribute* pAttribute);

	void AddTrigger(TBC::PhysicsManager::TriggerID pTriggerId, const void*);
	const void* GetTrigger(TBC::PhysicsManager::TriggerID pTriggerId) const;

	bool UpdateFullPosition(const ObjectPositionalData*& pPositionalData);
	void SetFullPosition(const ObjectPositionalData& pPositionalData);
	void SetInitialTransform(const TransformationF& pTransformation);
	TransformationF GetInitialTransform() const;
	Vector3DF GetPosition() const;
	QuaternionF GetOrientation() const;
	Vector3DF GetVelocity() const;
	float GetForwardSpeed() const;
	float GetMass() const;

	bool SetPhysics(TBC::ChunkyPhysics* pStructure);
	TBC::ChunkyPhysics* GetPhysics() const;
	TBC::ChunkyBoneGeometry* GetStructureGeometry(unsigned pIndex) const;
	TBC::ChunkyBoneGeometry* GetStructureGeometry(TBC::PhysicsManager::BodyID pBodyId) const;
	void SetEnginePower(unsigned pAspect, float pPower, float pAngle);

	bool QueryResendTime(float pDeltaTime, bool pUnblockDelta);
	int PopSendCount();
	void SetSendCount(int pCount);

	virtual void StartLoading() = 0;
	virtual void OnTick(float pFrameTime) = 0;
	virtual void OnAlarm(int pAlarmId, void* pExtraData) = 0;
	virtual void OnPhysicsTick();

protected:
	void ForceSetFullPosition(const ObjectPositionalData& pPositionalData, const TBC::ChunkyBoneGeometry* pGeometry);
	void AttachToObject(TBC::ChunkyBoneGeometry* pBoneGeometry1, ContextObject* pObject2, TBC::ChunkyBoneGeometry* pBoneGeometry2, bool pSend);
	bool IsAttachedTo(ContextObject* pObject) const;
	void AddAttachment(ContextObject* pObject, TBC::PhysicsManager::JointID pJoint, TBC::PhysicsEngine* pEngine);

	void SetUsePhysics(bool pUsePhysics);
	void AddChild(ContextObject* pObject);
	void SetupChildTriggerHandlers();

	virtual bool IsSameInstance(TBC::PhysicsManager::ForceFeedbackListener* pOther);

	ResourceManager* GetResourceManager() const;

	typedef std::vector<ContextObjectAttribute*> AttributeArray;
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
	str mClassId;
	NetworkObjectType mNetworkObjectType;
	void* mExtraData;
	ChildList mChildList;
	TriggerMap mTriggerMap;
	bool mIsLoaded;
	AttributeArray mAttributeArray;
	TBC::ChunkyPhysics* mPhysics;
	bool mUsePhysics;
	float mLastSendTime;
	ObjectPositionalData mPosition;
	int mSendCount;
	bool mAllowMoveSelf;	// This is set to false when attached to someone/something else.
	ConnectionList mConnectionList;

	LOG_CLASS_DECLARE();
};



}
