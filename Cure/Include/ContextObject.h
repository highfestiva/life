
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include <list>
#include "../../Lepra/Include/String.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "Cure.h"
#include "PositionalData.h"



namespace TBC
{
class ChunkyPhysics;
class ChunkyBoneGeometry;
class PhysicsEngine;
}



namespace Cure
{



class ContextManager;
class ContextObjectAttribute;


class ContextObject: public TBC::PhysicsManager::TriggerListener, public TBC::PhysicsManager::ForceFeedbackListener
{
public:
	ContextObject(const Lepra::String& pClassId);
	virtual ~ContextObject();

	ContextManager* GetManager() const;
	void SetManager(ContextManager* pManager);

	GameObjectId GetInstanceId() const;
	void SetInstanceId(GameObjectId pInstanceId);
	const Lepra::String& GetClassId() const;

	NetworkObjectType GetNetworkObjectType() const;
	void SetNetworkObjectType(NetworkObjectType pType);

	void* GetExtraData() const;
	void SetExtraData(void* pData);

	void SetAllowMoveSelf(bool pAllow);
	void AttachToObject(TBC::PhysicsManager::BodyID pBody1, ContextObject* pObject2, TBC::PhysicsManager::BodyID pBody2);
	void AttachToObject(unsigned pBody1Index, ContextObject* pObject2, unsigned pBody2Index);
	bool DetachFromObject(ContextObject* pObject);

	void AddAttribute(ContextObjectAttribute* pAttribute);
	void RemoveAttribute(ContextObjectAttribute* pAttribute);

	bool UpdateFullPosition(const ObjectPositionalData*& pPositionalData);
	void SetFullPosition(const ObjectPositionalData& pPositionalData);
	Lepra::Vector3DF GetPosition() const;
	float GetForwardSpeed() const;
	float GetMass() const;

	bool SetPhysics(TBC::ChunkyPhysics* pStructure);
	TBC::ChunkyBoneGeometry* GetStructureGeometry(unsigned pIndex) const;
	TBC::ChunkyBoneGeometry* GetStructureGeometry(TBC::PhysicsManager::BodyID pBodyId) const;
	void SetEnginePower(unsigned pAspect, float pPower, float pAngle);

	bool QueryResendTime(float pDeltaTime, bool pUnblockDelta);
	int PopSendCount();
	void SetSendCount(int pCount);

	virtual void OnTick(float pFrameTime) = 0;
	virtual void OnAlarm(int pAlarmId) = 0;
	virtual void OnPhysicsTick();

protected:
	void AttachToObject(TBC::ChunkyBoneGeometry* pBoneGeometry1, ContextObject* pObject2, TBC::ChunkyBoneGeometry* pBoneGeometry2, bool pSend);
	bool IsAttachedTo(ContextObject* pObject) const;
	void AddAttachment(ContextObject* pObject, TBC::PhysicsManager::JointID pJoint, TBC::PhysicsEngine* pEngine);

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

	ContextManager* mManager;
	GameObjectId mInstanceId;
	Lepra::String mClassId;
	NetworkObjectType mNetworkObjectType;
	void* mExtraData;
	AttributeArray mAttributeArray;
	TBC::ChunkyPhysics* mPhysics;
	float mLastSendTime;
	ObjectPositionalData mPosition;
	int mSendCount;
	bool mAllowMoveSelf;	// This is set to false when attached to someone/something else.
	ConnectionList mConnectionList;

	LOG_CLASS_DECLARE();
};



}
