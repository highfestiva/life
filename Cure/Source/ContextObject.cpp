
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include <assert.h>
#include <math.h>
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/RotationMatrix.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../Include/ContextManager.h"
#include "../Include/ContextObject.h"
#include "../Include/ContextObjectEngine.h"
#include "../Include/Cure.h"
#include "../Include/GameManager.h"



namespace Cure
{



#define GET_OBJECT_POSITIONAL_AT(obj, at, cls, name, type)		\
	cls* name = (cls*)((obj).GetAt(at));				\
	if (name && name->GetType() != type)				\
	{								\
		name = 0;						\
	}

#define GETSET_OBJECT_POSITIONAL_AT(obj, at, cls, name, type, scale)	\
	GET_OBJECT_POSITIONAL_AT(obj, at, cls, name, type)		\
	if (!name)							\
	{								\
		name = new cls;						\
		(obj).SetAt(at, name);					\
	}								\
	name->SetScale(scale);



ContextObject::ContextObject(const Lepra::String& pClassId):
	mManager(0),
	mInstanceId(0),
	mClassId(pClassId),
	mNetworkObjectType(NETWORK_OBJECT_LOCAL_ONLY),
	mRootPhysicsIndex(-1),
	mAllowMoveSelf(true)
{
}

ContextObject::~ContextObject()
{
	log_volatile(mLog.Debugf(_T("Destructing context object %s."), mClassId.c_str()));

	mManager->RemoveObject(this);
	mRootPhysicsIndex = -1;

	PhysicsNodeArray::iterator x = mPhysicsNodeArray.begin();
	for (; x != mPhysicsNodeArray.end(); ++x)
	{
		mManager->RemovePhysicsJoint(x->GetJointId());
	}
	x = mPhysicsNodeArray.begin();
	for (; x != mPhysicsNodeArray.end(); ++x)
	{
		mManager->RemovePhysicsBody(x->GetBodyId());
	}
	mPhysicsNodeArray.clear();

	AttributeArray::iterator y = mAttributeArray.begin();
	for (; y != mAttributeArray.end(); ++y)
	{
		delete (*y);
	}
	mAttributeArray.clear();

	mManager->FreeGameObjectId(mNetworkObjectType, mInstanceId);
	mInstanceId = 0;
}



ContextManager* ContextObject::GetManager() const
{
	return (mManager);
}

void ContextObject::SetManager(ContextManager* pManager)
{
	assert(mManager == 0);
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

const Lepra::String& ContextObject::GetClassId() const
{
	return (mClassId);
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



void ContextObject::SetAllowMoveSelf(bool pAllow)
{
	mAllowMoveSelf = pAllow;
}

bool ContextObject::IsConnectedTo(ContextObject* pObject) const
{
	return (std::find(mConnectionList.begin(), mConnectionList.end(), pObject) != mConnectionList.end());
}

void ContextObject::AddConnection(ContextObject* pObject, TBC::PhysicsEngine::JointID pJoint)
{
	assert(!IsConnectedTo(pObject));
	mConnectionList.push_back(pObject);
	pJoint;	// TODO!
}

bool ContextObject::RemoveConnection(ContextObject* pObject)
{
	bool lRemoved = false;
	// TODO: something about that joint!
	ConnectionList::iterator x = std::find(mConnectionList.begin(), mConnectionList.end(), pObject);
	if (x != mConnectionList.end())
	{
		mConnectionList.erase(x);
		lRemoved = true;
	}
	return (lRemoved);
}



void ContextObject::AddAttribute(ContextObjectAttribute* pAttribute)
{
	mAttributeArray.push_back(pAttribute);
}



bool ContextObject::UpdateFullPosition(const ObjectPositionalData*& pPositionalData)
{
	if (mRootPhysicsIndex < 0 || mPhysicsNodeArray[mRootPhysicsIndex].GetBodyId() == TBC::INVALID_BODY)
	{
		return (false);
	}

	TBC::PhysicsEngine* lPhysics = mManager->GetGameManager()->GetPhysicsManager();
	TBC::PhysicsEngine::BodyID lBody = mPhysicsNodeArray[mRootPhysicsIndex].GetBodyId();
	lPhysics->GetBodyTransform(lBody, mPosition.mPosition.mTransformation);
	lPhysics->GetBodyVelocity(lBody, mPosition.mPosition.mVelocity);
	lPhysics->GetBodyAcceleration(lBody, mPosition.mPosition.mAcceleration);
	lPhysics->GetBodyAngularVelocity(lBody, mPosition.mPosition.mAngularVelocity);
	lPhysics->GetBodyAngularAcceleration(lBody, mPosition.mPosition.mAngularAcceleration);

	size_t y = 0;
	for (size_t x = 0; x < mPhysicsNodeArray.size(); ++x)
	{
		// TODO: add support for parent ID.
		const PhysicsNode& lNode = mPhysicsNodeArray[x];
		lBody = lNode.GetBodyId();
		TBC::PhysicsEngine::JointID lJoint = lNode.GetJointId();
		switch (lNode.GetJointType())
		{
			case PhysicsNode::TYPE_SUSPEND_HINGE:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData2, lData, PositionalData::TYPE_POSITION_2, 1);
				++y;
				TBC::PhysicsEngine::Joint3Diff lDiff;
				if (!lPhysics->GetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get hinge-2!");
					return (false);
				}
				lData->mTransformation[0] = lDiff.mValue;
				lData->mTransformation[1] = lDiff.mAngle2;
				lData->mVelocity[0] = lDiff.mValueVelocity;
				lData->mVelocity[1] = lDiff.mAngle2Velocity;
				lData->mAcceleration[0] = lDiff.mValueAcceleration;
				lData->mAcceleration[1] = lDiff.mAngle2Acceleration;
			}
			break;
			case PhysicsNode::TYPE_HINGE2:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData3, lData, PositionalData::TYPE_POSITION_3, 1);
				++y;
				TBC::PhysicsEngine::Joint3Diff lDiff;
				if (!lPhysics->GetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get hinge-2!");
					return (false);
				}
				lData->mTransformation[0] = lDiff.mValue;
				lData->mTransformation[1] = lDiff.mAngle1;
				lData->mTransformation[2] = lDiff.mAngle2;
				lData->mVelocity[0] = lDiff.mValueVelocity;
				lData->mVelocity[1] = lDiff.mAngle1Velocity;
				lData->mVelocity[2] = lDiff.mAngle2Velocity;
				lData->mAcceleration[0] = lDiff.mValueAcceleration;
				lData->mAcceleration[1] = lDiff.mAngle1Acceleration;
				lData->mAcceleration[2] = lDiff.mAngle2Acceleration;
			}
			break;
			case PhysicsNode::TYPE_HINGE:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData1, lData, PositionalData::TYPE_POSITION_1, 1);
				++y;
				TBC::PhysicsEngine::Joint1Diff lDiff;
				if (!lPhysics->GetJoint1Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get hinge!");
					return (false);
				}
				lData->mTransformation = lDiff.mAngle;
				lData->mVelocity = lDiff.mAngleVelocity;
				lData->mAcceleration = lDiff.mAngleAcceleration;
			}
			break;
			case PhysicsNode::TYPE_BALL:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData3, lData, PositionalData::TYPE_POSITION_3, 0.00001f);
				++y;
				TBC::PhysicsEngine::Joint3Diff lDiff;
				if (!lPhysics->GetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get ball!");
					return (false);
				}
				lData->mTransformation[0] = lDiff.mValue;
				lData->mTransformation[1] = lDiff.mAngle1;
				lData->mTransformation[2] = lDiff.mAngle2;
				lData->mVelocity[0] = lDiff.mValueVelocity;
				lData->mVelocity[1] = lDiff.mAngle1Velocity;
				lData->mVelocity[2] = lDiff.mAngle2Velocity;
				lData->mAcceleration[0] = lDiff.mValueAcceleration;
				lData->mAcceleration[1] = lDiff.mAngle1Acceleration;
				lData->mAcceleration[2] = lDiff.mAngle2Acceleration;
			}
			break;
			case PhysicsNode::TYPE_UNIVERSAL:
			{
				GETSET_OBJECT_POSITIONAL_AT(mPosition, y, PositionalData2, lData, PositionalData::TYPE_POSITION_2, 1);
				++y;
				TBC::PhysicsEngine::Joint2Diff lDiff;
				if (!lPhysics->GetJoint2Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not get universal!");
					return (false);
				}
				lData->mTransformation[0] = lDiff.mValue;
				lData->mTransformation[1] = lDiff.mAngle;
				lData->mVelocity[0] = lDiff.mValueVelocity;
				lData->mVelocity[1] = lDiff.mAngleVelocity;
				lData->mAcceleration[0] = lDiff.mValueAcceleration;
				lData->mAcceleration[1] = lDiff.mAngleAcceleration;
			}
			break;
			case PhysicsNode::TYPE_EXCLUDE:
			{
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}
	}

	AttributeArray::const_iterator z = mAttributeArray.begin();
	for (; z != mAttributeArray.end(); ++z)
	{
		// TODO: add support for parent ID.
		ContextObjectAttribute* lAttribute = *z;
		if (lAttribute->GetType() == ContextObjectAttribute::TYPE_ENGINE)
		{
			ContextObjectEngine* lEngine = (ContextObjectEngine*)lAttribute;
			switch (lEngine->GetEngineType())
			{
				case ContextObjectEngine::ENGINE_CAMERA_FLAT_PUSH:
				{
					GETSET_OBJECT_POSITIONAL_AT(mPosition, y, RealData4, lData, PositionalData::TYPE_REAL_4, 1);
					++y;
					::memcpy(lData->mValue, lEngine->GetValues(), sizeof(float)*4);
				}
				break;
				case ContextObjectEngine::ENGINE_HINGE2_ROLL:
				{
					GETSET_OBJECT_POSITIONAL_AT(mPosition, y, RealData1, lData, PositionalData::TYPE_REAL_1, 1);
					++y;
					lData->mValue = lEngine->GetValue();
				}
				break;
				case ContextObjectEngine::ENGINE_HINGE2_TURN:
				{
					GETSET_OBJECT_POSITIONAL_AT(mPosition, y, RealData1Bool, lData, PositionalData::TYPE_REAL_1_BOOL, 1);
					++y;
					lData->mValue = lEngine->GetValue();
					lData->mBool = lEngine->IsAnalogue();
				}
				break;
				case ContextObjectEngine::ENGINE_HINGE2_BREAK:
				{
					GETSET_OBJECT_POSITIONAL_AT(mPosition, y, RealData1, lData, PositionalData::TYPE_REAL_1, 1);
					++y;
					lData->mValue = lEngine->GetValue();
				}
				break;
				case ContextObjectEngine::ENGINE_HINGE:
				{
					GETSET_OBJECT_POSITIONAL_AT(mPosition, y, RealData1, lData, PositionalData::TYPE_REAL_1, 1);
					++y;
					lData->mValue = lEngine->GetValue();
				}
				break;
				case ContextObjectEngine::ENGINE_ROLL_STRAIGHT:
				{
					// Unsynchronized "engine".
				}
				break;
				default:
				{
					assert(false);
				}
				break;
			}
		}
	}

	mPosition.Trunkate(y);
	pPositionalData = &mPosition;

	return (true);
}

