
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/Vector3D.h"
//#include "ChunkyLoader.h"
#include "PhysicsManager.h"



namespace Tbc
{



class ChunkyPhysics;



class ChunkyBoneGeometry
{
public:
	enum BoneType
	{
		BONE_BODY = 1,
		BONE_TRIGGER,
		BONE_POSITION,
	};

	enum GeometryType
	{
		GEOMETRY_CAPSULE = 1,
		GEOMETRY_CYLINDER,
		GEOMETRY_SPHERE,
		GEOMETRY_BOX,
		GEOMETRY_MESH,
	};

	enum JointType
	{
		JOINT_EXCLUDE = 1,
		JOINT_SUSPEND_HINGE,
		JOINT_HINGE2,
		JOINT_HINGE,
		JOINT_BALL,
		JOINT_SLIDER,
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
			JointType pJointType, bool pIsAffectedByGravity, BoneType pBoneType):
			mMass(pMass),
			mFriction(pFriction),
			mBounce(pBounce),
			mParent(pParent),
			mJointType(pJointType),
			mIsAffectedByGravity(pIsAffectedByGravity),
			mBoneType(pBoneType)
		{
			::memset(mParameter, 0, sizeof(mParameter));
		}
		float mMass;
		float mFriction;
		float mBounce;
		ChunkyBoneGeometry* mParent;
		JointType mJointType;
		bool mIsAffectedByGravity;
		BoneType mBoneType;
		float mParameter[16];
	};
	struct BodyData: public BodyDataBase
	{
		BodyData(float pMass, float pFriction, float pBounce, ChunkyBoneGeometry* pParent = 0,
			JointType pJointType = JOINT_EXCLUDE, ConnectorType pConnectorType = CONNECT_NONE,
			bool pIsAffectedByGravity = true, BoneType pBoneType = BONE_BODY):
			BodyDataBase(pMass, pFriction, pBounce, pParent, pJointType, pIsAffectedByGravity, pBoneType),
			mConnectorType(pConnectorType)
		{
		}
		ConnectorType mConnectorType;
	};

	ChunkyBoneGeometry(const BodyData& pBodyData);
	virtual ~ChunkyBoneGeometry();
	virtual void RelocatePointers(const ChunkyPhysics* pTarget, const ChunkyPhysics* pSource, const ChunkyBoneGeometry& pOriginal);

	static ChunkyBoneGeometry* Create(const ChunkyBoneGeometry& pOriginal);
	static ChunkyBoneGeometry* Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount);

	virtual GeometryType GetGeometryType() const = 0;

	bool CreateJoint(ChunkyPhysics* pStructure, PhysicsManager* pPhysics, unsigned pPhysicsFps);
	virtual bool CreateBody(PhysicsManager* pPhysics, bool pIsRoot,
		int pForceListenerId, PhysicsManager::BodyType pType,
		const xform& pTransform) = 0;
	virtual bool CreateTrigger(PhysicsManager* pPhysics, int pTrigListenerId,
		const xform& pTransform) = 0;
	void RemovePhysics(PhysicsManager* pPhysics);

	float GetMass() const;
	ChunkyBoneGeometry* GetParent() const;
	JointType GetJointType() const;
	bool IsAffectedByGravity() const;
	bool IsCollideWithSelf() const;
	BoneType GetBoneType() const;
	PhysicsManager::JointID GetJointId() const;
	void ResetJointId();
	PhysicsManager::BodyID GetBodyId() const;
	void ResetBodyId();
	PhysicsManager::TriggerID GetTriggerId() const;
	bool IsConnectorType(ConnectorType pType) const;
	void AddConnectorType(ConnectorType pType);
	vec3 GetOriginalOffset() const;
	float GetImpactFactor() const;
	const str& GetMaterial() const;
	void SetMaterial(const str& pMaterial);

	float GetExtraData() const;
	void SetExtraData(float pExtraData);

	virtual unsigned GetChunkySize(const void* pData = 0) const;
	virtual void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

	virtual vec3 GetShapeSize() const = 0;

