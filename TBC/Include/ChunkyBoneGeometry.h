
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/Vector3D.h"
#include "ChunkyLoader.h"
#include "PhysicsEngine.h"



namespace TBC
{



class ChunkyBoneGeometry
{
public:
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
		CONNECTOR_3DOF,	// Three degrees of freedom.
		CONNECTEE_3DOF,
	};

	struct BodyDataBase
	{
		BodyDataBase(PhysicsEngine::TriggerListener* pTriggerListener,
			PhysicsEngine::ForceFeedbackListener* pForceFeedbackListener,
			ChunkyBoneGeometry* pParent, JointType pJointType, bool pIsAffectedByGravity):
			mTriggerListener(pTriggerListener),
			mForceFeedbackListener(pForceFeedbackListener),
			mParent(pParent),
			mJointType(pJointType),
			mIsAffectedByGravity(pIsAffectedByGravity)
		{
			::memset(mParameter, 0, sizeof(mParameter));
		}
		PhysicsEngine::TriggerListener* mTriggerListener;
		PhysicsEngine::ForceFeedbackListener* mForceFeedbackListener;
		ChunkyBoneGeometry* mParent;
		JointType mJointType;
		bool mIsAffectedByGravity;
		float mParameter[16];
	};
	struct BodyData: public BodyDataBase
	{
		BodyData(PhysicsEngine::TriggerListener* pTriggerListener,
			PhysicsEngine::ForceFeedbackListener* pForceFeedbackListener,
			ChunkyBoneGeometry* pParent = 0, JointType pJointType = TYPE_EXCLUDE,
			ConnectorType pConnectorType = CONNECT_NONE, bool pIsAffectedByGravity = true):
			BodyDataBase(pTriggerListener, pForceFeedbackListener, pParent, pJointType, pIsAffectedByGravity),
			mConnectorType(pConnectorType)
		{
		}
		ConnectorType mConnectorType;
	};

	ChunkyBoneGeometry(const BodyData& pBodyData);
	virtual ~ChunkyBoneGeometry();

	bool CreateJoint(ChunkyStructure* pStructure, PhysicsEngine* pPhysics);
	virtual bool CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform) = 0;
	virtual bool CreateTrigger(PhysicsEngine* pPhysics, const Lepra::TransformationF& pTransform) = 0;
	void RemovePhysics(PhysicsEngine* pPhysics);

	ChunkyBoneGeometry* GetParent() const;
	JointType GetJointType() const;
	TBC::PhysicsEngine::JointID GetJointId() const;
	TBC::PhysicsEngine::BodyID GetBodyId() const;
	TBC::PhysicsEngine::TriggerID GetTriggerId() const;
	bool IsConnectorType(ConnectorType pType) const;
	void AddConnectorType(ConnectorType pType);

	float GetExtraData() const;
	void SetExtraData(float pExtraData);

	virtual ChunkyType GetChunkyType() const = 0;
	virtual unsigned GetChunkySize() const = 0;
	virtual void SaveChunkyData(void* pData) const = 0;
	virtual void LoadChunkyData(const void* pData) = 0;

protected:
	typedef std::vector<ConnectorType> ConnectorArray;

	BodyDataBase mBodyData;
	TBC::PhysicsEngine::JointID mJointId;
	PhysicsEngine::BodyID mBodyId;
	PhysicsEngine::TriggerID mTriggerId;
	ConnectorArray mConnectorArray;
	float mExtraData;
};



class ChunkyBoneCapsule: public ChunkyBoneGeometry
{
public:
	ChunkyBoneCapsule(const BodyData& pBodyData, Lepra::float32 pMass, Lepra::float32 pRadius,
		Lepra::float32 pLength, Lepra::float32 pFriction = 1, Lepra::float32 pBounce = 0);
	bool CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform);
	bool CreateTrigger(PhysicsEngine* pPhysics, const Lepra::TransformationF& pTransform);

	ChunkyType GetChunkyType() const;
	unsigned GetChunkySize() const;
	void SaveChunkyData(void* pData) const;
	void LoadChunkyData(const void* pData);

private:
	Lepra::float32 mMass;
	Lepra::float32 mRadius;
	Lepra::float32 mLength;
	Lepra::float32 mFriction;
	Lepra::float32 mBounce;
};



class ChunkyBoneSphere: public ChunkyBoneGeometry
{
public:
	ChunkyBoneSphere(const BodyData& pBodyData, Lepra::float32 pMass, Lepra::float32 pRadius,
		Lepra::float32 pFriction = 1, Lepra::float32 pBounce = 0);
	bool CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform);
	bool CreateTrigger(PhysicsEngine* pPhysics, const Lepra::TransformationF& pTransform);

	ChunkyType GetChunkyType() const;
	unsigned GetChunkySize() const;
	void SaveChunkyData(void* pData) const;
	void LoadChunkyData(const void* pData);

private:
	Lepra::float32 mMass;
	Lepra::float32 mRadius;
	Lepra::float32 mFriction;
	Lepra::float32 mBounce;
};



class ChunkyBoneBox: public ChunkyBoneGeometry
{
public:
	ChunkyBoneBox(const BodyData& pBodyData, Lepra::float32 pMass, const Lepra::Vector3DF& pSize,
		Lepra::float32 pFriction = 1, Lepra::float32 pBounce = 0);
	bool CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform);
	bool CreateTrigger(PhysicsEngine* pPhysics, const Lepra::TransformationF& pTransform);

	ChunkyType GetChunkyType() const;
	unsigned GetChunkySize() const;
	void SaveChunkyData(void* pData) const;
	void LoadChunkyData(const void* pData);

private:
	Lepra::float32 mMass;
	Lepra::Vector3DF mSize;
	Lepra::float32 mFriction;
	Lepra::float32 mBounce;
};



}