void ContextObject::SetFullPosition(const ObjectPositionalData& pPositionalData)
{
	if (mRootPhysicsIndex < 0 || mPhysicsNodeArray[mRootPhysicsIndex].GetBodyId() == TBC::INVALID_BODY)
	{
		return;
	}

	if (mPosition.IsSameStructure(pPositionalData))
	{
		if (mPosition.GetScaledDifference(&pPositionalData) == 0)
		{
			return;
		}
	}

	mPosition.CopyData(&pPositionalData);

	TBC::PhysicsEngine* lPhysics = mManager->GetGameManager()->GetPhysicsManager();
	TBC::PhysicsEngine::BodyID lBody;
	if (mAllowMoveSelf)
	{
		lBody = mPhysicsNodeArray[mRootPhysicsIndex].GetBodyId();
		lPhysics->SetBodyTransform(lBody, pPositionalData.mPosition.mTransformation);
		lPhysics->SetBodyVelocity(lBody, pPositionalData.mPosition.mVelocity);
		lPhysics->SetBodyAcceleration(lBody, pPositionalData.mPosition.mAcceleration);
		lPhysics->SetBodyAngularVelocity(lBody, pPositionalData.mPosition.mAngularVelocity);
		lPhysics->SetBodyAngularAcceleration(lBody, pPositionalData.mPosition.mAngularAcceleration);
	}

	if (mPosition.mBodyPositionArray.size() <= 0)
	{
		return;
	}

	//mLog.AInfo("Setting full position.");

	size_t y = 0;
	for (size_t x = 0; x < mPhysicsNodeArray.size(); ++x)
	{
		assert(mPosition.mBodyPositionArray.size() > y);
		// TODO: add support for parent ID.
		const PhysicsNode& lNode = mPhysicsNodeArray[x];
		lBody = lNode.GetBodyId();
		TBC::PhysicsEngine::JointID lJoint = lNode.GetJointId();
		switch (lNode.GetJointType())
		{
			case PhysicsNode::TYPE_SUSPEND_HINGE:
			{
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_2);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const PositionalData2, lData, PositionalData::TYPE_POSITION_2);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsEngine::Joint3Diff lDiff(lData->mTransformation[0], lData->mTransformation[1], 100000,
					lData->mVelocity[0], lData->mVelocity[1], 100000,
					lData->mAcceleration[0], lData->mAcceleration[1], 100000);
				if (!lPhysics->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge-2!");
					return;
				}
			}
			break;
			case PhysicsNode::TYPE_HINGE2:
			{
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_3);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const PositionalData3, lData, PositionalData::TYPE_POSITION_3);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsEngine::Joint3Diff lDiff(lData->mTransformation[0], lData->mTransformation[1], lData->mTransformation[2],
					lData->mVelocity[0], lData->mVelocity[1], lData->mVelocity[2],
					lData->mAcceleration[0], lData->mAcceleration[1], lData->mAcceleration[2]);
				if (!lPhysics->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge-2!");
					return;
				}
			}
			break;
			case PhysicsNode::TYPE_HINGE:
			{
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_1);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const PositionalData1, lData, PositionalData::TYPE_POSITION_1);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsEngine::Joint1Diff lDiff(lData->mTransformation,
					lData->mVelocity, lData->mAcceleration);
				if (!lPhysics->SetJoint1Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set hinge!");
					return;
				}
			}
			break;
			case PhysicsNode::TYPE_BALL:
			{
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_3);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const PositionalData3, lData, PositionalData::TYPE_POSITION_3);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsEngine::Joint3Diff lDiff(lData->mTransformation[0], lData->mTransformation[1], lData->mTransformation[2],
					lData->mVelocity[0], lData->mVelocity[1], lData->mVelocity[2],
					lData->mAcceleration[0], lData->mAcceleration[1], lData->mAcceleration[2]);
				if (!lPhysics->SetJoint3Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set ball!");
					return;
				}
			}
			break;
			case PhysicsNode::TYPE_UNIVERSAL:
			{
				assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_POSITION_2);
				GET_OBJECT_POSITIONAL_AT(mPosition, y, const PositionalData2, lData, PositionalData::TYPE_POSITION_2);
				++y;
				assert(lData);
				if (!lData)
				{
					mLog.AError("Could not fetch the right type of network positional!");
					return;
				}
				const TBC::PhysicsEngine::Joint2Diff lDiff(lData->mTransformation[0], lData->mTransformation[1],
					lData->mVelocity[0], lData->mVelocity[1],
					lData->mAcceleration[0], lData->mAcceleration[1]);
				if (!lPhysics->SetJoint2Diff(lBody, lJoint, lDiff))
				{
					mLog.AError("Could not set universal!");
					return;
				}
			}
			break;
			case PhysicsNode::TYPE_EXCLUDE:
			{
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}
	}

	AttributeArray::iterator z = mAttributeArray.begin();
	for (; z != mAttributeArray.end(); ++z)
	{
		// TODO: add support for parent ID.
		ContextObjectAttribute* lAttribute = *z;
		if (lAttribute->GetType() == ContextObjectAttribute::TYPE_ENGINE)
		{
			ContextObjectEngine* lEngine = (ContextObjectEngine*)lAttribute;
			switch (lEngine->GetEngineType())
			{
				case ContextObjectEngine::ENGINE_CAMERA_FLAT_PUSH:
				{
					assert(mPosition.mBodyPositionArray.size() > y);
					assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_4);
					GET_OBJECT_POSITIONAL_AT(mPosition, y, const RealData4, lData, PositionalData::TYPE_REAL_4);
					++y;
					assert(lData);
					if (!lData)
					{
						mLog.AError("Could not fetch the right type of network positional!");
						return;
					}
					SetEnginePower(0, lData->mValue[0], lData->mValue[3]);
					SetEnginePower(1, lData->mValue[1], lData->mValue[3]);
					SetEnginePower(2, lData->mValue[2], lData->mValue[3]);
				}
				break;
				case ContextObjectEngine::ENGINE_HINGE2_ROLL:
				{
					assert(mPosition.mBodyPositionArray.size() > y);
					assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_1);
					GET_OBJECT_POSITIONAL_AT(mPosition, y, const RealData1, lData, PositionalData::TYPE_REAL_1);
					++y;
					assert(lData);
					if (!lData)
					{
						mLog.AError("Could not fetch the right type of network positional!");
						return;
					}
					assert(lData->mValue == 0 || lData->mValue == 1 || lData->mValue == -1);
					SetEnginePower(lEngine->GetControllerIndex(), lData->mValue, 0);
				}
				break;
				case ContextObjectEngine::ENGINE_HINGE2_TURN:
				{
					assert(mPosition.mBodyPositionArray.size() > y);
					assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_1_BOOL);
					GET_OBJECT_POSITIONAL_AT(mPosition, y, const RealData1Bool, lData, PositionalData::TYPE_REAL_1_BOOL);
					++y;
					assert(lData);
					if (!lData)
					{
						mLog.AError("Could not fetch the right type of network positional!");
						return;
					}
					assert(lData->mValue == 0 || lData->mValue == 1 || lData->mValue == -1);
					assert(!lData->mBool);
					unsigned lAnalogueFlag = (lData->mBool)? 0x80 : 0;
					SetEnginePower(lEngine->GetControllerIndex()|lAnalogueFlag, lData->mValue, 0);
				}
				break;
				case ContextObjectEngine::ENGINE_HINGE2_BREAK:
				{
					assert(mPosition.mBodyPositionArray.size() > y);
					assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_1);
					GET_OBJECT_POSITIONAL_AT(mPosition, y, const RealData1, lData, PositionalData::TYPE_REAL_1);
					++y;
					assert(lData);
					if (!lData)
					{
						mLog.AError("Could not fetch the right type of network positional!");
						return;
					}
					assert(lData->mValue == 0 || lData->mValue == 1 || lData->mValue == -1);
					SetEnginePower(lEngine->GetControllerIndex(), lData->mValue, 0);
				}
				break;
				case ContextObjectEngine::ENGINE_HINGE:
				{
					assert(mPosition.mBodyPositionArray.size() > y);
					assert(mPosition.mBodyPositionArray[y]->GetType() == PositionalData::TYPE_REAL_1);
					GET_OBJECT_POSITIONAL_AT(mPosition, y, const RealData1, lData, PositionalData::TYPE_REAL_1);
					++y;
					assert(lData);
					if (!lData)
					{
						mLog.AError("Could not fetch the right type of network positional!");
						return;
					}
					assert(lData->mValue == 0 || lData->mValue == 1 || lData->mValue == -1);
					SetEnginePower(lEngine->GetControllerIndex(), lData->mValue, 0);
				}
				break;
				case ContextObjectEngine::ENGINE_ROLL_STRAIGHT:
				{
					// Unsynchronized "engine".
				}
				break;
				default:
				{
					assert(false);
				}
				break;
			}
		}
	}
}

