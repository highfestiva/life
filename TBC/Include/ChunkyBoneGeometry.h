
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
	enum GeometryType
	{
		GEOMETRY_CAPSULE = 1,
		GEOMETRY_SPHERE,
		GEOMETRY_BOX,
	};

	enum JointType
	{
		JOINT_EXCLUDE = 1,
		JOINT_SUSPEND_HINGE,
		JOINT_HINGE2,
		JOINT_HINGE,
		JOINT_BALL,
		JOINT_UNIVERSAL,
	};

	enum ConnectorType
	{
		CONNECT_NONE = 0,
		CONNECTOR_3DOF,	// Three degrees of freedom.
		CONNECTEE_3DOF,
	};

	struct BodyDataBase
	{
		BodyDataBase(float pMass, float pFriction, float pBounce, ChunkyBoneGeometry* pParent,
			JointType pJointType, bool pIsAffectedByGravity):
			mMass(pMass),
			mFriction(pFriction),
			mBounce(pBounce),
			mParent(pParent),
			mJointType(pJointType),
			mIsAffectedByGravity(pIsAffectedByGravity)
		{
			::memset(mParameter, 0, sizeof(mParameter));
		}
		float mMass;
		float mFriction;
		float mBounce;
		ChunkyBoneGeometry* mParent;
		JointType mJointType;
		bool mIsAffectedByGravity;
		float mParameter[16];
	};
	struct BodyData: public BodyDataBase
	{
		BodyData(float pMass, float pFriction, float pBounce, ChunkyBoneGeometry* pParent = 0,
			JointType pJointType = JOINT_EXCLUDE, ConnectorType pConnectorType = CONNECT_NONE,
			bool pIsAffectedByGravity = true):
			BodyDataBase(pMass, pFriction, pBounce, pParent, pJointType, pIsAffectedByGravity),
			mConnectorType(pConnectorType)
		{
		}
		ConnectorType mConnectorType;
	};

	ChunkyBoneGeometry(const BodyData& pBodyData);
	virtual ~ChunkyBoneGeometry();

	static ChunkyBoneGeometry* Load(ChunkyStructure* pStructure, const void* pData, unsigned pByteCount);

	bool CreateJoint(ChunkyStructure* pStructure, PhysicsEngine* pPhysics, unsigned pPhysicsFps);
	virtual bool CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::TriggerListener* pTrigListener,
		PhysicsEngine::ForceFeedbackListener* pForceListener, PhysicsEngine::BodyType pType,
		const Lepra::TransformationF& pTransform) = 0;
	virtual bool CreateTrigger(PhysicsEngine* pPhysics, PhysicsEngine::TriggerListener* pTrigListener,
		const Lepra::TransformationF& pTransform) = 0;
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

	virtual unsigned GetChunkySize(const void* pData = 0) const;
	virtual void SaveChunkyData(const ChunkyStructure* pStructure, void* pData) const;

protected:
	virtual void LoadChunkyData(ChunkyStructure* pStructure, const void* pData);
	virtual GeometryType GetGeometryType() const = 0;

	typedef std::vector<ConnectorType> ConnectorArray;

	BodyDataBase mBodyData;
	TBC::PhysicsEngine::JointID mJointId;
	PhysicsEngine::BodyID mBodyId;
	PhysicsEngine::TriggerID mTriggerId;
	ConnectorArray mConnectorArray;
	float mExtraData;

	LOG_CLASS_DECLARE();
};



class ChunkyBoneCapsule: public ChunkyBoneGeometry
{
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneCapsule(const BodyData& pBodyData, Lepra::float32 pRadius, Lepra::float32 pLength);
	bool CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::TriggerListener* pTrigListener,
		PhysicsEngine::ForceFeedbackListener* pForceListener, PhysicsEngine::BodyType pType,
		const Lepra::TransformationF& pTransform);
	bool CreateTrigger(PhysicsEngine* pPhysics, PhysicsEngine::TriggerListener* pTrigListener,
		const Lepra::TransformationF& pTransform);

	unsigned GetChunkySize(const void* pData = 0) const;
	void SaveChunkyData(const ChunkyStructure* pStructure, void* pData) const;

private:
	void LoadChunkyData(ChunkyStructure* pStructure, const void* pData);
	GeometryType GetGeometryType() const;

	Lepra::float32 mRadius;
	Lepra::float32 mLength;
};



class ChunkyBoneSphere: public ChunkyBoneGeometry
{
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneSphere(const BodyData& pBodyData, Lepra::float32 pRadius);
	bool CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::TriggerListener* pTrigListener,
		PhysicsEngine::ForceFeedbackListener* pForceListener, PhysicsEngine::BodyType pType,
		const Lepra::TransformationF& pTransform);
	bool CreateTrigger(PhysicsEngine* pPhysics, PhysicsEngine::TriggerListener* pTrigListener,
		const Lepra::TransformationF& pTransform);

	unsigned GetChunkySize(const void* pData = 0) const;
	void SaveChunkyData(const ChunkyStructure* pStructure, void* pData) const;

private:
	void LoadChunkyData(ChunkyStructure* pStructure, const void* pData);
	GeometryType GetGeometryType() const;

	Lepra::float32 mRadius;
};



class ChunkyBoneBox: public ChunkyBoneGeometry
{
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneBox(const BodyData& pBodyData, const Lepra::Vector3DF& pSize);
	bool CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::TriggerListener* pTrigListener,
		PhysicsEngine::ForceFeedbackListener* pForceListener, PhysicsEngine::BodyType pType,
		const Lepra::TransformationF& pTransform);
	bool CreateTrigger(PhysicsEngine* pPhysics, PhysicsEngine::TriggerListener* pTrigListener,
		const Lepra::TransformationF& pTransform);

	unsigned GetChunkySize(const void* pData = 0) const;
	void SaveChunkyData(const ChunkyStructure* pStructure, void* pData) const;

private:
	void LoadChunkyData(ChunkyStructure* pStructure, const void* pData);
	GeometryType GetGeometryType() const;

	Lepra::Vector3DF mSize;
};



}