protected:
	virtual void LoadChunkyData(ChunkyPhysics* pStructure, const void* pData);

	typedef std::vector<ConnectorType> ConnectorArray;

	BodyDataBase mBodyData;
	PhysicsManager::JointID mJointId;
	PhysicsManager::BodyID mBodyId;
	PhysicsManager::TriggerID mTriggerId;
	ConnectorArray mConnectorArray;
	str mMaterial;
	float mExtraData;

	logclass();
};



class ChunkyBoneCapsule: public ChunkyBoneGeometry
{
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneCapsule(const BodyData& pBodyData);
	virtual bool CreateBody(PhysicsManager* pPhysics, bool pIsRoot, int pForceListenerId,
		PhysicsManager::BodyType pType, const xform& pTransform);
	virtual bool CreateTrigger(PhysicsManager* pPhysics, int pTrigListenerId,
		const xform& pTransform);

	virtual unsigned GetChunkySize(const void* pData = 0) const;
	virtual void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

protected:
	virtual void LoadChunkyData(ChunkyPhysics* pStructure, const void* pData);
	virtual vec3 GetShapeSize() const;
	virtual GeometryType GetGeometryType() const;

	float32 mRadius;
	float32 mLength;
};



class ChunkyBoneCylinder: public ChunkyBoneCapsule
{
	typedef ChunkyBoneCapsule Parent;
public:
	ChunkyBoneCylinder(const BodyData& pBodyData);
	virtual bool CreateBody(PhysicsManager* pPhysics, bool pIsRoot, int pForceListenerId,
		PhysicsManager::BodyType pType, const xform& pTransform);
	virtual bool CreateTrigger(PhysicsManager* pPhysics, int pTrigListenerId,
		const xform& pTransform);

private:
	virtual vec3 GetShapeSize() const;
	virtual GeometryType GetGeometryType() const;
};



class ChunkyBoneSphere: public ChunkyBoneGeometry
{
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneSphere(const BodyData& pBodyData);
	virtual bool CreateBody(PhysicsManager* pPhysics, bool pIsRoot, int pForceListenerId,
		PhysicsManager::BodyType pType, const xform& pTransform);
	virtual bool CreateTrigger(PhysicsManager* pPhysics, int pTrigListenerId,
		const xform& pTransform);

	virtual unsigned GetChunkySize(const void* pData = 0) const;
	virtual void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

private:
	virtual vec3 GetShapeSize() const;
	virtual void LoadChunkyData(ChunkyPhysics* pStructure, const void* pData);
	virtual GeometryType GetGeometryType() const;

	float32 mRadius;
};



class ChunkyBoneBox: public ChunkyBoneGeometry
{
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneBox(const BodyData& pBodyData);
	virtual bool CreateBody(PhysicsManager* pPhysics, bool pIsRoot, int pForceListenerId,
		PhysicsManager::BodyType pType, const xform& pTransform);
	virtual bool CreateTrigger(PhysicsManager* pPhysics, int pTrigListenerId,
		const xform& pTransform);

	virtual unsigned GetChunkySize(const void* pData = 0) const;
	virtual void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

	virtual vec3 GetShapeSize() const;

private:
	virtual void LoadChunkyData(ChunkyPhysics* pStructure, const void* pData);
	virtual GeometryType GetGeometryType() const;

	vec3 mSize;
};



class ChunkyBoneMesh: public ChunkyBoneGeometry
{
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneMesh(const BodyData& pBodyData);
	virtual ~ChunkyBoneMesh();
	virtual bool CreateBody(PhysicsManager* pPhysics, bool pIsRoot, int pForceListenerId,
		PhysicsManager::BodyType pType, const xform& pTransform);
	virtual bool CreateTrigger(PhysicsManager* pPhysics, int pTrigListenerId,
		const xform& pTransform);

	virtual unsigned GetChunkySize(const void* pData = 0) const;
	virtual void SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const;

private:
	virtual vec3 GetShapeSize() const;
	virtual void LoadChunkyData(ChunkyPhysics* pStructure, const void* pData);
	void Clear();
	virtual GeometryType GetGeometryType() const;
	virtual void RelocatePointers(const ChunkyPhysics* pTarget, const ChunkyPhysics* pSource, const ChunkyBoneGeometry& pOriginal);

	uint32 mVertexCount;
	float* mVertices;
	uint32 mTriangleCount;
	uint32* mIndices;
};



}
