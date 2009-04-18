
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../TBC/Include/PhysicsEngine.h"



namespace Cure
{



class PhysicsNode
{
public:
	typedef unsigned Id;

	enum JointType
	{
		TYPE_EXCLUDE = 1,
		TYPE_SUSPEND_HINGE,
		TYPE_HINGE2,
		TYPE_HINGE,
		TYPE_BALL,
		TYPE_UNIVERSAL,
	};

	enum ConnectorType
	{
		CONNECT_NONE,
		CONNECTOR_3,
		CONNECTEE_3,
	};

	PhysicsNode();
	PhysicsNode(Id pParentId, Id pId, TBC::PhysicsEngine::BodyID pBodyId,
		JointType pJointType = TYPE_EXCLUDE, TBC::PhysicsEngine::JointID pJointId = TBC::INVALID_JOINT,
		ConnectorType pConnectorType = CONNECT_NONE);
	virtual ~PhysicsNode();

	Id GetParentId() const;
	Id GetId() const;
	TBC::PhysicsEngine::BodyID GetBodyId() const;
	JointType GetJointType() const;
	TBC::PhysicsEngine::JointID GetJointId() const;
	bool IsConnectorType(ConnectorType pType) const;
	void AddConnectorType(ConnectorType pType);

	float GetExtraData() const;
	void SetExtraData(float pExtraData);

private:
	typedef std::vector<ConnectorType> ConnectorArray;

	Id mParentId;
	Id mId;
	TBC::PhysicsEngine::BodyID mBodyId;
	JointType mJointType;
	TBC::PhysicsEngine::JointID mJointId;
	ConnectorArray mConnectorArray;
	float mExtraData;
};



}
