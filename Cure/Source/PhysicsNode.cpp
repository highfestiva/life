
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include "../Include/PhysicsNode.h"



namespace Cure
{



PhysicsNode::PhysicsNode():
	mId((unsigned)-1),
	mParentId((unsigned)-1),
	mBodyId(TBC::INVALID_BODY),
	mJointType(TYPE_EXCLUDE),
	mJointId(TBC::INVALID_JOINT)
{
}

PhysicsNode::PhysicsNode(Id pParentId, Id pId, TBC::PhysicsEngine::BodyID pBodyId,
	JointType pJointType, TBC::PhysicsEngine::JointID pJointId):
	mParentId(pParentId),
	mId(pId),
	mBodyId(pBodyId),
	mJointType(pJointType),
	mJointId(pJointId),
	mExtraData(0)
{
	assert(mId >= 0);
	assert(mBodyId != TBC::INVALID_BODY);
}

PhysicsNode::~PhysicsNode()
{
}



PhysicsNode::Id PhysicsNode::GetParentId() const
{
	return (mParentId);
}

PhysicsNode::Id PhysicsNode::GetId() const
{
	return (mId);
}

TBC::PhysicsEngine::BodyID PhysicsNode::GetBodyId() const
{
	return (mBodyId);
}

PhysicsNode::JointType PhysicsNode::GetJointType() const
{
	return (mJointType);
}

TBC::PhysicsEngine::JointID PhysicsNode::GetJointId() const
{
	return (mJointId);
}



float PhysicsNode::GetExtraData() const
{
	return (mExtraData);
}

void PhysicsNode::SetExtraData(float pExtraData)
{
	mExtraData = pExtraData;
}



}
