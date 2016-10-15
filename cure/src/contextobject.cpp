
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/contextobject.h"
#include <algorithm>
#include <math.h>
#include "../../lepra/include/hashutil.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/random.h"
#include "../../lepra/include/resourcetracker.h"
#include "../../lepra/include/rotationmatrix.h"
#include "../../tbc/include/chunkybonegeometry.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../../tbc/include/physicsengine.h"
#include "../../tbc/include/physicsmanager.h"
#include "../../tbc/include/physicsspawner.h"
#include "../../tbc/include/physicstrigger.h"
#include "../include/contextmanager.h"
#include "../include/contextobjectattribute.h"
#include "../include/cure.h"
#include "../include/floatattribute.h"
#include "../include/gamemanager.h"
#include "../include/positionhauler.h"
#include "../include/timemanager.h"



namespace cure {



ContextObject::ContextObject(cure::ResourceManager* resource_manager, const str& class_id):
	manager_(0),
	resource_manager_(resource_manager),
	instance_id_(0),
	owner_instance_id_(0),
	borrower_instance_id_(0),
	class_id_(class_id),
	network_object_type_(kNetworkObjectLocalOnly),
	parent_(0),
	extra_data_(0),
	spawner_(0),
	is_loaded_(false),
	physics_(0),
	physics_override_(kPhysicsOverrideNormal),
	total_mass_(0),
	last_send_time_(-10000.0f),
	network_output_ghost_(0),
	send_count_(0),
	allow_move_root_(true) {
	deb_assert(!class_id.empty());
	LEPRA_ACQUIRE_RESOURCE(ContextObject);
}

ContextObject::~ContextObject() {
	deb_assert(!manager_ || Thread::GetCurrentThread()->GetThreadName() == "MainThread");

	log_volatile(log_.Tracef("Destructing context object %s.", class_id_.c_str()));

	DeleteNetworkOutputGhost();

	if (parent_) {
		parent_->RemoveChild(this);
		parent_ = 0;
	}

	for (Array::iterator x = child_array_.begin(); x != child_array_.end(); ++x) {
		(*x)->SetParent(0);
		delete (*x);
	}
	child_array_.clear();

	trigger_map_.clear();
	spawner_ = 0;

	if (manager_) {
		manager_->RemoveObject(this);
	}

	ClearPhysics();

	// Fuck off, attributes.
	{
		AttributeArray::iterator x = attribute_array_.begin();
		for (; x != attribute_array_.end(); ++x) {
			delete (*x);
		}
		attribute_array_.clear();
	}

	if (manager_) {
		manager_->FreeGameObjectId(network_object_type_, instance_id_);
	}
	instance_id_ = 0;

	LEPRA_RELEASE_RESOURCE(ContextObject);
}



ContextManager* ContextObject::GetManager() const {
	return (manager_);
}

void ContextObject::SetManager(ContextManager* manager) {
	deb_assert(manager_ == 0 || manager == 0);
	manager_ = manager;
}

GameObjectId ContextObject::GetInstanceId() const {
	return (instance_id_);
}

void ContextObject::SetInstanceId(GameObjectId instance_id) {
	deb_assert(instance_id_ == 0);
	instance_id_ = instance_id;
}

const str& ContextObject::GetClassId() const {
	return (class_id_);
}

GameObjectId ContextObject::GetOwnerInstanceId() const {
	return (owner_instance_id_);
}

void ContextObject::SetOwnerInstanceId(GameObjectId instance_id) {
	owner_instance_id_ = instance_id;
}

GameObjectId ContextObject::GetBorrowerInstanceId() const {
	return borrower_instance_id_;
}

void ContextObject::SetBorrowerInstanceId(GameObjectId instance_id) {
	borrower_instance_id_ = instance_id;
}



NetworkObjectType ContextObject::GetNetworkObjectType() const {
	return (network_object_type_);
}

void ContextObject::SetNetworkObjectType(NetworkObjectType type) {
	deb_assert((network_object_type_ == type) ||
		(network_object_type_ == kNetworkObjectLocallyControlled && type == kNetworkObjectRemoteControlled) ||
		(network_object_type_ == kNetworkObjectRemoteControlled && type == kNetworkObjectLocallyControlled) ||
		(network_object_type_ == kNetworkObjectLocalOnly));
	network_object_type_ = type;
}



void* ContextObject::GetExtraData() const {
	return (extra_data_);
}

void ContextObject::SetExtraData(void* data) {
	extra_data_ = data;
}



bool ContextObject::IsLoaded() const {
	return (is_loaded_);
}

void ContextObject::SetLoadResult(bool ok) {
	deb_assert(!is_loaded_);
	is_loaded_ = true;
	if (ok) {
		OnLoaded();
	}
	if (GetManager()) {
		GetManager()->GetGameManager()->OnLoadCompleted(this, ok);
	}
}



void ContextObject::SetAllowMoveRoot(bool allow) {
	allow_move_root_ = allow;
}

void ContextObject::AttachToObjectByBodyIds(tbc::PhysicsManager::BodyID body1, ContextObject* object2, tbc::PhysicsManager::BodyID body2) {
	if (IsAttachedTo(object2)) {
		return;
	}
	AttachToObject(GetStructureGeometry(body1), object2, object2->GetStructureGeometry(body2), true);
}

void ContextObject::AttachToObjectByBodyIndices(unsigned body1_index, ContextObject* object2, unsigned body2_index) {
	if (IsAttachedTo(object2)) {
		log_.Warningf("Object %i already attached to object %i!", GetInstanceId(), object2->GetInstanceId());
		return;
	}
	AttachToObject(physics_->GetBoneGeometry(body1_index), object2, object2->GetStructureGeometry(body2_index), false);
}

void ContextObject::DetachAll() {
	while (!connection_list_.empty()) {
		ContextObject* _object2 = connection_list_.front().object_;
		DetachFromObject(_object2);
	}
}

bool ContextObject::DetachFromObject(ContextObject* object) {
	bool removed = false;

	ConnectionList::iterator x = connection_list_.begin();
	for (; x != connection_list_.end(); ++x) {
		if (object == x->object_) {
			tbc::PhysicsManager::JointID joint_id = x->joint_id_;
			EngineList engine_list = x->engine_list_;
			connection_list_.erase(x);
			object->DetachFromObject(this);
			if (joint_id != tbc::INVALID_JOINT) {
				manager_->GetGameManager()->GetPhysicsManager()->DeleteJoint(joint_id);
				manager_->GetGameManager()->SendDetach(this, object);
			}
			const int bone_count = object->GetPhysics()->GetBoneCount();
			for (int y = 0; y < bone_count; ++y) {
				if (object->GetPhysics()->GetBoneGeometry(y)->GetJointId() == joint_id) {
					object->GetPhysics()->GetBoneGeometry(y)->GetBodyData().parent_ = 0;
					object->GetPhysics()->GetBoneGeometry(y)->SetJointId(tbc::INVALID_JOINT);
					object->GetPhysics()->GetBoneGeometry(y)->SetJointType(tbc::ChunkyBoneGeometry::kJointExclude);
					break;
				}
			}
			EngineList::iterator y = engine_list.begin();
			for (; y != engine_list.end(); ++y) {
				tbc::PhysicsEngine* _engine = *y;
				_engine->RemoveControlledGeometry(object->GetPhysics()->GetBoneGeometry(0));
			}
			removed = true;
			break;
		}
	}
	return (removed);
}

ContextObject::Array ContextObject::GetAttachedObjects() const {
	Array objects;
	ConnectionList::const_iterator x = connection_list_.begin();
	for (; x != connection_list_.end(); ++x) {
		objects.push_back(x->object_);
	}
	return objects;
}

void ContextObject::AddAttachedObjectEngine(ContextObject* attached_object, tbc::PhysicsEngine* engine) {
	ConnectionList::iterator x = connection_list_.begin();
	for (; x != connection_list_.end(); ++x) {
		if (x->object_ == attached_object) {
			x->engine_list_.push_back(engine);
		}
	}
}



void ContextObject::AddAttribute(ContextObjectAttribute* attribute) {
	attribute_array_.push_back(attribute);
	if (manager_) {
		manager_->AddAttributeSenderObject(this);
	}
}

void ContextObject::DeleteAttribute(const str& name) {
	AttributeArray::iterator x = attribute_array_.begin();
	while (x != attribute_array_.end()) {
		if ((*x)->GetName() == name) {
			delete (*x);
			x = attribute_array_.erase(x);
		} else {
			++x;
		}
	}
}

ContextObjectAttribute* ContextObject::GetAttribute(const str& name) const {
	AttributeArray::const_iterator x = attribute_array_.begin();
	for (; x != attribute_array_.end(); ++x) {
		if ((*x)->GetName() == name) {
			return *x;
		}
	}
	return 0;
}

const ContextObject::AttributeArray& ContextObject::GetAttributes() const {
	return attribute_array_;
}

float ContextObject::GetAttributeFloatValue(const str& attribute_name) const {
	const FloatAttribute* float_attribute = (const FloatAttribute*)GetAttribute(attribute_name);
	if (!float_attribute) {
		return 0;
	}
	return float_attribute->GetValue();
}

void ContextObject::QuerySetChildishness(float childishness) {
	const str _name = "float_childishness";
	cure::FloatAttribute* _attribute = (cure::FloatAttribute*)GetAttribute(_name);
	if (!_attribute) {
		_attribute = new cure::FloatAttribute(this, _name, 0);
	}
	_attribute->SetValue(childishness);
}

bool ContextObject::IsAttributeTrue(const str& attribute_name) const {
	return (GetAttributeFloatValue(attribute_name) > 0.5f);
}

void ContextObject::OnAttributeUpdated(ContextObjectAttribute*) {
	if (manager_) {
		manager_->AddAttributeSenderObject(this);
	}
}



void ContextObject::AddTrigger(tbc::PhysicsManager::BodyID trigger_id, const void* trigger) {
	trigger_map_.insert(TriggerMap::value_type(trigger_id, trigger));
}

void ContextObject::FinalizeTrigger(const tbc::PhysicsTrigger*) {
}

const void* ContextObject::GetTrigger(tbc::PhysicsManager::BodyID trigger_id) const {
	return HashUtil::FindMapObject(trigger_map_, trigger_id);
}

size_t ContextObject::GetTriggerCount(const void*& trigger) const {
	if (trigger_map_.empty()) {
		return (0);
	}
	trigger = trigger_map_.begin()->second;
	return (trigger_map_.size());
}



void ContextObject::SetSpawner(const tbc::PhysicsSpawner* spawner) {
	spawner_ = spawner;
}

const tbc::PhysicsSpawner* ContextObject::GetSpawner() const {
	return spawner_;
}



void ContextObject::AddChild(ContextObject* child) {
	deb_assert(child->GetInstanceId() != 0);
	if (std::find(child_array_.begin(), child_array_.end(), child) != child_array_.end()) {
		// Already added. This may for instance happen when another path for a level is added.
		return;
	}
	child_array_.push_back(child);
	child->SetParent(this);
}

const ContextObject::Array& ContextObject::GetChildArray() const {
	return child_array_;
}



bool ContextObject::UpdateFullPosition(const ObjectPositionalData*& positional_data) {
	if (!physics_) {
		return false;
	}
	tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
	if (!geometry || geometry->GetBodyId() == tbc::INVALID_BODY) {
		log_.Errorf("Could not get positional update (for streaming), since %i/%s not loaded yet!",
			GetInstanceId(), GetClassId().c_str());
		return false;
	}

	tbc::PhysicsManager* physics_manager = manager_->GetGameManager()->GetPhysicsManager();
	bool _ok = PositionHauler::Get(position_, physics_manager, physics_, total_mass_);
	if (_ok) {
		positional_data = &position_;
	}
	return _ok;
}

void ContextObject::SetFullPosition(const ObjectPositionalData& positional_data, float delta_threshold) {
	if (!IsLoaded()) {
		// Movement of this object was received (from remote host) before we were ready.
		position_.CopyData(&positional_data);
		return;
	}

	const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
	if (!geometry || geometry->GetBodyId() == tbc::INVALID_BODY) {
		return;
	}

	if (position_.IsSameStructure(positional_data)) {
		const bool position_only = (delta_threshold != 0);	// If there is a threshold at all, it's always about positions, never about engines.
		const float scaled_diff = position_.GetBiasedTypeDifference(&positional_data, position_only);
		if (scaled_diff <= delta_threshold) {
			return;
		}
		//if (delta_threshold > 0)
		//{
		//	log_.Infof("Positional diff is %f.", scaled_diff);
		//}
	}

	ForceSetFullPosition(positional_data);
}

void ContextObject::SetPositionFinalized() {
}

void ContextObject::SetInitialTransform(const xform& transformation) {
	//const quat& q = transformation.GetOrientation();
	//mLog.Infof("Setting initial quaternion (%f;%f;%f;%f for class %s."), q.a, q.b, q.c, q.d, GetClassId().c_str());
	position_.position_.transformation_ = transformation;
}

xform ContextObject::GetInitialTransform() const {
	return xform(GetOrientation(), GetPosition());
}

void ContextObject::SetInitialPositionalData(const ObjectPositionalData& positional_data) {
	//const quat& q = positional_data.position_.transformation_.GetOrientation();
	//mLog.Infof("Setting initial quaternion/pos (%f;%f;%f;%f for class %s."), q.a, q.b, q.c, q.d, GetClassId().c_str());
	position_.CopyData(&positional_data);
}

vec3 ContextObject::GetPosition() const {
	if (physics_ && physics_override_ != kPhysicsOverrideBones) {
		const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
		if (geometry) {
			tbc::PhysicsManager::BodyID _body_id = geometry->GetBodyId();
			return (manager_->GetGameManager()->GetPhysicsManager()->GetBodyPosition(_body_id));
		}
		//deb_assert(false);
	}
	return position_.position_.transformation_.GetPosition();
}

void ContextObject::SetRootPosition(const vec3& position) {
	//deb_assert(physics_override_ == kPhysicsOverrideBones);
	position_.position_.transformation_.SetPosition(position);

	if (physics_ && physics_->GetBoneCount() > 0) {
		physics_->GetBoneTransformation(0).SetPosition(position);
	}
}

vec3 ContextObject::GetRootPosition() const {
	return position_.position_.transformation_.GetPosition();
}

void ContextObject::SetRootOrientation(const quat& orientation) {
	//deb_assert(physics_override_ == kPhysicsOverrideBones);
	position_.position_.transformation_.SetOrientation(orientation);

	if (physics_ && physics_->GetBoneCount() > 0) {
		physics_->GetBoneTransformation(0).SetOrientation(orientation);
	}
}

void ContextObject::SetRootVelocity(const vec3& velocity) {
	position_.position_.velocity_ = velocity;
}

quat ContextObject::GetOrientation() const {
	if (physics_) {
		const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
		if (geometry && geometry->GetBodyId() != tbc::INVALID_BODY && geometry->GetBoneType() == tbc::ChunkyBoneGeometry::kBoneBody) {
			return manager_->GetGameManager()->GetPhysicsManager()->GetBodyOrientation(geometry->GetBodyId()) *
				physics_->GetOriginalBoneTransformation(0).GetOrientation();
		}
		//deb_assert(false);
	}
	return (position_.position_.transformation_.GetOrientation());
}

vec3 ContextObject::GetVelocity() const {
	if (physics_) {
		const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
		if (geometry && geometry->GetBodyId() != tbc::INVALID_BODY && geometry->GetBoneType() == tbc::ChunkyBoneGeometry::kBoneBody) {
			vec3 _velocity;
			manager_->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(geometry->GetBodyId(), _velocity);
			return _velocity;
		}
	}
	return position_.position_.velocity_;
}

vec3 ContextObject::GetAngularVelocity() const {
	if (physics_) {
		vec3 angular_velocity;
		const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
		if (geometry && geometry->GetBodyId() != tbc::INVALID_BODY && geometry->GetBoneType() == tbc::ChunkyBoneGeometry::kBoneBody) {
			manager_->GetGameManager()->GetPhysicsManager()->GetBodyAngularVelocity(geometry->GetBodyId(), angular_velocity);
		}
		return angular_velocity;
	} else {
		return position_.position_.angular_velocity_;
	}
}

vec3 ContextObject::GetAcceleration() const {
	vec3 acceleration;
	const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
	if (geometry && geometry->GetBodyId() != tbc::INVALID_BODY && geometry->GetBoneType() == tbc::ChunkyBoneGeometry::kBoneBody) {
		manager_->GetGameManager()->GetPhysicsManager()->GetBodyForce(geometry->GetBodyId(), acceleration);
	}
	return acceleration;
}

vec3 ContextObject::GetForwardDirection() const {
	const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
	if (geometry && geometry->GetBodyId() != tbc::INVALID_BODY && geometry->GetBoneType() == tbc::ChunkyBoneGeometry::kBoneBody) {
		const xform& original_transform =
			physics_->GetOriginalBoneTransformation(0);
		const vec3 forward_axis = original_transform.GetOrientation().GetConjugate() * vec3(0, 1, 0);	// Assumes original quaternion normalized.
		xform transform;
		manager_->GetGameManager()->GetPhysicsManager()->GetBodyTransform(geometry->GetBodyId(), transform);
		return (transform.GetOrientation() * forward_axis);
	}
	return vec3(0, 1, 0);
}

float ContextObject::GetForwardSpeed() const {
	float speed = 0;
	const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
	if (geometry && geometry->GetBodyId() != tbc::INVALID_BODY && geometry->GetBoneType() == tbc::ChunkyBoneGeometry::kBoneBody) {
		vec3 _velocity;
		manager_->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(geometry->GetBodyId(), _velocity);
		speed = _velocity * GetForwardDirection();
	}
	/*else {
		deb_assert(false);
	}*/
	return (speed);
}

float ContextObject::GetMass() const {
	return total_mass_;
}

float ContextObject::QueryMass() {
	tbc::PhysicsManager* physics_manager = manager_->GetGameManager()->GetPhysicsManager();
	total_mass_ = physics_->QueryTotalMass(physics_manager);
	return total_mass_;
}

void ContextObject::SetMass(float mass) {
	total_mass_ = mass;
}

ObjectPositionalData* ContextObject::GetNetworkOutputGhost() {
	if (!network_output_ghost_) {
		network_output_ghost_ = new ObjectPositionalData;
	}
	return network_output_ghost_;
}

void ContextObject::DeleteNetworkOutputGhost() {
	delete network_output_ghost_;
	network_output_ghost_ = 0;
}

void ContextObject::SetPhysics(tbc::ChunkyPhysics* structure) {
	physics_ = structure;
	if (physics_override_ == kPhysicsOverrideBones) {
		const int bone_count = physics_->GetBoneCount();
		for (int x = 0; x < bone_count; ++x) {
			const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(x);
			manager_->AddPhysicsBody(this, geometry->GetBodyId());
		}
	}
}

void ContextObject::ClearPhysics() {
	// Removes bodies from manager, then destroys all physical stuff.
	if (manager_ && physics_ && physics_override_ != kPhysicsOverrideBones) {
		DetachAll();

		const int bone_count = physics_->GetBoneCount();
		for (int x = 0; x < bone_count; ++x) {
			tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(x);
			if (geometry) {
				manager_->RemovePhysicsBody(geometry->GetBodyId());
			}
		}
		// Not a good idea for a shared resource:
		//physics_->ClearAll(manager_->GetGameManager()->GetPhysicsManager());
		physics_ = 0;
	}
}

tbc::ChunkyPhysics* ContextObject::GetPhysics() const {
	return (physics_);
}

void ContextObject::SetPhysicsTypeOverride(PhysicsOverride physics_override) {
	physics_override_ = physics_override;
}

tbc::ChunkyBoneGeometry* ContextObject::GetStructureGeometry(unsigned index) const {
	return (physics_->GetBoneGeometry(index));
}

tbc::ChunkyBoneGeometry* ContextObject::GetStructureGeometry(tbc::PhysicsManager::BodyID body_id) const {
	return (physics_->GetBoneGeometry(body_id));
}

bool ContextObject::SetEnginePower(unsigned aspect, float power) {
	return physics_->SetEnginePower(aspect, power);
}

float ContextObject::GetImpact(const vec3& gravity, const vec3& force, const vec3& torque, float extra_mass, float sideways_factor) const {
	const float mass_factor = 1 / (GetMass() + extra_mass);
	const float gravity_invert_factor = 1/gravity.GetLength();
	const vec3 gravity_direction(gravity * gravity_invert_factor);
	// High angle against direction of gravity means high impact.
	const float opposing_gravity_factor = -(force*gravity_direction) * gravity_invert_factor * mass_factor;
	const float _sideways_factor = force.Cross(gravity_direction).GetLength() * mass_factor;
	const float torque_factor = torque.GetLength() * mass_factor;
	float impact = 0;
	impact = std::max(impact, opposing_gravity_factor * 0.1f);
	impact = std::max(impact, opposing_gravity_factor * -0.8f);
	impact = std::max(impact, _sideways_factor * sideways_factor * 0.01f);
	impact = std::max(impact, torque_factor * 0.03f);
	if (impact >= 1.0f) {
		log_volatile(log_.Tracef("Collided hard with something dynamic."));
	}
	return (impact);
}



void ContextObject::ForceSend() {
	GetManager()->AddPhysicsSenderObject(this);	// Put us in send list.
	last_send_time_ -= 10;	// Make sure we send immediately.
}

bool ContextObject::QueryResendTime(float delta_time, bool unblock_delta) {
	bool ok_to_send = false;
	const float absolute_time = GetManager()->GetGameManager()->GetTimeManager()->GetAbsoluteTime();
	if (delta_time <= cure::TimeManager::GetAbsoluteTimeDiff(absolute_time, last_send_time_)) {
		ok_to_send = true;
		last_send_time_ = absolute_time - (unblock_delta? delta_time+MathTraits<float>::FullEps() : 0);
	}
	return (ok_to_send);
}

int ContextObject::PopSendCount() {
	if (send_count_ > 0) {
		--send_count_;
	}
	return (send_count_);
}

void ContextObject::SetSendCount(int count) {
	send_count_ = count;
}



void ContextObject::OnLoaded() {
	if (GetPhysics() && GetManager()) {
		// Calculate total mass.
		deb_assert(total_mass_ == 0);
		QueryMass();

		OnTick();

		/*if (physics_override_ != kPhysicsOverrideBones) {
			PositionHauler::Set(position_, physics_manager, physics_, total_mass_, allow_move_root_);
		}*/

		GetManager()->EnableTickCallback(this);
	}
}

void ContextObject::OnTick() {
}



void ContextObject::ForceSetFullPosition(const ObjectPositionalData& positional_data) {
	position_.CopyData(&positional_data);
	deb_assert(total_mass_ != 0 || GetPhysics()->GetBoneGeometry(0)->GetBoneType() == tbc::ChunkyBoneGeometry::kBoneTrigger);
	PositionHauler::Set(position_, manager_->GetGameManager()->GetPhysicsManager(), physics_, total_mass_, allow_move_root_);
}

void ContextObject::AttachToObject(tbc::ChunkyBoneGeometry* bone_geometry1, ContextObject* object2, tbc::ChunkyBoneGeometry* bone_geometry2, bool send) {
	deb_assert(object2);
	deb_assert(!IsAttachedTo(object2));
	if (!object2 || !bone_geometry1 || !bone_geometry2) {
		return;
	}
	if (!bone_geometry2->IsConnectorType(tbc::ChunkyBoneGeometry::kConnectee3Dof)) {
		return;
	}

	// Find first parent that is a dynamic body.
	tbc::PhysicsManager* physics_manager = manager_->GetGameManager()->GetPhysicsManager();
	const int physics_fps = manager_->GetGameManager()->GetTimeManager()->GetDesiredMicroSteps();
	tbc::PhysicsManager::BodyID body2_connectee = bone_geometry2->GetBodyId();
	tbc::ChunkyBoneGeometry* node2_connectee = bone_geometry2;
	while (physics_manager->IsStaticBody(body2_connectee)) {
		node2_connectee = node2_connectee->GetParent();
		if (!node2_connectee) {
			if (bone_geometry1->IsConnectorType(tbc::ChunkyBoneGeometry::kConnectorFixed)) {
				break;
			}
			log_.Error("Failing to attach joint to a static object. Try reversing the attachment!");
			return;
		}
		body2_connectee = node2_connectee->GetBodyId();
		bone_geometry2 = node2_connectee;
	}

	object2->SetAllowMoveRoot(false);

	if (bone_geometry1->IsConnectorType(tbc::ChunkyBoneGeometry::kConnectorBall)) {
		log_debug("Attaching two objects with ball joint.");
		bone_geometry2->SetJointType(tbc::ChunkyBoneGeometry::kJointBall);
	} else if (bone_geometry1->IsConnectorType(tbc::ChunkyBoneGeometry::kConnectorFixed)) {
		log_debug("Attaching two objects with fixed joint.");
		bone_geometry2->SetJointType(tbc::ChunkyBoneGeometry::kJointFixed);
	} else if (bone_geometry1->IsConnectorType(tbc::ChunkyBoneGeometry::kConnectorHinge2)) {
		log_debug("Attaching two objects with hinge-2 joint.");
		bone_geometry2->SetJointType(tbc::ChunkyBoneGeometry::kJointHinge2);
	} else if (bone_geometry1->IsConnectorType(tbc::ChunkyBoneGeometry::kConnectorSuspendHinge)) {
		log_debug("Attaching two objects with suspend hinge joint.");
		bone_geometry2->SetJointType(tbc::ChunkyBoneGeometry::kJointSuspendHinge);
	} else if (bone_geometry1->IsConnectorType(tbc::ChunkyBoneGeometry::kConnectorHinge)) {
		log_debug("Attaching two objects with hinge joint.");
		bone_geometry2->SetJointType(tbc::ChunkyBoneGeometry::kJointHinge);
	} else if (bone_geometry1->IsConnectorType(tbc::ChunkyBoneGeometry::kConnectorUniversal)) {
		log_debug("Attaching two objects with universal joint.");
		bone_geometry2->SetJointType(tbc::ChunkyBoneGeometry::kJointUniversal);
	} else if (bone_geometry1->IsConnectorType(tbc::ChunkyBoneGeometry::kConnectorSlider)) {
		log_debug("Attaching two objects with slider joint.");
		bone_geometry2->SetJointType(tbc::ChunkyBoneGeometry::kJointSlider);
	} else {
		log_.Error("Could not find connection type to attach two objects with a joint.");
		return;
	}
	bone_geometry2->GetBodyData().parent_ = bone_geometry1;
	bone_geometry2->CreateJoint(object2->GetPhysics(), physics_manager, physics_fps);

	AddAttachment(object2, bone_geometry2->GetJointId(), 0);
	object2->AddAttachment(this, tbc::INVALID_JOINT, 0);

	if (send) {
		//manager_->GetGameManager()->SendAttach(this, bone_geometry1->GetId(), object2, bone_geometry2->GetId());
	}
}

bool ContextObject::IsAttachedTo(ContextObject* object) const {
	ConnectionList::const_iterator x = connection_list_.begin();
	for (; x != connection_list_.end(); ++x) {
		if (object == x->object_) {
			return (true);
		}
	}
	return (false);
}

void ContextObject::AddAttachment(ContextObject* object, tbc::PhysicsManager::JointID joint, tbc::PhysicsEngine* engine) {
	deb_assert(!IsAttachedTo(object));
	connection_list_.push_back(Connection(object, joint, engine));
	if (engine) {
		physics_->AddEngine(engine);
	}
}



void ContextObject::RemoveChild(ContextObject* child) {
	child_array_.erase(std::remove(child_array_.begin(), child_array_.end(), child), child_array_.end());
}

void ContextObject::SetParent(ContextObject* parent) {
	parent_ = parent;
}

void ContextObject::SetupChildHandlers() {
	const int trigger_count = physics_->GetTriggerCount();
	for (int x = 0; x < trigger_count; ++x) {
		const tbc::PhysicsTrigger* _trigger = physics_->GetTrigger(x);
		ContextObject* handler_child = GetManager()->GetGameManager()->CreateLogicHandler(_trigger->GetFunction());
		if (!handler_child) {
			continue;
		}
		AddChild(handler_child);
		const int bone_trigger_count = _trigger->GetTriggerGeometryCount();
		for (int y = 0; y < bone_trigger_count; ++y) {
			AddTrigger(_trigger->GetPhysicsTriggerId(y), handler_child);
			handler_child->AddTrigger(_trigger->GetPhysicsTriggerId(y), _trigger);
		}
		if (bone_trigger_count == 0) {
			handler_child->AddTrigger(tbc::INVALID_BODY, _trigger);
		}
		handler_child->FinalizeTrigger(_trigger);
	}

	const int spawner_count = physics_->GetSpawnerCount();
	for (int x = 0; x < spawner_count; ++x) {
		const tbc::PhysicsSpawner* _spawner = physics_->GetSpawner(x);
		ContextObject* handler_child = GetManager()->GetGameManager()->CreateLogicHandler(_spawner->GetFunction());
		if (!handler_child) {
			continue;
		}
		AddChild(handler_child);
		handler_child->SetSpawner(_spawner);
	}
}



ResourceManager* ContextObject::GetResourceManager() const {
	return (resource_manager_);
}



loginstance(kGameContext, ContextObject);



}