Lepra::Vector3DF ContextObject::GetPosition() const
{
	Lepra::Vector3DF lPosition;
	if (mRootPhysicsIndex >= 0)
	{
		Lepra::TransformationF lTransform;
		mManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(mPhysicsNodeArray[mRootPhysicsIndex].GetBodyId(), lTransform);
		lPosition.x = lTransform.GetPosition().x;
		lPosition.y = lTransform.GetPosition().y;
		lPosition.z = lTransform.GetPosition().z;
	}
	else
	{
		assert(false);
		// TODO: throw something here...
	}
	return (lPosition);
}



void ContextObject::AddPhysicsObject(const PhysicsNode& pPhysicsNode)
{
	mPhysicsNodeArray.push_back(pPhysicsNode);
	if (pPhysicsNode.GetParentId() == 0)
	{
		assert(mRootPhysicsIndex == -1);
		mRootPhysicsIndex = (int)mPhysicsNodeArray.size()-1;
		//assert(mRootPhysicsIndex == 0);
	}
	mManager->AddPhysicsBody(this, pPhysicsNode.GetBodyId());
}

PhysicsNode* ContextObject::GetPhysicsNode(PhysicsNode::Id pId) const
{
	PhysicsNode* lNode = 0;
	PhysicsNodeArray::const_iterator x = mPhysicsNodeArray.begin();
	for (; !lNode && x != mPhysicsNodeArray.end(); ++x)
	{
		if (x->GetId() == pId)
		{
			lNode = (PhysicsNode*)&(*x);
		}
	}
	return (lNode);
}

