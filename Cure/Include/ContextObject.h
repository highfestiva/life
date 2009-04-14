
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include "../../Lepra/Include/String.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "Cure.h"
#include "PositionalData.h"
#include "PhysicsNode.h"



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

	void AddAttribute(ContextObjectAttribute* pAttribute);

	bool UpdateFullPosition(const ObjectPositionalData*& pPositionalData);
	void SetFullPosition(const ObjectPositionalData& pPositionalData);
	Lepra::Vector3DF GetPosition() const;

	void AddPhysicsObject(const PhysicsNode& pPhysicsNode);
	PhysicsNode* GetPhysicsNode(PhysicsNode::Id pId) const;
	void SetEnginePower(unsigned pAspect, float pPower, float pAngle);

	void StepGhost(ObjectPositionalData& pGhost, float pDeltaTime);

	virtual void OnTick(float pFrameTime) = 0;
	virtual void OnAlarm(int pAlarmId) = 0;
	virtual void OnPhysicsTick();

protected:
	typedef std::vector<ContextObjectAttribute*> AttributeArray;
	typedef std::vector<PhysicsNode> PhysicsNodeArray;

	ContextManager* mManager;
	GameObjectId mInstanceId;
	Lepra::String mClassId;
	NetworkObjectType mNetworkObjectType;
	AttributeArray mAttributeArray;
	PhysicsNodeArray mPhysicsNodeArray;
	int mRootPhysicsIndex;	// TODO: remove this hack!
	ObjectPositionalData mPosition;

	LOG_CLASS_DECLARE();
};



class ContextObjectFactory
{
public:
	virtual ContextObject* Create(const Lepra::String& pClassId) const = 0;
	virtual bool CreatePhysics(ContextObject* pObject, ContextObject* pTriggerListener) const = 0;
};



}
