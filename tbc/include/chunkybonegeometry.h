
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/vector3d.h"
//#include "ChunkyLoader.h"
#include "physicsmanager.h"



namespace tbc {



class ChunkyPhysics;



class ChunkyBoneGeometry {
public:
	enum BoneType {
		kBoneBody = 1,
		kBoneTrigger,
		kBonePosition,
	};

	enum GeometryType {
		kGeometryCapsule = 1,
		kGeometryCylinder,
		kGeometrySphere,
		kGeometryBox,
		kGeometryMesh,
	};

	enum JointType {
		kJointExclude = 1,
		kJointFixed,
		kJointSuspendHinge,
		kJointHinge2,
		kJointHinge,
		kJointBall,
		kJointSlider,
		kJointUniversal,
	};

	enum ConnectorType {
		kConnectNone = 0,
		kConnectorSlider,
		kConnectorUniversal,
		kConnectorSuspendHinge,
		kConnectorHinge,
		kConnectorHinge2,
		kConnector3Dof,	// Three degrees of freedom.
		kConnectorFixed,
		kConnectee3Dof,
	};

	enum BodyParameters {
		kParamSpringConstant = 0,
		kParamSpringDamping,
		kParamEulerTheta,
		kParamEulerPhi,
		kParamLowStop,
		kParamHighStop,
		kParamOffsetX,
		kParamOffsetY,
		kParamOffsetZ,
		kParamImpactFactor,
		kParamCollideWithSelf,
		kParamDetachable,
	};

	struct BodyDataBase {
		BodyDataBase(float mass, float friction, float bounce, ChunkyBoneGeometry* parent,
			JointType joint_type, bool is_affected_by_gravity, BoneType bone_type):
			mass_(mass),
			friction_(friction),
			bounce_(bounce),
			parent_(parent),
			joint_type_(joint_type),
			is_affected_by_gravity_(is_affected_by_gravity),
			bone_type_(bone_type) {
			::memset(parameter_, 0, sizeof(parameter_));
		}
		float mass_;
		float friction_;
		float bounce_;
		ChunkyBoneGeometry* parent_;
		JointType joint_type_;
		bool is_affected_by_gravity_;
		BoneType bone_type_;
		float parameter_[16];
	};
	struct BodyData: public BodyDataBase {
		BodyData(float mass, float friction, float bounce, ChunkyBoneGeometry* parent = 0,
			JointType joint_type = kJointExclude, ConnectorType connector_type = kConnectNone,
			bool is_affected_by_gravity = true, BoneType bone_type = kBoneBody):
			BodyDataBase(mass, friction, bounce, parent, joint_type, is_affected_by_gravity, bone_type),
			connector_type_(connector_type) {
		}
		ConnectorType connector_type_;
	};

	ChunkyBoneGeometry(const BodyData& body_data);
	virtual ~ChunkyBoneGeometry();
	virtual void RelocatePointers(const ChunkyPhysics* target, const ChunkyPhysics* source, const ChunkyBoneGeometry& original);

	static ChunkyBoneGeometry* Create(const ChunkyBoneGeometry& original);
	static ChunkyBoneGeometry* Load(ChunkyPhysics* structure, const void* data, unsigned byte_count);

	virtual GeometryType GetGeometryType() const = 0;

	bool CreateJoint(ChunkyPhysics* structure, PhysicsManager* physics, unsigned physics_fps);
	virtual bool CreateBody(PhysicsManager* physics, bool is_root,
		int force_listener_id, PhysicsManager::BodyType type,
		const xform& transform) = 0;
	void RemovePhysics(PhysicsManager* physics);

	float GetMass() const;
	ChunkyBoneGeometry* GetParent() const;
	JointType GetJointType() const;
	void SetJointType(JointType joint_type);
	bool IsAffectedByGravity() const;
	bool IsCollideWithSelf() const;
	bool IsDetachable() const;
	BoneType GetBoneType() const;
	PhysicsManager::JointID GetJointId() const;
	void ResetJointId();
	void SetJointId(PhysicsManager::JointID joint_id);
	PhysicsManager::BodyID GetBodyId() const;
	void ResetBodyId();
	bool IsConnectorType(ConnectorType type) const;
	void AddConnectorType(ConnectorType type);
	void ClearConnectorTypes();
	vec3 GetOriginalOffset() const;
	float GetImpactFactor() const;
	const str& GetMaterial() const;
	void SetMaterial(const str& material);