PhysicsNode* ContextObject::GetPhysicsNode(TBC::PhysicsEngine::BodyID pBodyId) const
{
	PhysicsNode* lNode = 0;
	PhysicsNodeArray::const_iterator x = mPhysicsNodeArray.begin();
	for (; !lNode && x != mPhysicsNodeArray.end(); ++x)
	{
		if (x->GetBodyId() == pBodyId)
		{
			lNode = (PhysicsNode*)&(*x);
		}
	}
	return (lNode);
}

void ContextObject::SetEnginePower(unsigned pAspect, float pPower, float pAngle)
{
	AttributeArray::iterator x = mAttributeArray.begin();
	for (; x != mAttributeArray.end(); ++x)
	{
		ContextObjectAttribute* lAttribute = *x;
		if (lAttribute->GetType() == ContextObjectAttribute::TYPE_ENGINE)
		{
			ContextObjectEngine* lEngine = (ContextObjectEngine*)lAttribute;
			lEngine->SetValue(pAspect, pPower, pAngle);
		}
	}
}



void ContextObject::StepGhost(ObjectPositionalData& pGhost, float pDeltaTime)
{
	// We just add acceleration to velocity, and velocity to position. An improvement
	// here might save us a few bytes of network data.
	pGhost.mPosition.mVelocity.Add(pGhost.mPosition.mAcceleration*pDeltaTime);
	pGhost.mPosition.mTransformation.GetPosition().Add(pGhost.mPosition.mVelocity*pDeltaTime);
}



void ContextObject::OnPhysicsTick()
{
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ContextObject);



}
