
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../tbc/include/physicsmanager.h"
#include "../../lepra/include/unordered.h"
#include <list>
#include <vector>
#include "../../lepra/include/string.h"
#include "cure.h"
#include "positionaldata.h"



namespace tbc {
class ChunkyPhysics;
class ChunkyBoneGeometry;
class PhysicsEngine;
class PhysicsSpawner;
class PhysicsTrigger;
}



namespace cure {



class ContextManager;
class ContextObjectAttribute;
class ResourceManager;


class ContextObject {
public:
	typedef std::vector<ContextObject*> Array;
	typedef std::vector<ContextObjectAttribute*> AttributeArray;

	ContextObject(ResourceManager* resource_manager, const str& class_id);
	virtual ~ContextObject();

	ContextManager* GetManager() const;
	virtual void SetManager(ContextManager* manager);

	GameObjectId GetInstanceId() const;
	void SetInstanceId(GameObjectId instance_id);
	const str& GetClassId() const;
	GameObjectId GetOwnerInstanceId() const;
	void SetOwnerInstanceId(GameObjectId instance_id);
	GameObjectId GetBorrowerInstanceId() const;
	void SetBorrowerInstanceId(GameObjectId instance_id);

	NetworkObjectType GetNetworkObjectType() const;
	void SetNetworkObjectType(NetworkObjectType type);

	void* GetExtraData() const;
	void SetExtraData(void* data);

	bool IsLoaded() const;
	void SetLoadResult(bool ok);

	void SetAllowMoveRoot(bool allow);
	void AttachToObjectByBodyIds(tbc::PhysicsManager::BodyID body1, ContextObject* object2, tbc::PhysicsManager::BodyID body2);
	void AttachToObjectByBodyIndices(unsigned body1_index, ContextObject* object2, unsigned body2_index);
	void DetachAll();
	bool DetachFromObject(ContextObject* object);
	Array GetAttachedObjects() const;
	void AddAttachedObjectEngine(ContextObject* attached_object, tbc::PhysicsEngine* engine);

	void AddAttribute(ContextObjectAttribute* attribute);
	void DeleteAttribute(const str& name);
	ContextObjectAttribute* GetAttribute(const str& name) const;
	const AttributeArray& GetAttributes() const;
	float GetAttributeFloatValue(const str& attribute_name) const;
	void QuerySetChildishness(float childishness);
	bool IsAttributeTrue(const str& attribute_name) const;
	void OnAttributeUpdated(ContextObjectAttribute* attribute);

	void AddTrigger(tbc::PhysicsManager::BodyID trigger_id, const void*);
	virtual void FinalizeTrigger(const tbc::PhysicsTrigger* trigger);
	const void* GetTrigger(tbc::PhysicsManager::BodyID trigger_id) const;
	size_t GetTriggerCount(const void*& trigger) const;

	virtual void SetSpawner(const tbc::PhysicsSpawner* spawner);
	const tbc::PhysicsSpawner* GetSpawner() const;

	void AddChild(ContextObject* child);
	const Array& GetChildArray() const;

	bool UpdateFullPosition(const ObjectPositionalData*& positional_data);	// Fetch full phys position (and update object graph as necessary).
	static bool UpdateFullPosition(ObjectPositionalData& position, tbc::PhysicsManager* physics_manager, tbc::ChunkyPhysics* structure);
	void SetFullPosition(const ObjectPositionalData& positional_data, float delta_threshold);	// Sets full phys position if structure or significant difference seen.
	virtual void SetPositionFinalized();
	void SetInitialTransform(const xform& transformation);
	xform GetInitialTransform() const;
	void SetInitialPositionalData(const ObjectPositionalData& positional_data);
	vec3 GetPosition() const;
	void SetRootPosition(const vec3& position);
	vec3 GetRootPosition() const;
	void SetRootOrientation(const quat& orientation);
	void SetRootVelocity(const vec3& velocity);
	quat GetOrientation() const;
	vec3 GetVelocity() const;
	vec3 GetAngularVelocity() const;
	vec3 GetAcceleration() const;
	vec3 GetForwardDirection() const;
	float GetForwardSpeed() const;
	float GetMass() const;
	float QueryMass();
	void SetMass(float mass);
	ObjectPositionalData* GetNetworkOutputGhost();
	void DeleteNetworkOutputGhost();

	void SetPhysics(tbc::ChunkyPhysics* structure);
	void ClearPhysics();
	tbc::ChunkyPhysics* GetPhysics() const;
	void SetPhysicsTypeOverride(PhysicsOverride physics_override);
	tbc::ChunkyBoneGeometry* GetStructureGeometry(unsigned index) const;
	tbc::ChunkyBoneGeometry* GetStructureGeometry(tbc::PhysicsManager::BodyID body_id) const;
	bool SetEnginePower(unsigned aspect, float power);
	float GetImpact(const vec3& gravity, const vec3& force, const vec3& torque, float extra_mass = 0, float sideways_factor = 1) const;

	void ForceSend();
	bool QueryResendTime(float delta_time, bool unblock_delta);
	int PopSendCount();
	void SetSendCount(int count);

	virtual void StartLoading() = 0;
	virtual void OnLoaded();
	virtual void OnMicroTick(float frame_time) = 0;
	virtual void OnAlarm(int alarm_id, void* extra_data) = 0;
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) = 0;
	virtual void OnForceApplied(ContextObject* other_object,
		 tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
		 const vec3& force, const vec3& torque,
		 const vec3& position, const vec3& relative_velocity) = 0;
	virtual void OnTick();

protected:
	void ForceSetFullPosition(const ObjectPositionalData& positional_data);
	void AttachToObject(tbc::ChunkyBoneGeometry* bone_geometry1, ContextObject* object2, tbc::ChunkyBoneGeometry* bone_geometry2, bool send);
	bool IsAttachedTo(ContextObject* object) const;
	void AddAttachment(ContextObject* object, tbc::PhysicsManager::JointID joint, tbc::PhysicsEngine* engine);

	void RemoveChild(ContextObject* child);
	void SetParent(ContextObject* parent);
	virtual void SetupChildHandlers();

	ResourceManager* GetResourceManager() const;

	typedef std::vector<tbc::PhysicsEngine*> EngineList;
	struct Connection {
		Connection(ContextObject* object, tbc::PhysicsManager::JointID joint_id, tbc::PhysicsEngine* engine=0):
			object_(object),
			joint_id_(joint_id) {
			if (engine) {
				engine_list_.push_back(engine);
			}
		}
		ContextObject* object_;
		tbc::PhysicsManager::JointID joint_id_;
		EngineList engine_list_;
	};
	typedef std::list<Connection> ConnectionList;
	typedef std::unordered_map<tbc::PhysicsManager::BodyID, const void*> TriggerMap;

	ContextManager* manager_;
	ResourceManager* resource_manager_;
	GameObjectId instance_id_;
	GameObjectId owner_instance_id_;
	GameObjectId borrower_instance_id_;
	str class_id_;
	NetworkObjectType network_object_type_;
	ContextObject* parent_;
	void* extra_data_;
	Array child_array_;
	TriggerMap trigger_map_;
	const tbc::PhysicsSpawner* spawner_;
	bool is_loaded_;
	AttributeArray attribute_array_;
	tbc::ChunkyPhysics* physics_;
	PhysicsOverride physics_override_;
	float total_mass_;
	float last_send_time_;
	ObjectPositionalData position_;
	ObjectPositionalData* network_output_ghost_;
	int send_count_;
	bool allow_move_root_;	// This is set to false when attached to someone/something else.
	ConnectionList connection_list_;

	logclass();
};



}