	float GetExtraData() const;
	void SetExtraData(float extra_data);

	BodyDataBase& GetBodyData();	// Only use when fiddling with internals.

	virtual unsigned GetChunkySize(const void* data = 0) const;
	virtual void SaveChunkyData(const ChunkyPhysics* structure, void* data) const;

	virtual vec3 GetShapeSize() const = 0;

protected:
	virtual void LoadChunkyData(ChunkyPhysics* structure, const void* data);

	typedef std::vector<ConnectorType> ConnectorArray;

	BodyDataBase body_data_;
	PhysicsManager::JointID joint_id_;
	PhysicsManager::BodyID body_id_;
	PhysicsManager::BodyID trigger_id_;
	ConnectorArray connector_array_;
	str material_;
	float extra_data_;

	logclass();
};



class ChunkyBoneCapsule: public ChunkyBoneGeometry {
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneCapsule(const BodyData& body_data);
	virtual bool CreateBody(PhysicsManager* physics, bool is_root, int force_listener_id,
		PhysicsManager::BodyType type, const xform& transform);

	virtual unsigned GetChunkySize(const void* data = 0) const;
	virtual void SaveChunkyData(const ChunkyPhysics* structure, void* data) const;

	virtual void LoadChunkyData(ChunkyPhysics* structure, const void* data);
	virtual vec3 GetShapeSize() const;
	virtual GeometryType GetGeometryType() const;

	float32 radius_;
	float32 length_;
};



class ChunkyBoneCylinder: public ChunkyBoneCapsule {
	typedef ChunkyBoneCapsule Parent;
public:
	ChunkyBoneCylinder(const BodyData& body_data);
	virtual bool CreateBody(PhysicsManager* physics, bool is_root, int force_listener_id,
		PhysicsManager::BodyType type, const xform& transform);

private:
	virtual vec3 GetShapeSize() const;
	virtual GeometryType GetGeometryType() const;
};



class ChunkyBoneSphere: public ChunkyBoneGeometry {
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneSphere(const BodyData& body_data);
	virtual bool CreateBody(PhysicsManager* physics, bool is_root, int force_listener_id,
		PhysicsManager::BodyType type, const xform& transform);

	virtual unsigned GetChunkySize(const void* data = 0) const;
	virtual void SaveChunkyData(const ChunkyPhysics* structure, void* data) const;

	virtual vec3 GetShapeSize() const;
	virtual void LoadChunkyData(ChunkyPhysics* structure, const void* data);
	virtual GeometryType GetGeometryType() const;

	float32 radius_;
};



class ChunkyBoneBox: public ChunkyBoneGeometry {
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneBox(const BodyData& body_data);
	virtual bool CreateBody(PhysicsManager* physics, bool is_root, int force_listener_id,
		PhysicsManager::BodyType type, const xform& transform);

	virtual unsigned GetChunkySize(const void* data = 0) const;
	virtual void SaveChunkyData(const ChunkyPhysics* structure, void* data) const;

	virtual vec3 GetShapeSize() const;

	virtual void LoadChunkyData(ChunkyPhysics* structure, const void* data);
	virtual GeometryType GetGeometryType() const;

	vec3 size_;
};



class ChunkyBoneMesh: public ChunkyBoneGeometry {
	typedef ChunkyBoneGeometry Parent;
public:
	ChunkyBoneMesh(const BodyData& body_data);
	virtual ~ChunkyBoneMesh();
	virtual bool CreateBody(PhysicsManager* physics, bool is_root, int force_listener_id,
		PhysicsManager::BodyType type, const xform& transform);

	virtual unsigned GetChunkySize(const void* data = 0) const;
	virtual void SaveChunkyData(const ChunkyPhysics* structure, void* data) const;

	virtual vec3 GetShapeSize() const;
	virtual void LoadChunkyData(ChunkyPhysics* structure, const void* data);
	void Clear();
	virtual GeometryType GetGeometryType() const;
	virtual void RelocatePointers(const ChunkyPhysics* target, const ChunkyPhysics* source, const ChunkyBoneGeometry& original);

	uint32 vertex_count_;
	float* vertices_;
	uint32 triangle_count_;
	uint32* indices_;
};



}
