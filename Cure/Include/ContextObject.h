
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include <list>
#include "../../Lepra/Include/String.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "Cure.h"
#include "PositionalData.h"



namespace TBC
{
class ChunkyStructure;
class ChunkyBoneGeometry;
class StructureEngine;
}



namespace Cure
{



class ContextManager;
class ContextObjectAttribute;


class ContextObject: public TBC::PhysicsEngine::TriggerListener, public TBC::PhysicsEngine::ForceFeedbackListener
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

	void SetAllowMoveSelf(bool pAllow);
	void AttachToObject(TBC::PhysicsEngine::BodyID pBody1, ContextObject* pObject2, TBC::PhysicsEngine::BodyID pBody2);
	void AttachToObject(unsigned pBody1Index, ContextObject* pObject2, unsigned pBody2Index);
	bool DetachFromObject(ContextObject* pObject);

	void AddAttribute(ContextObjectAttribute* pAttribute);
	void RemoveAttribute(ContextObjectAttribute* pAttribute);

	bool UpdateFullPosition(const ObjectPositionalData*& pPositionalData);
	void SetFullPosition(const ObjectPositionalData& pPositionalData);
	Lepra::Vector3DF GetPosition() const;
	float GetForwardSpeed() const;
	float GetMass() const;

	bool SetStructure(TBC::ChunkyStructure* pStructure);
	TBC::ChunkyBoneGeometry* GetStructureGeometry(unsigned pIndex) const;
	TBC::ChunkyBoneGeometry* GetStructureGeometry(TBC::PhysicsEngine::BodyID pBodyId) const;
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
	void AddAttachment(ContextObject* pObject, TBC::PhysicsEngine::JointID pJoint, TBC::StructureEngine* pEngine);

	typedef std::vector<ContextObjectAttribute*> AttributeArray;
	struct Connection
	{
		Connection(ContextObject* pObject, TBC::PhysicsEngine::JointID pJointId, TBC::StructureEngine* pEngine):
			mObject(pObject),
			mJointId(pJointId),
			mEngine(pEngine)
		{
		}
		ContextObject* mObject;
		TBC::PhysicsEngine::JointID mJointId;
		TBC::StructureEngine* mEngine;
	};
	typedef std::list<Connection> ConnectionList;

	ContextManager* mManager;
	GameObjectId mInstanceId;
	Lepra::String mClassId;
	NetworkObjectType mNetworkObjectType;
	AttributeArray mAttributeArray;
	TBC::ChunkyStructure* mStructure;
	float mLastSendTime;
	ObjectPositionalData mPosition;
	int mSendCount;
	bool mAllowMoveSelf;	// This is set to false when attached to someone/something else.
	ConnectionList mConnectionList;

	LOG_CLASS_DECLARE();
};



class ContextObjectFactory
{
public:
	virtual ContextObject* Create(const Lepra::String& pClassId) const = 0;
	virtual bool CreatePhysics(ContextObject* pObject, ContextObject* pTriggerListener) const = 0;
};



}
