
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#pragma warning(push)
#pragma warning(disable: 4100)	// Warning: unreferenced formal parameter (in ODE).
#pragma warning(disable: 4127)	// Warning: conditional expression is constant (in ODE).
#include <ode/odemath.h>
#include <../ode/src/collision_kernel.h>
#include <../ode/src/collision_std.h>
#include <../ode/src/joints/ball.h>
#include <../ode/src/joints/hinge.h>
#include <../ode/src/joints/slider.h>
#include <../ode/src/joints/universal.h>
#pragma warning(pop)
#include "../../lepra/include/cyclicarray.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/math.h"
#include "../include/physicsmanagerode.h"



namespace tbc {



bool AreBodiesConnectedExcluding(dBodyID b1, dBodyID b2, int joint_type) {
	deb_assert(b1 || b2);
	dBodyID b = b1? b1 : b2;
	dBodyID s = b1? b2 : b1;
	for (dxJointNode* n=b->firstjoint; n; n=n->next) {
		if (dJointGetType(n->joint) != joint_type && n->body == s)
			return true;
	}
	return false;
}



PhysicsManagerODE::PhysicsManagerODE(float _radius, int levels, float sensitivity) {
	world_id_ = dWorldCreate();

	// Play with these to make the simulation behave better.
	SetSimulationParameters(0, 0.1f, 1);

	if (sensitivity) {
		::dWorldSetAutoDisableFlag(world_id_, 1);
		::dWorldSetAutoDisableLinearThreshold(world_id_, 0.02f/sensitivity);
		::dWorldSetAutoDisableAngularThreshold(world_id_, 0.02f/sensitivity);
		::dWorldSetAutoDisableSteps(world_id_, (int)(2*sensitivity));
		//::dWorldSetAutoDisableTime(world_id_, 0);
	} else {
		::dWorldSetAutoDisableFlag(world_id_, 0);
	}

	//::dWorldSetLinearDampingThreshold(world_id_, 100.0f);
	//::dWorldSetLinearDamping(world_id_, 0.9f);
	::dWorldSetAngularDampingThreshold(world_id_, 15.0f);
	::dWorldSetAngularDamping(world_id_, 0.5f);
	::dWorldSetMaxAngularSpeed(world_id_, 200.0f);

	// Collision space center and extents.
	dVector3 center = {0, 0, 0};
	dVector3 extents = {_radius, _radius, _radius};
	space_id_ = ::dQuadTreeSpaceCreate(0, center, extents, levels);

	::dWorldSetGravity(world_id_, 0, 0, -9.82f);

	// Create joint group for contact joints used in collision detection.
	contact_joint_group_id_ = ::dJointGroupCreate(0);
}

PhysicsManagerODE::~PhysicsManagerODE() {
	while (!object_table_.empty()) {
		Object* _object = *object_table_.begin();
		DeleteBody((BodyID)_object);
	}

	::dWorldDestroy(world_id_);
	::dSpaceDestroy(space_id_);
	::dJointGroupDestroy(contact_joint_group_id_);
}

void PhysicsManagerODE::SetSimulationParameters(float softness, float rubberbanding, float accuracy) {
	world_cfm_ = Math::Lerp(1e-9f, 1e-2f, softness);	// World softness and numerical stability, i.e peneraation.
	::dWorldSetCFM(world_id_, world_cfm_);
	world_erp_ = Math::Lerp(1.0f, 0.2f, rubberbanding);
	::dWorldSetERP(world_id_, world_erp_);	// Error reduction.
	::dWorldSetQuickStepNumIterations(world_id_, (int)Math::Lerp(1, 20, accuracy));
}

bool PhysicsManagerODE::InitCurrentThread() {
	return ::dAllocateODEDataForThread((unsigned)dAllocateMaskAll) != 0;
}

int PhysicsManagerODE::QueryRayCollisionAgainst(const vec3& ray_position, const vec3& ray_direction,
	float length, BodyID _body, vec3* collision_points, int max_collision_count) {
	if (max_collision_count <= 0) {
		deb_assert(false);
		return 0;
	}

	ObjectTable::iterator x = object_table_.find((Object*)_body);
	if (x == object_table_.end()) {
		deb_assert(false);
		return 0;
	}
	const Object* _object = *x;

	dGeomID ray_geometry_id = ::dCreateRay(0, length);
	::dGeomRaySet(ray_geometry_id, ray_position.x, ray_position.y, ray_position.z,
		ray_direction.x, ray_direction.y, ray_direction.z);

	dContactGeom contact[8];
	const int max_count = std::min((int)LEPRA_ARRAY_COUNT(contact), max_collision_count*2);
	const int collision_count = ::dCollide(ray_geometry_id, _object->geom_id_, max_count, &contact[0], sizeof(contact[0]));

	::dGeomDestroy(ray_geometry_id);

	int found_collision_points = 0;
	for (int x = 0; x < collision_count; ++x) {
		// Check that we've found a surface turned towards the given direction.
		const vec3 __normal(contact[x].normal[0], contact[x].normal[1], contact[x].normal[2]);
		if (__normal*ray_direction < 0) {
			collision_points[found_collision_points++].Set(contact[x].pos[0], contact[x].pos[1], contact[x].pos[2]);
		}
	}

	return found_collision_points;
}

int PhysicsManagerODE::QueryRayPick(const vec3& ray_position, const vec3& ray_direction, float length, int* force_feedback_ids, vec3* positions, int max_bodies) {
	dGeomID ray_geometry_id = ::dCreateRay(0, length);
	::dGeomRaySet(ray_geometry_id, ray_position.x, ray_position.y, ray_position.z,
		ray_direction.x, ray_direction.y, ray_direction.z);

	int hits = 0;
	void* _data[4] = {force_feedback_ids, positions, (void*)&hits, (void*)&max_bodies};
	space_id_->collide2(&_data, ray_geometry_id, &PhysicsManagerODE::RayPickCallback);

	::dGeomDestroy(ray_geometry_id);

	return hits;
}

PhysicsManager::BodyID PhysicsManagerODE::CreateSphere(bool is_root, const xform& transform,
	float32 _mass, float32 _radius, BodyType _type, float32 friction, float32 _bounce,
	int force_listener_id, bool is_trigger) {
	Object* _object = new Object(world_id_, is_root);
	_object->geom_id_ = dCreateSphere(space_id_, (dReal)_radius);
	if (is_trigger) {
		_object->trigger_listener_id_ = force_listener_id;
	} else {
		_object->force_feedback_id_ = force_listener_id;
	}

	if (_type == PhysicsManager::kDynamic) {
		dMass __mass;
		::dMassSetSphereTotal(&__mass, (dReal)_mass, (dReal)_radius);
		_object->body_id_ = dBodyCreate(world_id_);
		::dBodySetMass(_object->body_id_, &__mass);
		::dGeomSetBody(_object->geom_id_, _object->body_id_);
		::dBodySetAutoDisableDefaults(_object->body_id_);
		::dBodySetAngularDampingThreshold(_object->body_id_, 200.0f);
		::dBodySetAngularDamping(_object->body_id_, 0.2f);
	}

	::dGeomSetData(_object->geom_id_, _object);

	_object->geometry_data_[0] = _radius;
	_object->mass_ = _mass;
	_object->friction_ = friction;
	_object->bounce_   = _bounce;

	object_table_.insert(_object);

	SetBodyTransform((BodyID)_object, transform);

	return ((BodyID)_object);
}

PhysicsManager::BodyID PhysicsManagerODE::CreateCylinder(bool is_root, const xform& transform,
	float32 _mass, float32 _radius, float32 length, BodyType _type, float32 friction,
	float32 _bounce, int force_listener_id, bool is_trigger) {
	Object* _object = new Object(world_id_, is_root);

	// TODO: Create a real cylinder when ODE supports it.
	_object->geom_id_ = ::dCreateCylinder(space_id_, (dReal)_radius, (dReal)length);

	if (is_trigger) {
		_object->trigger_listener_id_ = force_listener_id;
	} else {
		_object->force_feedback_id_ = force_listener_id;
	}

	if (_type == PhysicsManager::kDynamic) {
		dMass __mass;
		::dMassSetCylinderTotal(&__mass, (dReal)_mass, 3, (dReal)_radius, (dReal)length);
		_object->body_id_ = dBodyCreate(world_id_);
		::dBodySetMass(_object->body_id_, &__mass);
		::dGeomSetBody(_object->geom_id_, _object->body_id_);
		::dBodySetAutoDisableDefaults(_object->body_id_);
	}

	::dGeomSetData(_object->geom_id_, _object);

	_object->geometry_data_[0] = _radius;
	_object->geometry_data_[1] = length;
	_object->mass_ = _mass;
	_object->friction_ = friction;
	_object->bounce_   = _bounce;

	object_table_.insert(_object);

	SetBodyTransform((BodyID)_object, transform);

	return (BodyID)_object;
}

PhysicsManager::BodyID PhysicsManagerODE::CreateCapsule(bool is_root, const xform& transform,
	float32 _mass, float32 _radius, float32 length, BodyType _type, float32 friction,
	float32 _bounce, int force_listener_id, bool is_trigger) {
	Object* _object = new Object(world_id_, is_root);

	_object->geom_id_ = ::dCreateCapsule(space_id_, (dReal)_radius, (dReal)length);
	if (is_trigger) {
		_object->trigger_listener_id_ = force_listener_id;
	} else {
		_object->force_feedback_id_ = force_listener_id;
	}

	if (_type == PhysicsManager::kDynamic) {
		dMass __mass;
		::dMassSetCapsuleTotal(&__mass, (dReal)_mass, 3, (dReal)_radius, (dReal)length);
		_object->body_id_ = dBodyCreate(world_id_);
		::dBodySetMass(_object->body_id_, &__mass);
		::dGeomSetBody(_object->geom_id_, _object->body_id_);
		::dBodySetAutoDisableDefaults(_object->body_id_);
	}

	_object->geometry_data_[0] = _radius;
	_object->geometry_data_[1] = length;
	dGeomSetData(_object->geom_id_, _object);

	_object->mass_ = _mass;
	_object->friction_ = friction;
	_object->bounce_ = _bounce;

	object_table_.insert(_object);

	SetBodyTransform((BodyID)_object, transform);

	return (BodyID)_object;
}

PhysicsManager::BodyID PhysicsManagerODE::CreateBox(bool is_root, const xform& transform,
	float32 _mass, const vec3& size, BodyType _type, float32 friction,
	float32 _bounce, int force_listener_id, bool is_trigger) {
	Object* _object = new Object(world_id_, is_root);

	_object->geom_id_ = ::dCreateBox(space_id_, (dReal)size.x, (dReal)size.y, (dReal)size.z);
	if (is_trigger) {
		_object->trigger_listener_id_ = force_listener_id;
	} else {
		_object->force_feedback_id_ = force_listener_id;
	}

	if (_type == PhysicsManager::kDynamic) {
		_object->body_id_ = ::dBodyCreate(world_id_);
		dMass __mass;
		::dMassSetBoxTotal(&__mass, (dReal)_mass, (dReal)size.x, (dReal)size.y, (dReal)size.z);
		::dBodySetMass(_object->body_id_, &__mass);
		::dGeomSetBody(_object->geom_id_, _object->body_id_);
		::dBodySetAutoDisableDefaults(_object->body_id_);
	}

	::dGeomSetData(_object->geom_id_, _object);

	_object->geometry_data_[0] = size.x;
	_object->geometry_data_[1] = size.y;
	_object->geometry_data_[2] = size.z;
	_object->mass_ = _mass;
	_object->friction_ = -friction;
	_object->bounce_ = _bounce;

	object_table_.insert(_object);

	SetBodyTransform((BodyID)_object, transform);

	return (BodyID)_object;
}

bool PhysicsManagerODE::Attach(BodyID static_body, BodyID main_body) {
	ObjectTable::iterator x = object_table_.find((Object*)static_body);
	if (x == object_table_.end()) {
		deb_assert(false);
		return (false);
	}
	ObjectTable::iterator y = object_table_.find((Object*)main_body);
	if (y == object_table_.end() || x == y) {
		deb_assert(false);
		return (false);
	}
	Object* static_object = *x;
	dVector3 __pos;
	::dGeomCopyPosition(static_object->geom_id_, __pos);
	dQuaternion o;
	::dGeomGetQuaternion(static_object->geom_id_, o);
	Object* main_object = *y;
	dBodyID _body_id = main_object->body_id_;
	if (!_body_id) {
		return true;
	}
	if (static_object->body_id_) {
		JointInfo* _joint_info = joint_info_allocator_.Alloc();
		_joint_info->joint_id_ = dJointCreateFixed(world_id_, 0);
		_joint_info->type_ = kJointFixed;
		_joint_info->body1_id_ = static_body;
		_joint_info->body2_id_ = main_body;
		_joint_info->listener_id1_ = static_object->force_feedback_id_;
		_joint_info->listener_id2_ = main_object->force_feedback_id_;
		main_object->has_mass_children_ = true;
		dJointAttach(_joint_info->joint_id_, static_object->body_id_, main_object->body_id_);
		dJointSetFixed(_joint_info->joint_id_);
	} else {
		::dGeomSetBody(static_object->geom_id_, _body_id);
		::dGeomSetOffsetWorldPosition(static_object->geom_id_, __pos[0], __pos[1], __pos[2]);
		::dGeomSetOffsetWorldQuaternion(static_object->geom_id_, o);
		AddMass(static_body, main_body);
	}
	return (true);
}

bool PhysicsManagerODE::DetachToDynamic(BodyID static_body, float32 _mass) {
	Object* _object = (Object*)static_body;

	dGeomID g = _object->geom_id_;
	dMass __mass;
	switch (g->type) {
		case dTriMeshClass:	// TRICKY: fall through (act as sphere).
		case dSphereClass:	::dMassSetSphereTotal(&__mass, (dReal)_mass, ((dxSphere*)g)->radius);						break;
		case dBoxClass:		::dMassSetBoxTotal(&__mass, (dReal)_mass, ((dxBox*)g)->side[0], ((dxBox*)g)->side[1], ((dxBox*)g)->side[2]);	break;
		case dCapsuleClass:	::dMassSetCapsuleTotal(&__mass, (dReal)_mass, 3, ((dxCapsule*)g)->radius, ((dxCapsule*)g)->lz);			break;
		case dCylinderClass:	::dMassSetCylinderTotal(&__mass, (dReal)_mass, 3, ((dxCylinder*)g)->radius, ((dxCylinder*)g)->lz);		break;
		default: {
			log_.Error("Trying to detach object of unknown type!");
			deb_assert(false);
		}
		return (false);
	}

	if (_object->body_id_) {
		// Already dynamic, just update mass.
	} else {
		// Create dynamic body for it.
		_object->body_id_ = ::dBodyCreate(world_id_);
		const dReal* __pos = ::dGeomGetPosition(_object->geom_id_);
		dQuaternion __quat;
		::dGeomGetQuaternion(_object->geom_id_, __quat);
		::dGeomSetBody(_object->geom_id_, _object->body_id_);
		::dGeomSetPosition(_object->geom_id_, __pos[0], __pos[1], __pos[2]);
		::dGeomSetQuaternion(_object->geom_id_, __quat);
		::dBodySetAutoDisableDefaults(_object->body_id_);
	}
	::dBodySetMass(_object->body_id_, &__mass);
	return true;
}

bool PhysicsManagerODE::MakeStatic(BodyID dynamic_body) {
	ObjectTable::iterator x = object_table_.find((Object*)dynamic_body);
	if (x == object_table_.end()) {
		deb_assert(false);
		return (false);
	}
	Object* dynamic_object = *x;
	if (dynamic_object->body_id_) {
		::dGeomSetBody(dynamic_object->geom_id_, 0);
		::dBodyDestroy(dynamic_object->body_id_);
		dynamic_object->body_id_ = 0;
	}
	return true;
}

bool PhysicsManagerODE::AddMass(BodyID static_body, BodyID main_body) {
	ObjectTable::iterator x = object_table_.find((Object*)static_body);
	if (x == object_table_.end()) {
		deb_assert(false);
		return (false);
	}
	ObjectTable::iterator y = object_table_.find((Object*)main_body);
	if (y == object_table_.end() || x == y) {
		deb_assert(false);
		return (false);
	}
	Object* static_object = *x;
	Object* main_object = *y;
	if (static_object->body_id_) {
		log_.Error("Attach() with non-static.");
		deb_assert(false);
		return (false);
	}
	if (!static_object->mass_) {	// No point in trying to add flyweight trigger.
		return false;
	}
	dBodyID _body_id = main_object->body_id_;
	if (!_body_id) {
		deb_assert(false);
		return (false);
	}

	main_object->has_mass_children_ = true;

	dMass __mass;
	const dReal mass_scalar = (dReal)static_object->mass_;
	deb_assert(mass_scalar > 0);
	float* _size = static_object->geometry_data_;
	// Adding mass to the dynamic object.
	switch (static_object->geom_id_->type) {
		case dTriMeshClass:	// TRICKY: fall through (act as sphere).
		case dSphereClass:	::dMassSetSphereTotal(&__mass, mass_scalar, (dReal)_size[0]);					break;
		case dBoxClass:		::dMassSetBoxTotal(&__mass, mass_scalar, (dReal)_size[0], (dReal)_size[1], (dReal)_size[2]);	break;
		case dCapsuleClass:	::dMassSetCapsuleTotal(&__mass, mass_scalar, 3, (dReal)_size[0], (dReal)_size[1]);		break;
		case dCylinderClass:	::dMassSetCylinderTotal(&__mass, mass_scalar, 3, (dReal)_size[0], (dReal)_size[1]);		break;
		default: {
			log_.Error("Trying to attach object of unknown type!");
			deb_assert(false);
			return (false);
		}
	}
	const dReal* rel_rot = ::dGeomGetOffsetRotation(static_object->geom_id_);
	::dMassRotate(&__mass, rel_rot);
	const dReal* rel_pos = ::dGeomGetOffsetPosition(static_object->geom_id_);
	::dMassTranslate(&__mass, rel_pos[0], rel_pos[1], rel_pos[2]);

	dMass dynamic_mass;
	::dBodyGetMass(_body_id, &dynamic_mass);
	::dMassAdd(&dynamic_mass, &__mass);
	::dBodySetMass(_body_id, &dynamic_mass);

	return true;
}

PhysicsManager::BodyID PhysicsManagerODE::CreateTriMesh(bool is_root, unsigned vertex_count, const float* vertices, unsigned triangle_count, const lepra::uint32* indices,
	const xform& transform, float32 _mass, BodyType _type, float32 friction, float32 _bounce, int force_listener_id, bool is_trigger) {
	Object* _object = new Object(world_id_, is_root);

	_object->tri_mesh_id_ = ::dGeomTriMeshDataCreate();
	::dGeomTriMeshDataBuildSingle(_object->tri_mesh_id_,
				    vertices,
				    sizeof(vertices[0]) * 3,
				    vertex_count,
				    indices,
				    triangle_count * 3,
				    sizeof(indices[0]) * 3);

	_object->geom_id_ = ::dCreateTriMesh(space_id_, _object->tri_mesh_id_, 0, 0, 0);
	//::dGeomSetBody(_object->geom_id_, _object->body_id_);
	::dGeomSetData(_object->geom_id_, _object);
	if (is_trigger) {
		_object->trigger_listener_id_ = force_listener_id;
	} else {
		_object->force_feedback_id_ = force_listener_id;
	}

//	dGeomTriMeshEnableTC(_object->geom_id_, dBoxClass, 1);

	float average_radius = 0;
	for (unsigned x = 0; x < vertex_count; ++x) {
		average_radius += vec3(vertices[x*3+0], vertices[x*3+0], vertices[x*3+0]).GetLength();
	}
	average_radius /= vertex_count;
	if (_type == PhysicsManager::kDynamic) {
		dMass __mass;
		::dMassSetSphereTotal(&__mass, (dReal)_mass, (dReal)average_radius);
		_object->body_id_ = dBodyCreate(world_id_);
		::dBodySetMass(_object->body_id_, &__mass);
		::dGeomSetBody(_object->geom_id_, _object->body_id_);
		::dBodySetAutoDisableDefaults(_object->body_id_);
		::dBodySetAngularDampingThreshold(_object->body_id_, 200.0f);
		::dBodySetAngularDamping(_object->body_id_, 0.2f);
	}

	_object->geometry_data_[0] = average_radius;
	_object->mass_ = _mass;
	_object->friction_ = -friction;
	_object->bounce_ = _bounce;

	object_table_.insert(_object);

	SetBodyTransform((BodyID)_object, transform);

	return (BodyID)_object;
}

bool PhysicsManagerODE::IsStaticBody(BodyID body_id) const {
	Object* _object = (Object*)body_id;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("IsStaticBody() - Body %i is not part of this world!", body_id);
		return (true);
	}
	return (_object->body_id_ == 0);
}

void PhysicsManagerODE::DeleteBody(BodyID body_id) {
	ObjectTable::iterator x = object_table_.find((Object*)body_id);
	if (x != object_table_.end()) {
		Object* _object = *x;
		if (_object->body_id_ != 0) {
			::dBodyDestroy(_object->body_id_);
		}
		if (_object->tri_mesh_id_ != 0) {
			::dGeomTriMeshDataDestroy(_object->tri_mesh_id_);
		}
		::dGeomDestroy(_object->geom_id_);
		delete (_object);
		object_table_.erase(x);
	} else {
		log_.Error("DeleteBody() - Can't find body to delete!");
		deb_assert(false);
	}
}

vec3 PhysicsManagerODE::GetBodyPosition(BodyID body_id) const {
	Object* _object = (Object*)body_id;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("GetBodyPosition() - Body %i is not part of this world!", body_id);
		return (vec3());
	}

	const dReal* _position = dGeomGetPosition(_object->geom_id_);
	return (vec3(_position[0], _position[1], _position[2]));
}

void PhysicsManagerODE::SetBodyPosition(BodyID body_id, const vec3& position) const {
	Object* _object = (Object*)body_id;

	if (_object->world_id_ != world_id_) {
		log_.Errorf("SetBodyPosition() - Body %i is not part of this world!", body_id);
		return;
	}

	if (_object->body_id_) {
		::dBodySetPosition(_object->body_id_, position.x, position.y, position.z);
		::dBodyEnable(_object->body_id_);
	} else {
		::dGeomSetPosition(_object->geom_id_, position.x, position.y, position.z);
	}
}

quat PhysicsManagerODE::GetBodyOrientation(BodyID body_id) const {
	Object* _object = (Object*)body_id;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("GetBodyOrientation() - Body %i is not part of this world!", body_id);
		return kIdentityQuaternionF;
	}

	quat __quat;
	::dGeomGetQuaternion(_object->geom_id_, __quat.data_);
	return __quat;
}

void PhysicsManagerODE::SetBodyOrientation(BodyID body_id, const quat& orientation) {
	Object* _object = (Object*)body_id;

	if (_object->world_id_ != world_id_) {
		log_.Errorf("SetBodyOrientation() - Body %i is not part of this world!", body_id);
		return;
	}

	if(_object->body_id_) {
		::dBodySetQuaternion(_object->body_id_, orientation.data_);
		::dBodyEnable(_object->body_id_);
	} else {
		::dGeomSetQuaternion(_object->geom_id_, orientation.data_);
	}
}

void PhysicsManagerODE::GetBodyTransform(BodyID body_id, xform& transform) const {
	Object* _object = (Object*)body_id;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("GetBodyTransform() - Body %i is not part of this world!", body_id);
		return;
	}

	::dGeomCopyPosition(_object->geom_id_, transform.position_.data_);
	::dGeomGetQuaternion(_object->geom_id_, transform.orientation_.data_);
}

void PhysicsManagerODE::SetBodyTransform(BodyID body_id, const xform& transform) {
	Object* _object = (Object*)body_id;

	if (_object->world_id_ != world_id_) {
		log_.Errorf("SetBodyTransform() - Body %i is not part of this world!", body_id);
		return;
	}

	const vec3 __pos(transform.GetPosition());
	quat __quat = transform.GetOrientation();
	dReal __q[4];
	__q[0] = __quat.a;
	__q[1] = __quat.b;
	__q[2] = __quat.c;
	__q[3] = __quat.d;
	if(_object->body_id_) {
		::dBodySetPosition(_object->body_id_, __pos.x, __pos.y, __pos.z);
		::dBodySetQuaternion(_object->body_id_, __q);
		::dBodyEnable(_object->body_id_);
	} else {
		::dGeomSetPosition(_object->geom_id_, __pos.x, __pos.y, __pos.z);
		::dGeomSetQuaternion(_object->geom_id_, __q);
	}
}

void PhysicsManagerODE::GetBodyVelocity(BodyID body_id, vec3& velocity) const {
	Object* _object = (Object*)body_id;
	dBodyID _body_id = _object->body_id_;
	if (!_body_id) {
		_body_id = _object->geom_id_->body;
	}
	if (_body_id) {
		const dReal* _velocity = dBodyGetLinearVel(_body_id);
		velocity.x = _velocity[0];
		velocity.y = _velocity[1];
		velocity.z = _velocity[2];
	} else {
		velocity.Set(0, 0, 0);
	}
}

void PhysicsManagerODE::SetBodyVelocity(BodyID body_id, const vec3& velocity) {
	Object* _object = (Object*)body_id;
	if(_object->body_id_) {
		::dBodySetLinearVel(_object->body_id_, velocity.x, velocity.y, velocity.z);
		::dBodyEnable(_object->body_id_);
	}
}

void PhysicsManagerODE::GetBodyForce(BodyID body_id, vec3& acceleration) const {
	Object* _object = (Object*)body_id;
	dBodyID _body_id = _object->body_id_;
	if (!_body_id) {
		_body_id = _object->geom_id_->body;
	}
	if(_body_id) {
		const dReal* _acceleration = dBodyGetForce(_body_id);
		acceleration.x = _acceleration[0];
		acceleration.y = _acceleration[1];
		acceleration.z = _acceleration[2];
	} else {
		acceleration.Set(0, 0, 0);
	}
}

void PhysicsManagerODE::SetBodyForce(BodyID body_id, const vec3& acceleration) {
	Object* _object = (Object*)body_id;
	if(_object->body_id_)
		dBodySetForce(_object->body_id_, acceleration.x, acceleration.y, acceleration.z);
}

void PhysicsManagerODE::GetBodyAcceleration(BodyID body_id, float total_mass, vec3& acceleration) const {
	deb_assert(total_mass > 0);
	GetBodyForce(body_id, acceleration);
	acceleration /= total_mass;
}

void PhysicsManagerODE::SetBodyAcceleration(BodyID body_id, float total_mass, const vec3& acceleration) {
	deb_assert(total_mass > 0);
	SetBodyForce(body_id, acceleration * total_mass);
}

void PhysicsManagerODE::GetBodyAngularVelocity(BodyID body_id, vec3& angular_velocity) const {
	Object* _object = (Object*)body_id;
	dBodyID _body_id = _object->body_id_;
	if (!_body_id) {
		_body_id = _object->geom_id_->body;
	}
	if(_body_id) {
		const dReal* _angular_velocity = ::dBodyGetAngularVel(_body_id);
		angular_velocity.x = _angular_velocity[0];
		angular_velocity.y = _angular_velocity[1];
		angular_velocity.z = _angular_velocity[2];
	} else {
		angular_velocity.Set(0, 0, 0);
	}
}

void PhysicsManagerODE::SetBodyAngularVelocity(BodyID body_id, const vec3& angular_velocity) {
	Object* _object = (Object*)body_id;
	if(_object->body_id_) {
		::dBodySetAngularVel(_object->body_id_, angular_velocity.x, angular_velocity.y, angular_velocity.z);
	}
}

void PhysicsManagerODE::GetBodyTorque(BodyID body_id, vec3& angular_acceleration) const {
	Object* _object = (Object*)body_id;
	dBodyID _body_id = _object->body_id_;
	if (!_body_id) {
		_body_id = _object->geom_id_->body;
	}
	if(_body_id) {
		const dReal* _angular_acceleration = dBodyGetTorque(_body_id);
		angular_acceleration.x = _angular_acceleration[0];
		angular_acceleration.y = _angular_acceleration[1];
		angular_acceleration.z = _angular_acceleration[2];
	} else {
		angular_acceleration.Set(0, 0, 0);
	}
}

void PhysicsManagerODE::SetBodyTorque(BodyID body_id, const vec3& angular_acceleration) {
	Object* _object = (Object*)body_id;
	if(_object->body_id_) {
		::dBodySetTorque(_object->body_id_, angular_acceleration.x, angular_acceleration.y, angular_acceleration.z);
	}
}

void PhysicsManagerODE::GetBodyAngularAcceleration(BodyID body_id, float total_mass, vec3& angular_acceleration) const {
	GetBodyTorque(body_id, angular_acceleration);
	// TODO: handle moment of inertia?
	angular_acceleration /= total_mass;
}

void PhysicsManagerODE::SetBodyAngularAcceleration(BodyID body_id, float total_mass, const vec3& angular_acceleration) {
	// TODO: handle moment of inertia?
	SetBodyTorque(body_id, angular_acceleration * total_mass);
}

float PhysicsManagerODE::GetBodyMass(BodyID body_id) {
	Object* _object = (Object*)body_id;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("GetBodyMass() - Body %i is not part of this world!", body_id);
		return (0);
	}
	return (_object->mass_);
}

void PhysicsManagerODE::SetBodyMass(BodyID body_id, float _mass) {
	Object* _object = (Object*)body_id;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("SetBodyMass() - Body %i is not part of this world!", body_id);
		return;
	}
	if (!_object->body_id_ || _mass <= 0) {
		log_.Errorf("SetBodyMass() - body %i is static or mass %f is not greater than zero!", body_id, _mass);
		return;
	}
	_object->mass_ = _mass;
	dMass __mass;
	__mass.setSphereTotal(_object->mass_, _object->geometry_data_[0]);
	::dBodySetMass(_object->body_id_, &__mass);
}

void PhysicsManagerODE::MassAdjustBody(BodyID body_id) {
	Object* _object = (Object*)body_id;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("MassAdjustBody() - Body %i is not part of this world!", body_id);
		return;
	}

	dBodyID __body = _object->body_id_;
	if (!__body) {
		//mLog.Warningf("MassAdjustBody( - static geometry %i does not have a body!"), pBodyId);
		return;
	}
	// Move geometries and body to center of mass.
	dMass m;
	::dBodyGetMass(__body, &m);
	if (m.c[0] != 0 || m.c[1] != 0 || m.c[2] != 0) {
		for (dGeomID geometry = __body->geom; geometry; geometry = geometry->body_next) {
			const dReal* g = ::dGeomGetOffsetPosition(geometry);
			::dGeomSetOffsetPosition(geometry, g[0]-m.c[0], g[1]-m.c[1], g[2]-m.c[2]);
		}
		const dReal* p = ::dBodyGetPosition(__body);
		::dBodySetPosition(__body, p[0]-m.c[0], p[1]-m.c[1], p[2]-m.c[2]);
		::dMassTranslate(&m, -m.c[0], -m.c[1], -m.c[2]);
		::dBodySetMass(__body, &m);
	}
}

void PhysicsManagerODE::SetBodyData(BodyID body_id, void* user_data) {
	Object* _object = (Object*)body_id;

	if (_object->world_id_ != world_id_) {
		log_.Errorf("SetBodyData() - Body %i is not part of this world!", body_id);
		return;
	}

	_object->user_data_ = user_data;
}

void* PhysicsManagerODE::GetBodyData(BodyID body_id) {
	Object* _object = (Object*)body_id;

	if (_object->world_id_ != world_id_) {
		log_.Errorf("GetBodyData() - Body %i is not part of this world!", body_id);
		return 0;
	}

	return _object->user_data_;
}


int PhysicsManagerODE::GetTriggerListenerId(BodyID trigger) {
	Object* _object = (Object*)trigger;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("GetForceFeedbackListenerId() - trigger %i is not part of this world!", trigger);
		return (0);
	}
	return (_object->trigger_listener_id_);
}

int PhysicsManagerODE::GetForceFeedbackListenerId(BodyID _body) {
	Object* _object = (Object*)_body;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("GetForceFeedbackListenerId() - Body %i is not part of this world!", _body);
		return (0);
	}
	return (_object->force_feedback_id_);
}

void PhysicsManagerODE::SetForceFeedbackListener(BodyID _body, int force_feedback_id) {
	Object* _object = (Object*)_body;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("SetForceFeedbackListener - Body %i is not part of this world!", _body);
		return;
	}
	_object->force_feedback_id_ = force_feedback_id;
}

PhysicsManager::JointID PhysicsManagerODE::CreateBallJoint(BodyID body1, BodyID body2, const vec3& anchor_pos) {
	Object* _object1;
	Object* _object2;
	if (CheckBodies(body1, body2, _object1, _object2, "CreateBallJoint") == false)
		return INVALID_JOINT;

	JointInfo* _joint_info = joint_info_allocator_.Alloc();
	_joint_info->joint_id_ = dJointCreateBall(world_id_, 0);
	_joint_info->type_ = kJointBall;
	_joint_info->body1_id_ = body1;
	_joint_info->body2_id_ = body2;
	_joint_info->listener_id1_ = _object1->force_feedback_id_;
	_joint_info->listener_id2_ = _object2->force_feedback_id_;

	_object1->has_mass_children_ = true;
	if (_object2 != 0) {
		_object2->has_mass_children_ = true;
		dJointAttach(_joint_info->joint_id_, _object1->body_id_, _object2->body_id_);
		//::dBodySetLinearDampingThreshold(_object2->body_id_, 30.0f);
		//::dBodySetLinearDamping(_object2->body_id_, 0.95f);
		::dBodySetAngularDampingThreshold(_object2->body_id_, 10.0f);
		::dBodySetAngularDamping(_object2->body_id_, 0.97f);
		::dBodySetMaxAngularSpeed(_object2->body_id_, 20.0f);
	} else {
		dJointAttach(_joint_info->joint_id_, _object1->body_id_, 0);
		::dBodySetAngularDampingThreshold(_object2->body_id_, 15.0f);
		::dBodySetAngularDamping(_object2->body_id_, 0.97f);
		::dBodySetMaxAngularSpeed(_object2->body_id_, 30.0f);
	}

	/*if ((_object1 != 0 && _object1->force_feedback_id_ != 0) ||
	   (_object2 != 0 && _object2->force_feedback_id_ != 0)) {
		dJointSetFeedback(_joint_info->joint_id_, &_joint_info->feedback_);
	}*/

	dJointSetBallAnchor(_joint_info->joint_id_, anchor_pos.x, anchor_pos.y, anchor_pos.z);

	joint_table_.insert(_joint_info);
	return (JointID)_joint_info;
}

PhysicsManager::JointID PhysicsManagerODE::CreateHingeJoint(BodyID body1, BodyID body2,
	const vec3& anchor_pos, const vec3& axis) {
	Object* _object1;
	Object* _object2;
	if (CheckBodies(body1, body2, _object1, _object2, "CreateHingeJoint") == false)
		return INVALID_JOINT;

	JointInfo* _joint_info = joint_info_allocator_.Alloc();
	_joint_info->joint_id_ = dJointCreateHinge(world_id_, 0);
	_joint_info->type_ = kJointHinge;
	_joint_info->body1_id_ = body1;
	_joint_info->body2_id_ = body2;
	_joint_info->listener_id1_ = _object1->force_feedback_id_;
	_joint_info->listener_id2_ = _object2->force_feedback_id_;

	_object1->has_mass_children_ = true;
	if (_object2 != 0) {
		_object2->has_mass_children_ = true;
		dJointAttach(_joint_info->joint_id_, _object1->geom_id_->body, _object2->geom_id_->body);

		if (_object2->geom_id_->type == dBoxClass) {
			// Someone is attaching a revolving box to another dynamic object. That means that we
			// potentially could crash into something hard, causing overflow in the physics stepper.
			// We eliminate by making the 2nd body use a pretty limited maximum angular speed. However,
			// we raise the angular damping threshold to avoid limiting engine power for these high-
			// rotation/low torque engines (usually rotor or similar).
			::dBodySetMaxAngularSpeed(_object2->body_id_, 50.0f);
			::dBodySetAngularDampingThreshold(_object2->body_id_, 55.0f);
			_object2->is_rotational_ = true;
		}
	} else {
		dJointAttach(_joint_info->joint_id_, _object1->geom_id_->body, 0);
		_object1->is_rotational_ = true;
	}

	/*if ((_object1 != 0 && _object1->force_feedback_id_ != 0) ||
	   (_object2 != 0 && _object2->force_feedback_id_ != 0)) {
		dJointSetFeedback(_joint_info->joint_id_, &_joint_info->feedback_);
	}*/

	dJointSetHingeAnchor(_joint_info->joint_id_, anchor_pos.x, anchor_pos.y, anchor_pos.z);
	dJointSetHingeAxis(_joint_info->joint_id_, axis.x, axis.y, axis.z);

	joint_table_.insert(_joint_info);
	return (JointID)_joint_info;
}

PhysicsManager::JointID PhysicsManagerODE::CreateHinge2Joint(BodyID body1, BodyID body2,
	const vec3& anchor_pos, const vec3& axis1,
	const vec3& axis2) {
	Object* _object1;
	Object* _object2;
	if (CheckBodies2(body1, body2, _object1, _object2, "CreateHinge2Joint") == false)
		return INVALID_JOINT;

	JointInfo* _joint_info = joint_info_allocator_.Alloc();
	_joint_info->joint_id_ = dJointCreateHinge2(world_id_, 0);
	_joint_info->type_ = kJointHinge2;
	_joint_info->body1_id_ = body1;
	_joint_info->body2_id_ = body2;
	_joint_info->listener_id1_ = _object1->force_feedback_id_;
	_joint_info->listener_id2_ = _object2->force_feedback_id_;

	dJointAttach(_joint_info->joint_id_, _object1->body_id_, _object2->body_id_);

	_object1->has_mass_children_ = true;
	_object2->has_mass_children_ = true;
	_object2->is_rotational_ = true;
	/*if (_object1->force_feedback_id_ != 0 ||
	   _object2->force_feedback_id_ != 0) {
		dJointSetFeedback(_joint_info->joint_id_, &_joint_info->feedback_);
	}*/

	dJointSetHinge2Anchor(_joint_info->joint_id_, anchor_pos.x, anchor_pos.y, anchor_pos.z);
	dJointSetHinge2Axis1(_joint_info->joint_id_, axis1.x, axis1.y, axis1.z);
	dJointSetHinge2Axis2(_joint_info->joint_id_, axis2.x, axis2.y, axis2.z);

	joint_table_.insert(_joint_info);
	return (JointID)_joint_info;
}

PhysicsManager::JointID PhysicsManagerODE::CreateUniversalJoint(BodyID body1, BodyID body2,
	const vec3& anchor_pos, const vec3& axis1,
	const vec3& axis2) {
	Object* _object1;
	Object* _object2;
	if (CheckBodies(body1, body2, _object1, _object2, "CreateUniversalJoint") == false)
		return INVALID_JOINT;

	JointInfo* _joint_info = joint_info_allocator_.Alloc();
	_joint_info->joint_id_ = ::dJointCreateUniversal(world_id_, 0);
	_joint_info->type_ = kJointUniversal;
	_joint_info->body1_id_ = body1;
	_joint_info->body2_id_ = body2;
	_joint_info->listener_id1_ = _object1->force_feedback_id_;
	_joint_info->listener_id2_ = _object2->force_feedback_id_;

	_object1->has_mass_children_ = true;
	if (_object2 != 0) {
		_object2->has_mass_children_ = true;
		::dJointAttach(_joint_info->joint_id_, _object1->body_id_, _object2->body_id_);
	} else {
		::dJointAttach(_joint_info->joint_id_, _object1->body_id_, 0);
	}

	/*if ((_object1 != 0 && _object1->force_feedback_id_ != 0) ||
	   (_object2 != 0 && _object2->force_feedback_id_ != 0)) {
		dJointSetFeedback(_joint_info->joint_id_, &_joint_info->feedback_);
	}*/

	::dJointSetUniversalAnchor(_joint_info->joint_id_, anchor_pos.x, anchor_pos.y, anchor_pos.z);
	::dJointSetUniversalAxis1(_joint_info->joint_id_, axis1.x, axis1.y, axis1.z);
	::dJointSetUniversalAxis2(_joint_info->joint_id_, axis2.x, axis2.y, axis2.z);

	joint_table_.insert(_joint_info);
	return (JointID)_joint_info;
}

PhysicsManager::JointID PhysicsManagerODE::CreateSliderJoint(BodyID body1, BodyID body2,
	const vec3& axis) {
	Object* _object1;
	Object* _object2;
	if (CheckBodies(body1, body2, _object1, _object2, "CreateSliderJoint") == false)
		return INVALID_JOINT;

	JointInfo* _joint_info = joint_info_allocator_.Alloc();
	_joint_info->joint_id_ = dJointCreateSlider(world_id_, 0);
	_joint_info->type_ = kJointSlider;
	_joint_info->body1_id_ = body1;
	_joint_info->body2_id_ = body2;
	_joint_info->listener_id1_ = _object1->force_feedback_id_;
	_joint_info->listener_id2_ = _object2->force_feedback_id_;

	_object1->has_mass_children_ = true;
	if (_object2 != 0) {
		_object2->has_mass_children_ = true;
		dJointAttach(_joint_info->joint_id_, _object1->body_id_, _object2->body_id_);
	} else {
		dJointAttach(_joint_info->joint_id_, _object1->body_id_, 0);
	}

	/*if ((_object1 != 0 && _object1->force_feedback_id_ != 0) ||
	   (_object2 != 0 && _object2->force_feedback_id_ != 0)) {
		dJointSetFeedback(_joint_info->joint_id_, &_joint_info->feedback_);
	}*/

	dJointSetSliderAxis(_joint_info->joint_id_, axis.x, axis.y, axis.z);

	joint_table_.insert(_joint_info);
	return (JointID)_joint_info;
}

PhysicsManager::JointID PhysicsManagerODE::CreateFixedJoint(BodyID body1, BodyID body2) {
	Object* _object1;
	Object* _object2;
	if (CheckBodies(body1, body2, _object1, _object2, "CreateFixedJoint") == false)
		return INVALID_JOINT;

	JointInfo* _joint_info = joint_info_allocator_.Alloc();
	_joint_info->joint_id_ = dJointCreateFixed(world_id_, 0);
	_joint_info->type_ = kJointFixed;
	_joint_info->body1_id_ = body1;
	_joint_info->body2_id_ = body2;
	_joint_info->listener_id1_ = _object1->force_feedback_id_;
	_joint_info->listener_id2_ = _object2->force_feedback_id_;

	_object1->has_mass_children_ = true;
	if (_object2 != 0) {
		_object2->has_mass_children_ = true;
		dJointAttach(_joint_info->joint_id_, _object1->body_id_, _object2->body_id_);
	} else {
		dJointAttach(_joint_info->joint_id_, _object1->body_id_, 0);
	}

	/*if ((_object1 != 0 && _object1->force_feedback_id_ != 0) ||
	   (_object2 != 0 && _object2->force_feedback_id_ != 0)) {
		dJointSetFeedback(_joint_info->joint_id_, &_joint_info->feedback_);
	}*/

	joint_table_.insert(_joint_info);
	return (JointID)_joint_info;
}

PhysicsManager::JointID PhysicsManagerODE::CreateAngularMotorJoint(BodyID body1, BodyID body2,
	const vec3& axis) {
	Object* _object1;
	Object* _object2;
	if (CheckBodies(body1, body2, _object1, _object2, "CreateAngularMotorJoint") == false)
		return INVALID_JOINT;

	JointInfo* _joint_info = joint_info_allocator_.Alloc();
	_joint_info->joint_id_ = dJointCreateAMotor(world_id_, 0);
	_joint_info->type_ = kJointAngularmotor;
	_joint_info->body1_id_ = body1;
	_joint_info->body2_id_ = body2;
	_joint_info->listener_id1_ = _object1->force_feedback_id_;
	_joint_info->listener_id2_ = _object2->force_feedback_id_;

	_object1->has_mass_children_ = true;
	if (_object2 != 0) {
		dJointAttach(_joint_info->joint_id_, _object1->body_id_, _object2->body_id_);
		_object2->has_mass_children_ = true;
		_object2->is_rotational_ = true;
	} else {
		dJointAttach(_joint_info->joint_id_, _object1->body_id_, 0);
		_object1->is_rotational_ = true;
	}

	/*if ((_object1 != 0 && _object1->force_feedback_id_ != 0) ||
	   (_object2 != 0 && _object2->force_feedback_id_ != 0)) {
		dJointSetFeedback(_joint_info->joint_id_, &_joint_info->feedback_);
	}*/

	dJointSetAMotorMode(_joint_info->joint_id_, dAMotorUser);
	dJointSetAMotorNumAxes(_joint_info->joint_id_, 1);

	// Set axis 0, relative to body 1.
	dJointSetAMotorAxis(_joint_info->joint_id_, 0, 1, axis.x, axis.y, axis.z);

	joint_table_.insert(_joint_info);
	return (JointID)_joint_info;
}

void PhysicsManagerODE::DeleteJoint(JointID joint_id) {
	JointInfo* _joint_info = (JointInfo*)joint_id;
	dJointDestroy(_joint_info->joint_id_);
	RemoveJoint(_joint_info);

	Object* _object1;
	Object* _object2;
	if (_joint_info->body1_id_ && CheckBodies(_joint_info->body1_id_, _joint_info->body2_id_, _object1, _object2, "DeleteJoint")) {
		if (_object2) {
			_object2->is_rotational_ = false;
		} else {
			_object1->is_rotational_ = false;
		}
	}
}

bool PhysicsManagerODE::StabilizeJoint(JointID joint_id) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Warningf("Couldn't find joint %i!", joint_id);
		return (false);
	}
	bool ok = false;
	JointInfo* _joint_info = *x;
	switch (_joint_info->type_) {
		case kJointBall: {
			dVector3 anchor;
			::dJointGetBallAnchor(_joint_info->joint_id_, anchor);
			dVector3 __anchor2;
			::dJointGetBallAnchor2(_joint_info->joint_id_, __anchor2);
			dxBody* child_body = _joint_info->joint_id_->node[1].body;
			if (!child_body) {
				child_body = _joint_info->joint_id_->node[0].body;
			}
			const dReal* __pos = ::dBodyGetPosition(child_body);
			anchor[0] = __pos[0] + anchor[0] - __anchor2[0];
			anchor[1] = __pos[1] + anchor[1] - __anchor2[1];
			anchor[2] = __pos[2] + anchor[2] - __anchor2[2];
			::dBodySetPosition(child_body, anchor[0], anchor[1], anchor[2]);
			const dReal* vel = ::dBodyGetLinearVel(child_body);
			if (::fabs(vel[0]) > 50 || ::fabs(vel[1]) > 50 || ::fabs(vel[2]) > 50) {
				::dBodySetLinearVel(child_body, 0, 0, 0);
				::dBodySetAngularVel(child_body, 0, 0, 0);
			}
		} break;
		default: {
			log_.Errorf("Joint type %i of non-1-type!", _joint_info->type_);
			deb_assert(false);
		} break;
	}
	return (ok);
}

void PhysicsManagerODE::SetIsGyroscope(BodyID body_id, bool is_gyro) {
	Object* _object = (Object*)body_id;
	if (_object->body_id_) {
		::dBodySetGyroscopicMode(_object->body_id_, is_gyro);
	}
}

bool PhysicsManagerODE::GetJoint1Diff(BodyID body_id, JointID joint_id, Joint1Diff& diff) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		//log_.Warningf("Couldn't find joint %i!", joint_id);
		return (false);
	}
	bool ok = false;
	const JointInfo* _joint_info = *x;
	switch (_joint_info->type_) {
		case kJointHinge: {
			ok = GetHingeDiff(body_id, joint_id, diff);
		} break;
		case kJointSlider: {
			ok = GetSliderDiff(body_id, joint_id, diff);
		} break;
		default: {
			log_.Errorf("Joint type %i of non-1-type!", _joint_info->type_);
			deb_assert(false);
		} break;
	}
	return (ok);
}

bool PhysicsManagerODE::SetJoint1Diff(BodyID body_id, JointID joint_id, const Joint1Diff& diff) {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Warningf("Couldn't find joint %i!", joint_id);
		return (false);
	}
	bool ok = false;
	JointInfo* _joint_info = (JointInfo*)*x;
	switch (_joint_info->type_) {
		case kJointHinge: {
			ok = SetHingeDiff(body_id, joint_id, diff);
		} break;
		case kJointSlider: {
			ok = SetSliderDiff(body_id, joint_id, diff);
		} break;
		default: {
			log_.Errorf("Joint type %i of non-1-type!", _joint_info->type_);
			deb_assert(false);
		} break;
	}
	return (ok);
}

bool PhysicsManagerODE::GetJoint2Diff(BodyID body_id, JointID joint_id, Joint2Diff& diff) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		//log_.Warningf("Couldn't find joint %i!", joint_id);
		return (false);
	}
	bool ok = false;
	JointInfo* _joint_info = (JointInfo*)*x;
	switch (_joint_info->type_) {
		case kJointUniversal: {
			ok = GetUniversalDiff(body_id, joint_id, diff);
		} break;
		default: {
			log_.Errorf("Joint type %i of non-2-type!", _joint_info->type_);
			deb_assert(false);
		} break;
	}
	return (ok);
}

bool PhysicsManagerODE::SetJoint2Diff(BodyID body_id, JointID joint_id, const Joint2Diff& diff) {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Warningf("Couldn't find joint %i!", joint_id);
		return (false);
	}
	bool ok = false;
	JointInfo* _joint_info = (JointInfo*)*x;
	switch (_joint_info->type_) {
		case kJointUniversal: {
			ok = SetUniversalDiff(body_id, joint_id, diff);
		} break;
		default: {
			log_.Errorf("Joint type %i of non-2-type!", _joint_info->type_);
			deb_assert(false);
		} break;
	}
	return (ok);
}

bool PhysicsManagerODE::GetJoint3Diff(BodyID body_id, JointID joint_id, Joint3Diff& diff) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		//log_.Warningf("Couldn't find joint %i!", joint_id);
		return (false);
	}
	bool ok = false;
	JointInfo* _joint_info = (JointInfo*)*x;
	switch (_joint_info->type_) {
		case kJointHinge2: {
			ok = GetHinge2Diff(body_id, joint_id, diff);
		} break;
		case kJointBall: {
			ok = GetBallDiff(body_id, joint_id, diff);
		} break;
		default: {
			log_.Errorf("Joint type %i of non-3-type!", _joint_info->type_);
			deb_assert(false);
		} break;
	}
	return (ok);
}

bool PhysicsManagerODE::SetJoint3Diff(BodyID body_id, JointID joint_id, const Joint3Diff& diff) {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Warningf("Couldn't find joint %i!", joint_id);
		return (false);
	}
	bool ok = false;
	JointInfo* _joint_info = (JointInfo*)*x;
	switch (_joint_info->type_) {
		case kJointHinge2: {
			ok = SetHinge2Diff(body_id, joint_id, diff);
		} break;
		case kJointBall: {
			ok = SetBallDiff(body_id, joint_id, diff);
		} break;
		default: {
			log_.Errorf("Joint type %i of non-3-type!", _joint_info->type_);
			deb_assert(false);
		} break;
	}
	return (ok);
}

void PhysicsManagerODE::RemoveJoint(JointInfo* joint_info) {
	joint_info->listener_id1_ = 0;
	joint_info->listener_id2_ = 0;
	joint_info->body1_id_ = 0;
	joint_info->body2_id_ = 0;
	joint_info_allocator_.Free(joint_info);
}

bool PhysicsManagerODE::GetHingeDiff(BodyID body_id, JointID joint_id, Joint1Diff& diff) const {
	JointInfo* _joint_info = (JointInfo*)joint_id;
	deb_assert(_joint_info->type_ == kJointHinge);
	if (_joint_info->type_ != kJointHinge) {
		log_.Errorf("Joint type %i of non-hinge-type!", _joint_info->type_);
		return (false);
	}

	vec3 _axis;
	if (!GetAxis1(joint_id, _axis) || !GetAngle1(joint_id, diff.value_)) {
		return (false);
	}

	{
		vec3 _velocity;
		GetBodyAngularVelocity(body_id, _velocity);
		diff.velocity_ = _axis * _velocity;
	}

	{
		vec3 _acceleration;
		GetBodyTorque(body_id, _acceleration);
		diff.acceleration_ = _axis * _acceleration;
	}

	return (true);
}

bool PhysicsManagerODE::SetHingeDiff(BodyID body_id, JointID joint_id, const Joint1Diff& diff) {
	JointInfo* _joint_info = (JointInfo*)joint_id;
	deb_assert(_joint_info->type_ == kJointHinge);
	if (_joint_info->type_ != kJointHinge) {
		log_.Errorf("Joint type %i of non-hinge-type!", _joint_info->type_);
		return (false);
	}

	vec3 anchor;
	vec3 _axis;
	if (!GetAnchorPos(joint_id, anchor) || !GetAxis1(joint_id, _axis)) {
		return (false);
	}
	deb_assert(_axis.GetLengthSquared() > 0.99f && _axis.GetLengthSquared() < 1.01f);

	dxBody* parent_body = _joint_info->joint_id_->node[0].body;

	// Fetch parent data (or identity if parent is World).
	const dReal* __pos = ::dBodyGetPosition(parent_body);
	const vec3 parent_position(__pos);
	quat parent_q(-1, 0, 0, 0);
	vec3 parent_velocity;
	vec3 parent_acceleration;
	if (!_joint_info->joint_id_->node[1].body) {
	} else {
		deb_assert(_joint_info->joint_id_->node[1].body == ((Object*)body_id)->body_id_);
		const dReal* pq = ::dBodyGetQuaternion(parent_body);
		parent_q.Set(pq);
	}

	{
		// Rotate to original child (us) orientation.
		dxJointHinge* __hinge = (dxJointHinge*)_joint_info->joint_id_;
		quat __q(__hinge->qrel);
		// Set orientation.
		xform _transform;
		GetBodyTransform(body_id, _transform);
		if (_joint_info->joint_id_->node[1].body) {
			__q = parent_q * __q;
		} else {
			__q.MakeInverse();
		}
		// Rotate to input angle.
		__q = quat(-diff.value_, _axis) * __q;
		_transform.SetOrientation(__q);
		if (_joint_info->joint_id_->node[1].body) {
			// Align anchors (happen after rotation) and store.
			vec3 __anchor2(__hinge->anchor2);
			__anchor2 = __q*__anchor2 + _transform.GetPosition();
			_transform.GetPosition() += anchor-__anchor2;
		}
		SetBodyTransform(body_id, _transform);
	}

	{
		if (diff.velocity_ < PIF*1000) {
			vec3 _velocity;
			GetBodyAngularVelocity(body_id, _velocity);
			// Drop angular velocity along axis 1 & 2, then add the specified amount.
			vec3 axis_velocity = _axis*(_axis*_velocity);
			_velocity -= axis_velocity;
			_velocity += _axis * diff.velocity_;
			SetBodyAngularVelocity(body_id, _velocity);
		}
	}

	{
		if (diff.acceleration_ < PIF*1000) {
			vec3 _acceleration;
			GetBodyTorque(body_id, _acceleration);
			// Drop angular acceleration along axis, then add the specified amount.
			vec3 axis_acceleration = _axis*(_axis*_acceleration);
			_acceleration -= axis_acceleration;
			_acceleration += _axis * diff.acceleration_;
			SetBodyTorque(body_id, _acceleration);
		}
	}

	return (true);
}

bool PhysicsManagerODE::GetSliderDiff(BodyID body_id, JointID joint_id, Joint1Diff& diff) const {
	JointInfo* _joint_info = (JointInfo*)joint_id;
	deb_assert(_joint_info->type_ == kJointSlider);
	if (_joint_info->type_ != kJointSlider) {
		log_.Errorf("Joint type %i of non-slider-type!", _joint_info->type_);
		return (false);
	}

	vec3 _axis;
	if (!GetAxis1(joint_id, _axis)) {
		return (false);
	}
	deb_assert(_axis.GetLengthSquared() > 0.99f && _axis.GetLengthSquared() < 1.01f);

	GetSliderPos(joint_id, diff.value_);
	GetSliderSpeed(joint_id, diff.velocity_);

	{
		vec3 _acceleration;
		GetBodyForce(body_id, _acceleration);
		dxBody* parent_body = _joint_info->joint_id_->node[0].body;
		const dReal* parent_force = ::dBodyGetForce(parent_body);
		_acceleration -= vec3(parent_force);
		diff.acceleration_ = _axis * _acceleration;
	}

	return (true);
}

bool PhysicsManagerODE::SetSliderDiff(BodyID body_id, JointID joint_id, const Joint1Diff& diff) {
	JointInfo* _joint_info = (JointInfo*)joint_id;
	deb_assert(_joint_info->type_ == kJointSlider);
	if (_joint_info->type_ != kJointSlider) {
		log_.Errorf("Joint type %i of non-hinge-type!", _joint_info->type_);
		return (false);
	}

	vec3 _axis;
	if (!GetAxis1(joint_id, _axis)) {
		return (false);
	}
	deb_assert(_axis.GetLengthSquared() > 0.99f && _axis.GetLengthSquared() < 1.01f);

	dxBody* parent_body = _joint_info->joint_id_->node[0].body;

	// Fetch parent data (or identity if parent is World).
	const dReal* __pos = ::dBodyGetPosition(parent_body);
	const vec3 parent_position(__pos);
	quat parent_q(-1, 0, 0, 0);
	vec3 parent_velocity;
	vec3 parent_acceleration;
	if (!_joint_info->joint_id_->node[1].body) {
	} else {
		deb_assert(_joint_info->joint_id_->node[1].body == ((Object*)body_id)->body_id_);
		const dReal* pq = ::dBodyGetQuaternion(parent_body);
		parent_q.Set(pq);
		const dReal* parent_v = ::dBodyGetLinearVel(parent_body);
		parent_velocity.Set(parent_v);
		const dReal* parent_force = ::dBodyGetForce(parent_body);
		parent_acceleration.Set(parent_force);
		// Downscale acceleration with mass.
		parent_acceleration *= _joint_info->joint_id_->node[1].body->mass.mass / parent_body->mass.mass;
	}

	{
		// Rotate to original child (us) orientation.
		dxJointSlider* __slider = (dxJointSlider*)_joint_info->joint_id_;
		quat __q(__slider->qrel);
		// Relative translation.
		vec3 __offset(__slider->offset);
		if (_joint_info->joint_id_->node[1].body) {
			__q = parent_q * __q;
			__offset = __q * __offset;
		} else {
			__q.MakeInverse();
			__offset = parent_position - __offset;
		}
		__offset += _axis*diff.value_;
		// Small translational diff, no orientational diff (world parent is stiff) means we get a
		// better experiance by not forcing this tiny jerk into the game. Instead let the local
		// (most probably client) physics engine be chief.
		if (_joint_info->joint_id_->node[1].body || __offset.GetLengthSquared() > 0.5f) {
			// Set orientation.
			xform _transform(__q,
				parent_position - __offset);
			SetBodyTransform(body_id, _transform);
		}
	}

	{
		parent_velocity -= _axis*diff.velocity_;
		SetBodyVelocity(body_id, parent_velocity);
	}

	{
		parent_acceleration -= _axis*diff.acceleration_;
		SetBodyForce(body_id, parent_acceleration);
	}

	return (true);
}

bool PhysicsManagerODE::GetUniversalDiff(BodyID body_id, JointID joint_id, Joint2Diff& diff) const {
	LEPRA_DEBUG_CODE(JointInfo* _joint_info = (JointInfo*)joint_id;)
	LEPRA_DEBUG_CODE(deb_assert(_joint_info->type_ == kJointUniversal));

	vec3 _axis1;
	vec3 _axis2;
	{
		if (!GetAxis1(joint_id, _axis1) || !GetAxis2(joint_id, _axis2) ||
			!GetAngle1(joint_id, diff.value_) || !GetAngle2(joint_id, diff.angle_)) {
			return (false);
		}
		deb_assert(_axis1.GetLengthSquared() > 0.99f && _axis1.GetLengthSquared() < 1.01f);
		deb_assert(_axis2.GetLengthSquared() > 0.99f && _axis2.GetLengthSquared() < 1.01f);
	}

	{
		if (!GetAngleRate1(joint_id, diff.value_velocity_) || !GetAngleRate2(joint_id, diff.angle_velocity_)) {
			return (false);
		}
	}

	{
		vec3 _acceleration;
		GetBodyTorque(body_id, _acceleration);
		diff.value_acceleration_ = -(_axis2 * _acceleration);
		diff.angle_acceleration_ = -(_axis1 * _acceleration);
	}

	return (true);
}

bool PhysicsManagerODE::SetUniversalDiff(BodyID body_id, JointID joint_id, const Joint2Diff& diff) {
	JointInfo* _joint_info = (JointInfo*)joint_id;
	deb_assert(_joint_info->type_ == kJointUniversal);

	vec3 _axis1;
	vec3 _axis2;
	vec3 anchor;
	if (!GetAnchorPos(joint_id, anchor) || !GetAxis1(joint_id, _axis1)) {
		return (false);
	}
	deb_assert(_axis1.GetLengthSquared() > 0.99f && _axis1.GetLengthSquared() < 1.01f);

	{
		// Fetch parent orientation.
		deb_assert(!_joint_info->joint_id_->node[1].body || _joint_info->joint_id_->node[1].body == ((Object*)body_id)->body_id_);
		dxBody* parent_body = _joint_info->joint_id_->node[0].body;
		const dReal* pq = ::dBodyGetQuaternion(parent_body);
		const quat parent_q(pq);
		const dReal* pp = ::dBodyGetPosition(parent_body);
		vec3 parent_p(pp);
		dxJointUniversal* __universal = (dxJointUniversal*)_joint_info->joint_id_;
		// TODO: get your linear algebra shit together!
		quat q;
		q.RotateAroundVector(_axis1, diff.value_);
		q = quat(__universal->qrel2) * q;	// Cross piece -> child.
		q.RotateAroundVector(_axis2, diff.angle_);
		xform child(q, vec3(__universal->anchor2));
		xform cross_piece(quat(__universal->qrel1), vec3(__universal->anchor1));	// qrel1 is parent -> cross piece.
		xform parent(parent_q, parent_p);
		cross_piece = parent.Transform(cross_piece);
		child = cross_piece.Transform(child);
		// TODO:
		//SetBodyTransform(body_id, child);
	}

	{
		vec3 axis_velocity;
		vec3 original_velocity;
		GetBodyAngularVelocity(body_id, original_velocity);
		vec3 _velocity = original_velocity;
		// Drop angular velocity along axis 1 & 2, then add the specified amount.
		if (diff.value_velocity_ < PIF*1000) {
			axis_velocity = _axis1*(_axis1*original_velocity);
			_velocity -= axis_velocity;
			_velocity += _axis1 * -diff.value_velocity_;
		}
		if (diff.angle_velocity_ < PIF*1000) {
			axis_velocity = _axis2*(_axis2*original_velocity);
			_velocity -= axis_velocity;
			_velocity += _axis2 * -diff.angle_velocity_;
		}
		//SetBodyVelocity(body_id, vec3());
		SetBodyAngularVelocity(body_id, _velocity);
	}

	{
		vec3 axis_acceleration;
		vec3 original_acceleration;
		GetBodyTorque(body_id, original_acceleration);
		vec3 _acceleration = original_acceleration;
		// Drop angular acceleration along axis 1 & 2, then add the specified amount.
		if (diff.value_acceleration_ < PIF*1000) {
			axis_acceleration = _axis1*(_axis1*original_acceleration);
			_acceleration -= axis_acceleration;
			_acceleration += _axis1 * -diff.value_acceleration_;
		}
		if (diff.angle_acceleration_ < PIF*1000) {
			axis_acceleration = _axis2*(_axis2*original_acceleration);
			_acceleration -= axis_acceleration;
			_acceleration += _axis2 * -diff.angle_acceleration_;
		}
		//SetBodyForce(body_id, vec3());
		SetBodyTorque(body_id, _acceleration);
	}

	return (true);
}

bool PhysicsManagerODE::GetHinge2Diff(BodyID body_id, JointID joint_id, Joint3Diff& diff) const {
	LEPRA_DEBUG_CODE(JointInfo* _joint_info = (JointInfo*)joint_id;)
	LEPRA_DEBUG_CODE(deb_assert(_joint_info->type_ == kJointHinge2));

	vec3 _axis1;
	vec3 _axis2;
	{
		xform _transform;
		GetBodyTransform(body_id, _transform);
		vec3 anchor;
		if (!GetAnchorPos(joint_id, anchor) || !GetAxis1(joint_id, _axis1) || !GetAxis2(joint_id, _axis2)) {
			return (false);
		}
		deb_assert(_axis1.GetLengthSquared() > 0.99f && _axis1.GetLengthSquared() < 1.01f);
		deb_assert(_axis2.GetLengthSquared() > 0.99f && _axis2.GetLengthSquared() < 1.01f);
		const vec3 _diff(_transform.GetPosition()-anchor);
		float _position;
		_position = -(_axis1 * _diff);
		diff.value_ = _position;
		diff.angle2_ = 0;	// JB-TODO: use this angle as well (go through body rather than ODE).
		if (!GetAngle1(joint_id, _position)) {	// JB: not available through ODE: || !GetAngle2(joint_id, pAngle2));
			return (false);
		}
		diff.angle1_ = _position;
	}

	{
		vec3 _velocity;
		GetBodyVelocity(body_id, _velocity);
		diff.value_velocity_ = -(_axis1 * _velocity);
		GetBodyAngularVelocity(body_id, _velocity);
		diff.angle1_velocity_ = -(_axis2 * _velocity);
		diff.angle2_velocity_ = -(_axis1 * _velocity);
	}

	{
		vec3 _acceleration;
		GetBodyForce(body_id, _acceleration);
		diff.value_acceleration_ = -(_axis1 * _acceleration);
		GetBodyTorque(body_id, _acceleration);
		diff.angle1_acceleration_ = -(_axis2 * _acceleration);
		diff.angle2_acceleration_ = -(_axis1 * _acceleration);
	}

	return (true);
}

bool PhysicsManagerODE::SetHinge2Diff(BodyID body_id, JointID joint_id, const Joint3Diff& diff) {
	LEPRA_DEBUG_CODE(JointInfo* _joint_info = (JointInfo*)joint_id;)
	LEPRA_DEBUG_CODE(deb_assert(_joint_info->type_ == kJointHinge2));

	vec3 _axis1;
	vec3 _axis2;
	{
		vec3 anchor;
		if (!GetAnchorPos(joint_id, anchor) || !GetAxis1(joint_id, _axis1) || !GetAxis2(joint_id, _axis2)) {
			return (false);
		}
		deb_assert(_axis1.GetLengthSquared() > 0.99f && _axis1.GetLengthSquared() < 1.01f);
		deb_assert(_axis2.GetLengthSquared() > 0.99f && _axis2.GetLengthSquared() < 1.01f);
		const vec3 _diff = _axis1 * -diff.value_;
		xform _transform;
		GetBodyTransform(body_id, _transform);
		_transform.SetPosition(anchor+_diff);
		float current_angle;
		if (diff.angle1_ < PIF*10) {
			if (!GetAngle1(joint_id, current_angle)) {	// JB: not available in ODE: || !GetAngle2(joint_id, lCurrentAngle2));
				return (false);
			}
			_transform.GetOrientation().RotateAroundVector(_axis1, current_angle-diff.angle1_);
		}
		if (diff.angle2_ < PIF*10) {
			//diff.angle2_;	// JB-TODO: use this angle as well.
		}
		SetBodyTransform(body_id, _transform);
	}

	{
		vec3 _velocity;
		GetBodyVelocity(body_id, _velocity);
		// Drop suspension velocity along axis1.
		vec3 axis_velocity(_axis1*(_axis1*_velocity));
		_velocity -= axis_velocity;
		// Add suspension velocity.
		_velocity += _axis1 * -diff.value_velocity_;
		SetBodyVelocity(body_id, _velocity);

		vec3 original_velocity;
		GetBodyAngularVelocity(body_id, original_velocity);
		_velocity = original_velocity;
		// Drop angular velocity along axis 1 & 2, then add the specified amount.
		if (diff.angle1_velocity_ < PIF*1000) {
			axis_velocity = _axis1*(_axis1*original_velocity);
			_velocity -= axis_velocity;
			_velocity += _axis2 * -diff.angle1_velocity_;
		}
		if (diff.angle2_velocity_ < PIF*1000) {
			axis_velocity = _axis2*(_axis2*original_velocity);
			_velocity -= axis_velocity;
			_velocity += _axis1 * -diff.angle2_velocity_;
		}
		SetBodyAngularVelocity(body_id, _velocity);
	}

	{
		vec3 _acceleration;
		GetBodyForce(body_id, _acceleration);
		// Drop suspension acceleration along axis1.
		vec3 axis_acceleration(_axis1*(_axis1*_acceleration));
		_acceleration -= axis_acceleration;
		// Add suspension acceleration.
		_acceleration += _axis1 * -diff.value_acceleration_;
		SetBodyForce(body_id, _acceleration);

		vec3 original_acceleration;
		GetBodyTorque(body_id, original_acceleration);
		_acceleration = original_acceleration;
		// Drop angular acceleration along axis 1 & 2, then add the specified amount.
		if (diff.angle1_acceleration_ < PIF*1000) {
			axis_acceleration = _axis1*(_axis1*original_acceleration);
			_acceleration -= axis_acceleration;
			_acceleration += _axis2 * -diff.angle1_acceleration_;
		}
		if (diff.angle2_acceleration_ < PIF*1000) {
			axis_acceleration = _axis2*(_axis2*original_acceleration);
			_acceleration -= axis_acceleration;
			_acceleration += _axis1 * -diff.angle2_acceleration_;
		}
		SetBodyTorque(body_id, _acceleration);
	}

	return (true);
}

bool PhysicsManagerODE::GetBallDiff(BodyID body_id, JointID joint_id, Joint3Diff& diff) const {
	JointInfo* _joint_info = (JointInfo*)joint_id;
	deb_assert(_joint_info->type_ == kJointBall);

	{
		deb_assert(_joint_info->joint_id_->node[1].body == ((Object*)body_id)->body_id_);
		dxBody* parent_body = _joint_info->joint_id_->node[0].body;
		const dReal* pq = ::dBodyGetQuaternion(parent_body);
		quat parent_q(pq);

		xform _transform;
		GetBodyTransform(body_id, _transform);
		const quat __q = _transform.GetOrientation()/parent_q;
		__q.GetEulerAngles(diff.value_, diff.angle1_, diff.angle2_);
	}

	{
		vec3 _velocity;
		GetBodyAngularVelocity(body_id, _velocity);
		diff.value_velocity_ = _velocity.x;
		diff.angle1_velocity_ = _velocity.y;
		diff.angle2_velocity_ = _velocity.z;
	}

	{
		vec3 _acceleration;
		GetBodyTorque(body_id, _acceleration);
		diff.value_acceleration_ = _acceleration.x;
		diff.angle1_acceleration_ = _acceleration.y;
		diff.angle2_acceleration_ = _acceleration.z;
	}

	return (true);
}

bool PhysicsManagerODE::SetBallDiff(BodyID body_id, JointID joint_id, const Joint3Diff& diff) {
	JointInfo* _joint_info = (JointInfo*)joint_id;
	deb_assert(_joint_info->type_ == kJointBall);

	{
		deb_assert(!_joint_info->joint_id_->node[1].body || _joint_info->joint_id_->node[1].body == ((Object*)body_id)->body_id_);
		dxBody* parent_body = _joint_info->joint_id_->node[0].body;
		const dReal* pq = ::dBodyGetQuaternion(parent_body);
		quat parent_q(pq);

		xform _transform;
		GetBodyTransform(body_id, _transform);
		quat relative_to_parent_q(parent_q/_transform.GetOrientation());
		dVector3 raw_anchor;
		::dJointGetBallAnchor2(_joint_info->joint_id_, raw_anchor);
		vec3 __anchor2(raw_anchor);
		vec3 _position = _transform.GetPosition()-__anchor2;
		_position = relative_to_parent_q*_position;	// Go to parent space.
		quat relative_from_parent_q;
		relative_from_parent_q.SetEulerAngles(diff.value_, diff.angle1_, diff.angle2_);
		_position = relative_from_parent_q*_position;	// Go from parent to given child space.
		_transform.SetOrientation(relative_from_parent_q*parent_q);	// Set complete orientation.
		vec3 anchor;
		if (!GetAnchorPos(joint_id, anchor)) {
			return (false);
		}
		_position += anchor;
		_transform.SetPosition(_position);
		SetBodyTransform(body_id, _transform);
	}

	{
		// TODO: adjust linear velocity.
		//SetBodyVelocity(body_id, vec3(0, 0, 0));
		//SetBodyVelocity(body_id, vec3(diff.value_velocity_, diff.angle1_velocity_, diff.angle2_velocity_));
		SetBodyAngularVelocity(body_id, vec3(diff.value_velocity_, diff.angle1_velocity_, diff.angle2_velocity_));
		//SetBodyAngularVelocity(body_id, vec3(0, 0, 0));
	}

	{
		// TODO: adjust linear acceleration.
		//SetBodyForce(body_id, vec3(0, 0, 0));
		//SetBodyForce(body_id, vec3(diff.value_acceleration_, diff.angle1_acceleration_, diff.angle2_acceleration_));
		SetBodyTorque(body_id, vec3(diff.value_acceleration_, diff.angle1_acceleration_, diff.angle2_acceleration_));
		//SetBodyTorque(body_id, vec3(0, 0, 0));
	}

	return (true);
}

bool PhysicsManagerODE::CheckBodies(BodyID& body1, BodyID& body2, Object*& object1, Object*& object2, const char* function) {
	if (body1 == 0 && body2 == 0) {
		str msg(function);
		msg += " - body1 = body2 = NULL!";
		log_.Error(msg);
		return (false);
	}

	if (body1 == body2) {
		str msg(function);
		msg += " - body1 = body2!";
		log_.Error(msg);
		return (false);
	}

	object1 = 0;
	object2 = 0;

	if (body1 == 0) {
		body1 = body2;
		body2 = INVALID_BODY;

		if (((Object*)body1)->world_id_ == world_id_) {
			object1 = (Object*)body1;
			return (true);
		} else {
			str msg(function);
			msg += " - Body is not part of this world!";
			log_.Error(msg);
			return (false);
		}
	} else {
		if (((Object*)body1)->world_id_ == world_id_) {
			object1 = (Object*)body1;
		} else {
			str msg(function);
			msg += " - Body1 is not part of this world!";
			log_.Error(msg);
			return (false);
		}

		if (((Object*)body2)->world_id_ == world_id_) {
			object2 = (Object*)body2;
		} else {
			str msg(function);
			msg += " - Body2 is not part of this world!";
			log_.Error(msg);
			return (false);
		}

		return (true);
	}
}

bool PhysicsManagerODE::CheckBodies2(BodyID& body1, BodyID& body2, Object*& object1, Object*& object2, const char* function) {
	if (body1 == 0) {
		str msg(function);
		msg += " - body1 = NULL!";
		log_.Error(msg);
		return (false);
	}

	if (body2 == 0) {
		str msg(function);
		msg += " - body2 = NULL!";
		log_.Error(msg);
		return (false);
	}

	if (body1 == body2) {
		str msg(function);
		msg += " - body1 = body2!";
		log_.Error(msg);
		return (false);
	}

	object1 = 0;
	object2 = 0;

	if (((Object*)body1)->world_id_ == world_id_) {
		object1 = (Object*)body1;
	} else {
		str msg(function);
		msg += " - Body1 is not part of this world!";
		log_.Error(msg);
		return (false);
	}

	if (((Object*)body2)->world_id_ == world_id_) {
		object2 = (Object*)body2;
	} else {
		str msg(function);
		msg += " - Body2 is not part of this world!";
		log_.Error(msg);
		return (false);
	}

	return (true);
}

bool PhysicsManagerODE::GetAnchorPos(JointID joint_id, vec3& anchor_pos) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAnchorPos - Couldn't find joint %i!", joint_id);
		return (false);
	}

	dVector3 __pos;

	switch ((*x)->type_) {
	case kJointBall:
		dJointGetBallAnchor((*x)->joint_id_, __pos);
		break;
	case kJointHinge:
		dJointGetHingeAnchor((*x)->joint_id_, __pos);
		break;
	case kJointHinge2:
		dJointGetHinge2Anchor((*x)->joint_id_, __pos);
		break;
	case kJointUniversal:
		dJointGetUniversalAnchor((*x)->joint_id_, __pos);
		break;
	case kJointFixed:
	case kJointAngularmotor:
	case kJointSlider:
		log_.Error("GetAnchorPos() - Joint doesn't have an anchor!");
		return (false);
	default:
		log_.Error("GetAnchorPos() - Unknown joint type!");
		return (false);
	};

	anchor_pos.x = __pos[0];
	anchor_pos.y = __pos[1];
	anchor_pos.z = __pos[2];

	return (true);
}

bool PhysicsManagerODE::GetAxis1(JointID joint_id, vec3& axis1) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAxis1 - Couldn't find joint %i!", joint_id);
		return (false);
	}

	dVector3 _axis;

	switch ((*x)->type_) {
	case kJointHinge:
		dJointGetHingeAxis((*x)->joint_id_, _axis);
		break;
	case kJointHinge2:
		dJointGetHinge2Axis1((*x)->joint_id_, _axis);
		break;
	case kJointUniversal:
		dJointGetUniversalAxis1((*x)->joint_id_, _axis);
		break;
	case kJointAngularmotor:
		dJointGetAMotorAxis((*x)->joint_id_, 0, _axis);
		break;
	case kJointSlider:
		dJointGetSliderAxis((*x)->joint_id_, _axis);
		break;
	case kJointBall:
	case kJointFixed:
		log_.Error("GetAxis1() - Joint doesn't have an axis!");
		return (false);
	default:
		log_.Error("GetAxis1() - Unknown joint type!");
		return (false);
	};

	axis1.x = _axis[0];
	axis1.y = _axis[1];
	axis1.z = _axis[2];

	return (true);
}

bool PhysicsManagerODE::GetAxis2(JointID joint_id, vec3& axis2) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAxis2 - Couldn't find joint %i!", joint_id);
		return (false);
	}

	dVector3 _axis;

	switch ((*x)->type_) {
	case kJointHinge2:
		dJointGetHinge2Axis2((*x)->joint_id_, _axis);
		break;
	case kJointUniversal:
		dJointGetUniversalAxis2((*x)->joint_id_, _axis);
		break;
	case kJointBall:
	case kJointHinge:
	case kJointSlider:
	case kJointFixed:
	case kJointAngularmotor:
		//log_.Error("GetAxis2() - Joint doesn't have two axes!");
		return (false);
	default:
		log_.Error("GetAxis2() - Unknown joint type!");
		return (false);
	};

	axis2.x = _axis[0];
	axis2.y = _axis[1];
	axis2.z = _axis[2];

	return (true);
}

bool PhysicsManagerODE::GetAngle1(JointID joint_id, float32& angle) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAngle1 - Couldn't find joint %i!", joint_id);
		return (false);
	}

	switch ((*x)->type_) {
	case kJointHinge2:
		angle = dJointGetHinge2Angle1((*x)->joint_id_);
		break;
	case kJointUniversal:
		angle = dJointGetUniversalAngle1((*x)->joint_id_);
		break;
	case kJointHinge:
		angle = dJointGetHingeAngle((*x)->joint_id_);
		break;
	case kJointAngularmotor:
		angle = dJointGetAMotorAngle((*x)->joint_id_, 0);
		break;
	case kJointBall:
	case kJointSlider:
	case kJointFixed:
		log_.Error("GetAngle1() - Joint doesn't have an angle!");
		return (false);
	default:
		log_.Error("GetAngle1() - Unknown joint type!");
		return (false);
	};

	return (true);
}

bool PhysicsManagerODE::GetAngle2(JointID joint_id, float32& angle) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAngle2 - Couldn't find joint %i!", joint_id);
		return (false);
	}

	switch ((*x)->type_) {
	case kJointUniversal:
		angle = dJointGetUniversalAngle2((*x)->joint_id_);
		break;
	case kJointHinge2:
		// TODO: implement this!
		log_.Error("GetAngle2() - Missing hinge2 implementation in ODE!");
		return (false);
	case kJointHinge:
	case kJointAngularmotor:
	case kJointBall:
	case kJointSlider:
	case kJointFixed:
		log_.Error("GetAngle2() - Joint doesn't have two angles!");
		return (false);
	default:
		log_.Error("GetAngle2() - Unknown joint type!");
		return (false);
	};

	return (true);
}

bool PhysicsManagerODE::GetAngleRate1(JointID joint_id, float32& angle_rate) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAngleRate1 - Couldn't find joint %i!", joint_id);
		return (false);
	}

	switch ((*x)->type_) {
	case kJointHinge2:
		angle_rate = dJointGetHinge2Angle1Rate((*x)->joint_id_);
		break;
	case kJointUniversal:
		angle_rate = dJointGetUniversalAngle1Rate((*x)->joint_id_);
		break;
	case kJointHinge:
		angle_rate = dJointGetHingeAngleRate((*x)->joint_id_);
		break;
	case kJointAngularmotor:
		angle_rate = dJointGetAMotorAngleRate((*x)->joint_id_, 0);
		break;
	case kJointBall:
	case kJointSlider:
	case kJointFixed:
		log_.Error("GetAngleRate1() - Joint doesn't have an angle!");
		return (false);
	default:
		log_.Error("GetAngleRate1() - Unknown joint type!");
		return (false);
	};

	return (true);
}

bool PhysicsManagerODE::GetAngleRate2(JointID joint_id, float32& angle_rate) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAngleRate2 - Couldn't find joint %i!", joint_id);
		return (false);
	}

	switch ((*x)->type_) {
	case kJointHinge2:
		angle_rate = dJointGetHinge2Angle2Rate((*x)->joint_id_);
		break;
	case kJointUniversal:
		angle_rate = dJointGetUniversalAngle2Rate((*x)->joint_id_);
		break;
	case kJointHinge:
		angle_rate = dJointGetHingeAngleRate((*x)->joint_id_);
		break;
	case kJointBall:
	case kJointSlider:
	case kJointFixed:
	case kJointAngularmotor:
		log_.Error("GetAngleRate2() - Joint doesn't have two angles!");
		return (false);
	default:
		log_.Error("GetAngleRate2() - Unknown joint type!");
		return (false);
	};

	return (true);
}

bool PhysicsManagerODE::SetAngle1(BodyID body_id, JointID joint_id, float32 angle) {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("SetAngle1 - Couldn't find joint %i!", joint_id);
		return (false);
	}

	Object* _object = (Object*)body_id;
	if (_object->world_id_ != world_id_) {
		log_.Errorf("GetBodyData - Body %i is not part of this world!", body_id);
		return (false);
	}

	JointInfo* __joint = (JointInfo*)joint_id;
	switch (__joint->type_) {
		case kJointHinge: {
			deb_assert(::dJointGetBody(__joint->joint_id_, 1) == _object->body_id_);
			const float current_angle = ::dJointGetHingeAngle(__joint->joint_id_);
			if (current_angle == angle) {
				return (true);
			}
			xform _transform;
			GetBodyTransform(body_id, _transform);
			vec3 _axis;
			if (GetAxis1(joint_id, _axis)) {
				dVector3 raw_anchor;
				::dJointGetHingeAnchor((*x)->joint_id_, raw_anchor);
				vec3 anchor(raw_anchor);
				::dJointGetHingeAnchor2((*x)->joint_id_, raw_anchor);
				vec3 __anchor2(raw_anchor);
				_transform.GetPosition() += anchor-__anchor2;
				_transform.RotateAroundAnchor(anchor, _axis, -angle+current_angle);
				SetBodyTransform(body_id, _transform);
				return (true);
			} else {
				log_.Error("SetAngle1() - hinge-2 joint error!");
			}
		} break;
		case kJointHinge2: {
			deb_assert(::dJointGetBody(__joint->joint_id_, 1) == _object->body_id_);
			const float current_angle = ::dJointGetHinge2Angle1(__joint->joint_id_);
			if (current_angle == angle) {
				return (true);
			}
			xform _transform;
			GetBodyTransform(body_id, _transform);
			vec3 _axis1;
			if (GetAxis1(joint_id, _axis1)) {
				_transform.GetOrientation().RotateAroundVector(_axis1, -angle+current_angle);
				SetBodyTransform(body_id, _transform);
				return (true);
			} else {
				log_.Error("SetAngle1() - hinge-2 joint error!");
			}
		} break;
		default: {
			log_.Error("SetAngle1() - Unknown joint type!");
		}
	};

	return (false);
}

bool PhysicsManagerODE::SetAngularMotorAngle(JointID joint_id, float32 angle) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("SetAngularMotorAngle - Couldn't find joint %i!", joint_id);
		return (false);
	}

	if ((*x)->type_ != kJointAngularmotor) {
		log_.Error("SetAngularMotorAngle() - Joint is not an angular motor!");
		return (false);
	}

	dJointSetAMotorAngle((*x)->joint_id_, 0, angle);
	return (true);
}

bool PhysicsManagerODE::SetAngularMotorSpeed(JointID joint_id, float32 speed) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("SetAngularMotorSpeed - Couldn't find joint %i!", joint_id);
		return (false);
	}

	if ((*x)->type_ != kJointAngularmotor) {
		log_.Error("SetAngularMotorSpeed() - Joint is not an angular motor!");
		return (false);
	}

	dJointSetAMotorParam((*x)->joint_id_, dParamVel, speed);
	return (true);
}

bool PhysicsManagerODE::SetMotorMaxForce(JointID joint_id, float32 max_force) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("SetMotorMaxForce - Couldn't find joint %i!", joint_id);
		return (false);
	}


	JointInfo* __joint = *x;
	if (__joint->type_ == kJointAngularmotor) {
		dJointSetAMotorParam(__joint->joint_id_, dParamFMax, max_force);
		return (true);
	} else if (__joint->type_ == kJointHinge2) {
		dJointSetHinge2Param(__joint->joint_id_, dParamFMax, max_force);
		dJointSetHinge2Param(__joint->joint_id_, dParamFMax2, max_force);
		return (true);
	} else if (__joint->type_ == kJointHinge) {
		dJointSetHingeParam(__joint->joint_id_, dParamFMax, max_force);
		dJointSetHingeParam(__joint->joint_id_, dParamFMax2, max_force);
		return (true);
	} else if (__joint->type_ == kJointSlider) {
		dJointSetSliderParam(__joint->joint_id_, dParamFMax, max_force);
		dJointSetSliderParam(__joint->joint_id_, dParamFMax2, max_force);
		return (true);
	}
	log_.Error("SetMotorMaxForce() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsManagerODE::SetAngularMotorRoll(JointID joint_id, float32 max_force, float32 target_velocity) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("SetAngularMotorRoll - Couldn't find joint %i!", joint_id);
		return (false);
	}

	JointInfo* __joint = *x;
	if (__joint->type_ == kJointHinge2) {
		::dJointSetHinge2Param(__joint->joint_id_, dParamFMax2, max_force);
		::dJointSetHinge2Param(__joint->joint_id_, dParamVel2, target_velocity);
		::dBodyEnable(::dJointGetBody(__joint->joint_id_, 1));
		return (true);
	} else if (__joint->type_ == kJointHinge) {
		::dJointSetHingeParam(__joint->joint_id_, dParamFMax, max_force);
		::dJointSetHingeParam(__joint->joint_id_, dParamVel, target_velocity);
		::dBodyEnable(::dJointGetBody(__joint->joint_id_, 1));
		return (true);
	} else if (__joint->type_ == kJointUniversal) {
		::dJointSetUniversalParam(__joint->joint_id_, dParamFMax, max_force);
		::dJointSetUniversalParam(__joint->joint_id_, dParamVel, target_velocity);
		::dJointSetUniversalParam(__joint->joint_id_, dParamFMax2, max_force);
		::dJointSetUniversalParam(__joint->joint_id_, dParamVel2, target_velocity);
		::dBodyEnable(::dJointGetBody(__joint->joint_id_, 1));
		return (true);
	}
	log_.Error("SetAngularMotorRoll() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsManagerODE::GetAngularMotorRoll(JointID joint_id, float32& max_force, float32& target_velocity) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAngularMotorRoll - Couldn't find joint %i!", joint_id);
		return (false);
	}

	JointInfo* __joint = *x;
	if (__joint->type_ == kJointHinge2) {
		max_force = ::dJointGetHinge2Param(__joint->joint_id_, dParamFMax2);
		target_velocity = ::dJointGetHinge2Param(__joint->joint_id_, dParamVel2);
		return (true);
	} else if (__joint->type_ == kJointHinge) {
		max_force = ::dJointGetHingeParam(__joint->joint_id_, dParamFMax);
		target_velocity = ::dJointGetHingeParam(__joint->joint_id_, dParamVel);
		return (true);
	}
	log_.Error("GetAngularMotorRoll() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsManagerODE::SetAngularMotorTurn(JointID joint_id, float32 max_force, float32 target_velocity) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("SetAngularMotorTurn - Couldn't find joint %i!", joint_id);
		return (false);
	}

	JointInfo* __joint = *x;
	if (__joint->type_ == kJointHinge2) {
		dJointSetHinge2Param(__joint->joint_id_, dParamFMax, max_force);
		dJointSetHinge2Param(__joint->joint_id_, dParamVel, target_velocity);
		::dBodyEnable(::dJointGetBody(__joint->joint_id_, 1));
		return (true);
	} else if (__joint->type_ == kJointHinge) {
		dJointSetHingeParam(__joint->joint_id_, dParamFMax, max_force);
		dJointSetHingeParam(__joint->joint_id_, dParamVel, target_velocity);
		::dBodyEnable(::dJointGetBody(__joint->joint_id_, 1));
		return (true);
	}
	log_.Error("SetAngularMotorTurn() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsManagerODE::GetAngularMotorAngle(JointID joint_id, float32& angle) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAngularMotorAngle - Couldn't find joint %i!", joint_id);
		return (false);
	}

	if ((*x)->type_ != kJointAngularmotor) {
		log_.Error("GetAngularMotorAngle() - Joint is not an angular motor!");
		return (false);
	}

	angle = dJointGetAMotorAngle((*x)->joint_id_, 0);
	return (true);
}

bool PhysicsManagerODE::GetAngularMotorSpeed(JointID joint_id, float32& speed) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAngularMotorSpeed - Couldn't find joint %i!", joint_id);
		return (false);
	}

	if ((*x)->type_ != kJointAngularmotor) {
		log_.Error("GetAngularMotorSpeed() - Joint is not an angular motor!");
		return (false);
	}

	speed = dJointGetAMotorParam((*x)->joint_id_, dParamVel);
	return (true);
}

bool PhysicsManagerODE::GetAngularMotorMaxForce(JointID joint_id, float32& max_force) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetAngularMotorMaxForce - Couldn't find joint %i!", joint_id);
		return (false);
	}

	if ((*x)->type_ != kJointAngularmotor) {
		log_.Error("GetAngularMotorMaxForce() - Joint is not an angular motor!");
		return (false);
	}

	max_force = dJointGetAMotorParam((*x)->joint_id_, dParamFMax);
	return (true);
}

bool PhysicsManagerODE::SetMotorTarget(JointID joint_id, float32 max_force, float32 target_velocity) {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("SetMotorTarget - Couldn't find joint %i!", joint_id);
		return (false);
	}

	if ((*x)->type_ != kJointSlider) {
		log_.Error("SetMotorTarget() - Joint is not an angular motor!");
		return (false);
	}

	::dJointSetSliderParam((*x)->joint_id_, dParamFMax, max_force);
	::dJointSetSliderParam((*x)->joint_id_, dParamVel, target_velocity);
	::dBodyEnable(::dJointGetBody((*x)->joint_id_, 1));
	return (true);
}

bool PhysicsManagerODE::SetJointParams(JointID joint_id, float32 low_stop, float32 high_stop, float32 _bounce, int extra_index) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("SetJointParams - Couldn't find joint!", joint_id);
		return (false);
	}

	dJointID _joint_id = (*x)->joint_id_;
	switch ((*x)->type_) {
		case kJointSlider: {
			dJointSetSliderParam(_joint_id, dParamLoStop, low_stop);
			dJointSetSliderParam(_joint_id, dParamHiStop, high_stop);
			dJointSetSliderParam(_joint_id, dParamBounce, _bounce);
			dJointSetSliderParam(_joint_id, dParamBounce2, _bounce);
		} break;
		case kJointHinge: {
			dJointSetHingeParam(_joint_id, dParamLoStop, low_stop);
			dJointSetHingeParam(_joint_id, dParamHiStop, high_stop);
			dJointSetHingeParam(_joint_id, dParamBounce, _bounce);
			dJointSetHingeParam(_joint_id, dParamStopERP, _bounce);
			dJointSetHingeParam(_joint_id, dParamCFM, 1e-11f);
			dJointSetHingeParam(_joint_id, dParamFudgeFactor, 0.01f);
		} break;
		case kJointHinge2: {
			if (extra_index == 0) {
				dJointSetHinge2Param(_joint_id, dParamLoStop, low_stop);
				dJointSetHinge2Param(_joint_id, dParamHiStop, high_stop);
			} else {
				dJointSetHinge2Param(_joint_id, dParamLoStop2, low_stop);
				dJointSetHinge2Param(_joint_id, dParamHiStop2, high_stop);
			}
			dJointSetHinge2Param(_joint_id, dParamBounce, _bounce);
		} break;
		case kJointUniversal: {
			dJointSetUniversalParam(_joint_id, dParamLoStop, low_stop);
			dJointSetUniversalParam(_joint_id, dParamHiStop, high_stop);
			dJointSetUniversalParam(_joint_id, dParamBounce, _bounce);
			dJointSetUniversalParam(_joint_id, dParamFudgeFactor, _bounce);
			dJointSetUniversalParam(_joint_id, dParamCFM, 0);
			dJointSetUniversalParam(_joint_id, dParamLoStop2, low_stop);
			dJointSetUniversalParam(_joint_id, dParamHiStop2, high_stop);
			dJointSetUniversalParam(_joint_id, dParamBounce2, _bounce);
			dJointSetUniversalParam(_joint_id, dParamFudgeFactor2, _bounce);
			dJointSetUniversalParam(_joint_id, dParamCFM2, 0);
		} break;
		case kJointAngularmotor: {
			dJointSetAMotorParam(_joint_id, dParamLoStop, low_stop);
			dJointSetAMotorParam(_joint_id, dParamHiStop, high_stop);
			dJointSetAMotorParam(_joint_id, dParamBounce, _bounce);
		} break;
		case kJointBall:
		case kJointFixed: {
			log_.Error("SetJointParams() - joint doesn't have params!");
		} break;
		default: {
			log_.Error("SetJointParams() - Unknown joint type!");
		}
		return (false);
	}

	return (true);
}

bool PhysicsManagerODE::GetJointParams(JointID joint_id, float32& low_stop, float32& high_stop, float32& _bounce) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		//mLog.Warningf("GetJointParams( - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*x)->type_) {
	case kJointSlider:
		low_stop  = dJointGetSliderParam((*x)->joint_id_, dParamLoStop);
		high_stop = dJointGetSliderParam((*x)->joint_id_, dParamHiStop);
		_bounce   = dJointGetSliderParam((*x)->joint_id_, dParamBounce);
		break;
	case kJointHinge:
		low_stop  = dJointGetHingeParam((*x)->joint_id_, dParamLoStop);
		high_stop = dJointGetHingeParam((*x)->joint_id_, dParamHiStop);
		_bounce   = dJointGetHingeParam((*x)->joint_id_, dParamBounce);
		break;
	case kJointHinge2:
		low_stop  = dJointGetHinge2Param((*x)->joint_id_, dParamLoStop);
		high_stop = dJointGetHinge2Param((*x)->joint_id_, dParamHiStop);
		_bounce   = dJointGetHinge2Param((*x)->joint_id_, dParamBounce);
		break;
	case kJointUniversal:
		low_stop  = dJointGetUniversalParam((*x)->joint_id_, dParamLoStop);
		high_stop = dJointGetUniversalParam((*x)->joint_id_, dParamHiStop);
		_bounce   = dJointGetUniversalParam((*x)->joint_id_, dParamBounce);
		break;
	case kJointAngularmotor:
		low_stop  = dJointGetAMotorParam((*x)->joint_id_, dParamLoStop);
		high_stop = dJointGetAMotorParam((*x)->joint_id_, dParamHiStop);
		_bounce   = dJointGetAMotorParam((*x)->joint_id_, dParamBounce);
		break;
	case kJointBall:
	case kJointFixed:
		log_.Error("GetJointParams() - joint doesn't have params!");
	default:
		log_.Error("GetJointParams() - Unknown joint type!");
		return (false);
	}

	return (true);
}

bool PhysicsManagerODE::SetSuspension(JointID joint_id, float32 frame_time, float32 spring_constant, float32 damping_constant) {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("SetSuspension - Couldn't find joint %i!", joint_id);
		return (false);
	}

	JointInfo* __joint = *x;
	if (__joint->type_ == kJointHinge2) {
		::dJointSetHinge2Param(__joint->joint_id_, dParamSuspensionERP,
			frame_time * spring_constant / (frame_time * spring_constant + damping_constant));
		::dJointSetHinge2Param(__joint->joint_id_, dParamSuspensionCFM,
			1 / (frame_time * spring_constant + damping_constant));
		return (true);
	}
	log_.Error("SetSuspension() - Joint is not a hinge-2!");
	return (false);
}

bool PhysicsManagerODE::GetSuspension(JointID joint_id, float32& erp, float32& cfm) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetSuspension - Couldn't find joint %i!", joint_id);
		return (false);
	}

	JointInfo* __joint = *x;
	if (__joint->type_ == kJointHinge2) {
		erp = ::dJointGetHinge2Param(__joint->joint_id_, dParamSuspensionERP);
		cfm = ::dJointGetHinge2Param(__joint->joint_id_, dParamSuspensionCFM);
		return (true);
	}
	log_.Error("GetSuspension() - Joint is not a hinge-2!");
	return (false);
}

bool PhysicsManagerODE::GetSliderPos(JointID joint_id, float32& _pos) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetSliderPos - Couldn't find joint %i!", joint_id);
		return (false);
	}

	if ((*x)->type_ != kJointSlider) {
		log_.Error("GetSliderPos() - Joint is not a slider!");
		return (false);
	}

	_pos = dJointGetSliderPosition((*x)->joint_id_);
	return (true);
}

bool PhysicsManagerODE::GetSliderSpeed(JointID joint_id, float32& speed) const {
	JointTable::const_iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("GetSliderSpeed - Couldn't find joint %i!", joint_id);
		return (false);
	}

	if ((*x)->type_ != kJointSlider) {
		log_.Error("GetSliderSpeed() - Joint is not a slider!");
		return (false);
	}

	speed = dJointGetSliderPositionRate((*x)->joint_id_);

	return (true);
}

bool PhysicsManagerODE::AddJointForce(JointID joint_id, float32 force) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("AddJointForce - Couldn't find joint %i!", joint_id);
		return (false);
	}

	if ((*x)->type_ != kJointSlider) {
		log_.Error("AddJointForce() - Joint is not a slider!");
		return (false);
	}

	::dJointAddSliderForce((*x)->joint_id_, force);
	::dBodyEnable(::dJointGetBody((*x)->joint_id_, 1));

	return (true);
}

bool PhysicsManagerODE::AddJointTorque(JointID joint_id, float32 torque) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("AddJointTorque - Couldn't find joint %i!", joint_id);
		return (false);
	}

	switch ((*x)->type_) {
	case kJointHinge:
		dJointAddHingeTorque((*x)->joint_id_, torque);
		break;
	case kJointHinge2:
		dJointAddHinge2Torques((*x)->joint_id_, torque, 0);
		break;
	case kJointUniversal:
		dJointAddUniversalTorques((*x)->joint_id_, torque, 0);
		break;
	case kJointAngularmotor:
		dJointAddAMotorTorques((*x)->joint_id_, torque, 0, 0);
		break;
	case kJointBall:
	case kJointFixed:
	case kJointSlider:
		log_.Error("AddJointTorque() - joint is of wrong type!");
	default:
		log_.Error("AddJointTorque() - Unknown joint type!");
		return (false);
	}
	::dBodyEnable(::dJointGetBody((*x)->joint_id_, 1));

	return (true);
}

bool PhysicsManagerODE::AddJointTorque(JointID joint_id, float32 torque1, float32 torque2) {
	JointTable::iterator x = joint_table_.find((JointInfo*)joint_id);
	if (x == joint_table_.end()) {
		log_.Errorf("AddJointTorque - Couldn't find joint %i!", joint_id);
		return (false);
	}

	switch ((*x)->type_) {
	case kJointHinge2:
		dJointAddHinge2Torques((*x)->joint_id_, torque1, torque2);
		break;
	case kJointUniversal:
		dJointAddUniversalTorques((*x)->joint_id_, torque1, torque2);
		break;
	case kJointBall:
	case kJointHinge:
	case kJointFixed:
	case kJointSlider:
	case kJointAngularmotor:
		log_.Error("AddJointTorque() - joint is of wrong type!");
	default:
		log_.Error("AddJointTorque() - Unknown joint type!");
		return (false);
	}
	::dBodyEnable(::dJointGetBody((*x)->joint_id_, 1));

	return (true);
}

void PhysicsManagerODE::AddForce(BodyID body_id, const vec3& force) {
	if (((Object*)body_id)->world_id_ != world_id_) {
		log_.Errorf("AddForce - Body %i is not part of this world!", body_id);
		return;
	}

	if (((Object*)body_id)->body_id_) {
		dBodyEnable(((Object*)body_id)->body_id_);
		dBodyAddForce(((Object*)body_id)->body_id_, force.x, force.y, force.z);
	} else {
		log_.Errorf("AddForce - Body %i is only geometry, not body!", body_id);
	}
}

void PhysicsManagerODE::AddTorque(BodyID body_id, const vec3& torque) {
	if (((Object*)body_id)->world_id_ != world_id_) {
		log_.Errorf("AddTorque - Body %i is not part of this world!", body_id);
		return;
	}

	dBodyEnable(((Object*)body_id)->body_id_);
	dBodyAddTorque(((Object*)body_id)->body_id_, torque.x, torque.y, torque.z);
}

void PhysicsManagerODE::AddRelForce(BodyID body_id, const vec3& force) {
	if (((Object*)body_id)->world_id_ != world_id_) {
		log_.Errorf("AddRelForce - Body %i is not part of this world!", body_id);
		return;
	}

	dBodyEnable(((Object*)body_id)->body_id_);
	dBodyAddRelForce(((Object*)body_id)->body_id_, force.x, force.y, force.z);
}

void PhysicsManagerODE::AddRelTorque(BodyID body_id, const vec3& torque) {
	if (((Object*)body_id)->world_id_ != world_id_) {
		log_.Errorf("AddRelTorque - Body %i is not part of this world!", body_id);
		return;
	}

	dBodyEnable(((Object*)body_id)->body_id_);
	dBodyAddRelTorque(((Object*)body_id)->body_id_, torque.x, torque.y, torque.z);
}

void PhysicsManagerODE::AddForceAtPos(BodyID body_id, const vec3& force,
									 const vec3& _pos) {
	if (((Object*)body_id)->world_id_ != world_id_) {
		log_.Errorf("AddForceAtPos - Body %i is not part of this world!", body_id);
		return;
	}

	dBodyEnable(((Object*)body_id)->body_id_);
	dBodyAddForceAtPos(((Object*)body_id)->body_id_, force.x, force.y, force.z, _pos.x, _pos.y, _pos.z);
}

void PhysicsManagerODE::AddForceAtRelPos(BodyID body_id, const vec3& force,
										const vec3& _pos) {
	if (((Object*)body_id)->world_id_ != world_id_) {
		log_.Errorf("AddForceAtRelPos - Body %i is not part of this world!", body_id);
		return;
	}

	dBodyEnable(((Object*)body_id)->body_id_);
	dBodyAddForceAtRelPos(((Object*)body_id)->body_id_, force.x, force.y, force.z, _pos.x, _pos.y, _pos.z);
}

void PhysicsManagerODE::AddRelForceAtPos(BodyID body_id, const vec3& force,
										const vec3& _pos) {
	if (((Object*)body_id)->world_id_ != world_id_) {
		log_.Errorf("AddRelForceAtPos - Body %i is not part of this world!", body_id);
		return;
	}

	dBodyEnable(((Object*)body_id)->body_id_);
	dBodyAddRelForceAtPos(((Object*)body_id)->body_id_, force.x, force.y, force.z, _pos.x, _pos.y, _pos.z);
}

void PhysicsManagerODE::AddRelForceAtRelPos(BodyID body_id, const vec3& force,
										   const vec3& _pos) {
	if (((Object*)body_id)->world_id_ != world_id_) {
		log_.Errorf("AddRelForceAtRelPos - Body %i is not part of this world!", body_id);
		return;
	}

	dBodyEnable(((Object*)body_id)->body_id_);
	dBodyAddRelForceAtRelPos(((Object*)body_id)->body_id_, force.x, force.y, force.z, _pos.x, _pos.y, _pos.z);
}

void PhysicsManagerODE::RestrictBody(BodyID body_id, float32 max_speed, float32 max_angular_speed) {
	if (((Object*)body_id)->world_id_ != world_id_) {
		log_.Errorf("RestrictBody - Body %i is not part of this world!", body_id);
		return;
	}

	::dBodySetMaxAngularSpeed(((Object*)body_id)->body_id_, max_angular_speed);

	const dReal* l_vel = dBodyGetLinearVel(((Object*)body_id)->body_id_);
	float32 l_speed = l_vel[0] * l_vel[0] + l_vel[1] * l_vel[1] + l_vel[2] * l_vel[2];
	if (l_speed > max_speed * max_speed) {
		float32 k = max_speed / sqrtf(l_speed);
		dBodySetLinearVel(((Object*)body_id)->body_id_, l_vel[0] * k, l_vel[1] * k, l_vel[2] * k);
	}
}

void PhysicsManagerODE::EnableGravity(BodyID body_id, bool enable) {
	if (((Object*)body_id)->world_id_ != world_id_) {
		log_.Errorf("EnableGravity - Body %i is not part of this world!", body_id);
		return;
	}

	if (((Object*)body_id)->body_id_) {
		::dBodySetGravityMode(((Object*)body_id)->body_id_, enable? 1 : 0);
	}
}

void PhysicsManagerODE::SetGravity(const vec3& gravity) {
	dWorldSetGravity(world_id_, gravity.x, gravity.y, gravity.z);
}

vec3 PhysicsManagerODE::GetGravity() const {
	dVector3 _gravity;
	::dWorldGetGravity(world_id_, _gravity);
	return vec3(_gravity);
}

void PhysicsManagerODE::EnableCollideWithSelf(BodyID body_id, bool enable) {
	ObjectTable::iterator x = object_table_.find((Object*)body_id);
	if (x != object_table_.end()) {
		(*x)->collide_with_self_ = enable;
	} else {
		log_.Errorf("EnableCollideWithSelf - body %p is not part of this world!", body_id);
		deb_assert(false);
		return;
	}
}

void PhysicsManagerODE::PreSteps() {
	FlagMovingObjects();
}

void PhysicsManagerODE::StepAccurate(float32 step_size, bool collide) {
	if (step_size > 0) {
		if (collide) dSpaceCollide(space_id_, this, CollisionCallback);
		dWorldStep(world_id_, step_size);

		if (collide) {
			DoForceFeedback();
			dJointGroupEmpty(contact_joint_group_id_);
		}
	}
}

void PhysicsManagerODE::StepFast(float32 step_size, bool collide) {
	if (step_size > 0) {
		if (collide) dSpaceCollide(space_id_, this, CollisionCallback);
		dWorldQuickStep(world_id_, step_size);

		if (collide) {
			DoForceFeedback();
			dJointGroupEmpty(contact_joint_group_id_);
		}
	}
}

bool PhysicsManagerODE::IsColliding(int force_feedback_id) {
	note_force_feedback_id_ = force_feedback_id;
	note_is_collided_ = false;
	dSpaceCollide(space_id_, this, CollisionNoteCallback);
	return note_is_collided_;
}

void PhysicsManagerODE::PostSteps() {
	HandleMovableObjects();
}

void PhysicsManagerODE::DoForceFeedback() {
	TriggerInfoList::iterator y;
	for (y = trigger_info_list_.begin(); y != trigger_info_list_.end(); ++y) {
		const TriggerInfo& trigger_info = *y;
		trigger_callback_->OnTrigger(trigger_info.trigger_id_, trigger_info.trigger_listener_id_, trigger_info.body_listener_id_, trigger_info.body_id_, trigger_info.position_, trigger_info.normal_);
	}
	trigger_info_list_.clear();

	JointList::iterator x;
	for (x = feedback_joint_list_.begin(); x != feedback_joint_list_.end(); ++x) {
		JointInfo* _joint_info = *x;

		const bool is_body1_static = _joint_info->IsBody1Static(this);
		const bool is_body2_static = _joint_info->IsBody2Static(this);
		//const bool lOneIsDynamic = (!is_body1_static || !is_body2_static);
		if (_joint_info->listener_id1_ != _joint_info->listener_id2_ /*&& lOneIsDynamic*/) {
			const dJointFeedback* feedback = &_joint_info->feedback_;
			if (_joint_info->listener_id1_ != 0) {
				force_feedback_callback_->OnForceApplied(
					_joint_info->listener_id1_,
					_joint_info->listener_id2_,
					_joint_info->body1_id_,
					_joint_info->body2_id_,
					vec3(feedback->f1),
					vec3(feedback->t1),
					_joint_info->position_,
					_joint_info->relative_velocity_);
			}
			if (_joint_info->listener_id2_ != 0) {
				if (is_body1_static || is_body2_static) {	// Only a single force/torque pair set?
					force_feedback_callback_->OnForceApplied(
						_joint_info->listener_id2_,
						_joint_info->listener_id1_,
						_joint_info->body2_id_,
						_joint_info->body1_id_,
						vec3(feedback->f1),
						vec3(feedback->t1),
						_joint_info->position_,
						_joint_info->relative_velocity_);
				} else {
					force_feedback_callback_->OnForceApplied(
						_joint_info->listener_id2_,
						_joint_info->listener_id1_,
						_joint_info->body2_id_,
						_joint_info->body1_id_,
						vec3(feedback->f2),
						vec3(feedback->t2),
						_joint_info->position_,
						_joint_info->relative_velocity_);
				}
			}
		}
		RemoveJoint(_joint_info);
	}
	feedback_joint_list_.clear();
}

void PhysicsManagerODE::CollisionCallback(void* data, dGeomID geom1, dGeomID geom2) {
	Object* _object1 = (Object*)dGeomGetData(geom1);
	Object* _object2 = (Object*)dGeomGetData(geom2);

	if ((_object1->force_feedback_id_ && _object1->force_feedback_id_ == _object2->force_feedback_id_) ||	// Same body.
		(_object1->trigger_listener_id_ && _object2->trigger_listener_id_) ||	// Elevator platform trigger moves into down trigger.
		(_object1->force_feedback_id_ && _object1->force_feedback_id_ == _object2->trigger_listener_id_) ||	// Trigger on self.
		(_object2->force_feedback_id_ && _object2->force_feedback_id_ == _object1->trigger_listener_id_)) {	// Trigger on self.
		if (!_object1->collide_with_self_ || !_object2->collide_with_self_) {
			return;
		}
	}

	dBodyID _body1 = ::dGeomGetBody(geom1);
	dBodyID _body2 = ::dGeomGetBody(geom2);

	if (!_body1 && !_body2) {	// Static body or trigger against static body or trigger?
		return;
	}
	if ((_body1 && !::dBodyIsEnabled(_body1)) && (_body2 && !::dBodyIsEnabled(_body2))) {	// Both disabled?
		return;
	}
	// Exit without doing anything if the two bodies are connected by a joint.
	if (AreBodiesConnectedExcluding(_body1, _body2, dJointTypeContact) != 0) {
		if (!_object1->collide_with_self_ || !_object2->collide_with_self_) {
			return;
		}
	}

	dContact contact[8];
	const int trigger_contact_point_count = ::dCollide(geom1, geom2, 8, &contact[0].geom, sizeof(contact[0]));
	if (trigger_contact_point_count <= 0) {
		// In AABB range (since call came here), but no real contact.
		return;
	}

	PhysicsManagerODE* value = (PhysicsManagerODE*)data;
	if (_object1->trigger_listener_id_ != 0) {	// Only trig, no force application.
		vec3 _position(contact[0].geom.pos);
		vec3 __normal(contact[0].geom.normal);
		value->trigger_info_list_.push_back(TriggerInfo((BodyID)_object1, _object1->trigger_listener_id_, _object2->force_feedback_id_, (BodyID)_object2, _position, __normal));
		return;
	}
	if(_object2->trigger_listener_id_ != 0) {	// Only trig, no force application.
		vec3 _position(contact[0].geom.pos);
		vec3 __normal(contact[0].geom.normal);
		value->trigger_info_list_.push_back(TriggerInfo((BodyID)_object2, _object2->trigger_listener_id_, _object1->force_feedback_id_, (BodyID)_object1, _position, __normal));
		return;
	}

	// bounce/slide when both objects are dynamic, non-trigger objects.
	{
		// Fetch force, will be used to scale friction (projected against surface normal).
		vec3 position1 = value->GetBodyPosition((BodyID)_object1);
		vec3 position2 = value->GetBodyPosition((BodyID)_object2);
		vec3 linear_velocity1;
		value->GetBodyVelocity((BodyID)_object1, linear_velocity1);
		vec3 linear_velocity2;
		value->GetBodyVelocity((BodyID)_object2, linear_velocity2);
		vec3 angular_velocity1;
		value->GetBodyAngularVelocity((BodyID)_object1, angular_velocity1);
		vec3 angular_velocity2;
		value->GetBodyAngularVelocity((BodyID)_object2, angular_velocity2);

		dMass __mass;
		float mass1 = 1;
		if (_body1) {
			::dBodyGetMass(_body1, &__mass);
			mass1 = __mass.mass;
		}
		float mass2_propotions = 1;
		if (_body2) {
			::dBodyGetMass(_body2, &__mass);
			mass2_propotions = __mass.mass / mass1;
		}

		// Perform normal collision detection.
		for (int i = 0; i < trigger_contact_point_count; i++) {
			dContact& __c = contact[i];
			vec3 _position(__c.geom.pos);
			vec3 spin;
			const float total_friction = ::fabs(_object1->friction_*_object2->friction_)+0.0001f;
			// Negative friction factor means simple friction model.
			if (_object1->friction_ > 0 || _object2->friction_ > 0) {
				__c.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1 | dContactFDir1 | dContactBounce;

				const vec3 __normal(__c.geom.normal);
				const vec3 collision_point(__c.geom.pos);
				const vec3 distance1(collision_point-position1);
				const vec3 distance2(collision_point-position2);
				const vec3 angular_surface_velocity1 = angular_velocity1.Cross(distance1);
				const vec3 angular_surface_velocity2 = angular_velocity2.Cross(distance2);
				const vec3 surface_velocity1 = -linear_velocity1.ProjectOntoPlane(__normal) + angular_surface_velocity1;
				const vec3 surface_velocity2 = -linear_velocity2.ProjectOntoPlane(__normal) + angular_surface_velocity2;
				spin = surface_velocity1-surface_velocity2;
				const float relative_velocity = spin.GetLength();
				vec3 spin_direction(spin);
				if (spin_direction.GetLengthSquared() <= 1e-4) {
					vec3 dummy;
					__normal.GetOrthogonals(spin_direction, dummy);
				} else {
					spin_direction.Normalize();
				}
				__c.fdir1[0] = spin_direction.x;
				__c.fdir1[1] = spin_direction.y;
				__c.fdir1[2] = spin_direction.z;

				__c.surface.mu = dInfinity;
				const float slip = (1e-4f * relative_velocity + 1e-6f) / total_friction;
				__c.surface.slip1 = slip;
				__c.surface.slip2 = slip;
			} else {
				__c.surface.mode = dContactBounce;
				__c.surface.mu = total_friction * 3 * mass1 * mass2_propotions;
			}
			__c.surface.bounce = (dReal)(_object1->bounce_ * _object2->bounce_);
			__c.surface.bounce_vel = (dReal)0.000001;
			if (__c.surface.bounce < 1e-1f) {
				__c.surface.mode |= dContactSoftERP;
				__c.surface.soft_erp = __c.surface.bounce * 1e1f * world_erp_;
			}
			if (__c.surface.bounce < 1e-7f) {
				__c.surface.mode |= dContactSoftCFM;
				__c.surface.soft_cfm = Math::Lerp(1e8f, 1.0f, __c.surface.bounce * 1e7f) * world_cfm_;
			}

			if (_object1->force_feedback_id_ != 0 ||
			   _object2->force_feedback_id_ != 0) {
				// Create a joint whith feedback info.
				JointInfo* _joint_info = value->joint_info_allocator_.Alloc();
				_joint_info->joint_id_ = dJointCreateContact(value->world_id_, value->contact_joint_group_id_, &__c);
				_joint_info->type_ = kJointContact;
				value->feedback_joint_list_.push_back(_joint_info);
				_joint_info->listener_id1_ = _object1->force_feedback_id_;
				_joint_info->listener_id2_ = _object2->force_feedback_id_;
				_joint_info->position_ = _position;
				_joint_info->relative_velocity_ = spin;

				dJointAttach(_joint_info->joint_id_, _body1, _body2);
				dJointSetFeedback(_joint_info->joint_id_, &_joint_info->feedback_);
				_joint_info->body1_id_ = _object1;
				_joint_info->body2_id_ = _object2;
			} else {
				// Create a temporary joint without feedback info.
				dJointID joint_id = dJointCreateContact(value->world_id_, value->contact_joint_group_id_, &__c);
				dJointAttach(joint_id, _body1, _body2);
			}
		}
	}
}

void PhysicsManagerODE::CollisionNoteCallback(void* data, dGeomID geom1, dGeomID geom2) {
	Object* _object1 = (Object*)dGeomGetData(geom1);
	Object* _object2 = (Object*)dGeomGetData(geom2);

	if (!_object1->force_feedback_id_ ||
		!_object2->force_feedback_id_ ||
		_object1->force_feedback_id_ == _object2->force_feedback_id_) {	// One is trigger, or same body.
		return;
	}

	PhysicsManagerODE* value = (PhysicsManagerODE*)data;
	if (_object1->force_feedback_id_ != value->note_force_feedback_id_ &&
		_object2->force_feedback_id_ != value->note_force_feedback_id_) {	// Not observed body.
		return;
	}

	dContact contact[1];
	const int trigger_contact_point_count = ::dCollide(geom1, geom2, 1, &contact[0].geom, sizeof(contact[0]));
	value->note_is_collided_ |= (trigger_contact_point_count > 0);	// False means in AABB range (since call came here), but no real contact.
}



const PhysicsManager::BodySet& PhysicsManagerODE::GetIdledBodies() {
	auto_disabled_object_set_.clear();
	ObjectTable::iterator x = object_table_.begin();
	for (; x != object_table_.end(); ++x) {
		Object* _object = *x;
		if (_object->did_stop_) {
			auto_disabled_object_set_.insert((BodyID)_object);
		}
	}
	return (auto_disabled_object_set_);
}



void PhysicsManagerODE::FlagMovingObjects() {
	ObjectTable::iterator x = object_table_.begin();
	for (; x != object_table_.end(); ++x) {
		Object* _object = *x;
		if (_object->body_id_ && _object->is_root_ && ::dBodyIsEnabled(_object->body_id_)) {
			_object->did_stop_ = true;
		}
	}
}

void PhysicsManagerODE::HandleMovableObjects() {
	ObjectTable::iterator x = object_table_.begin();
	for (; x != object_table_.end(); ++x) {
		Object* _object = (Object*)(*x);
		if (_object->body_id_ && ::dBodyIsEnabled(_object->body_id_)) {
			_object->did_stop_ = false;
			NormalizeRotation(_object);
		}
	}
}

void PhysicsManagerODE::NormalizeRotation(BodyID object) {
	Object* _object = (Object*)object;
	if (!_object->is_rotational_ && (_object->has_mass_children_ || _object->body_id_->geom->type == dBoxClass)) {
		vec3 _velocity;
		GetBodyAngularVelocity(_object, _velocity);
		const float max_angular_velocity = 12.0f;
		if (_velocity.GetLength() > max_angular_velocity) {
			_velocity.Normalize(max_angular_velocity);
			SetBodyAngularVelocity(_object, _velocity);
			SetBodyTorque(_object, vec3());
		}
	}
}



void PhysicsManagerODE::RayPickCallback(void* data_ptr, dGeomID o1, dGeomID o2) {
	void** _data = (void**)data_ptr;
	int* _force_feedback_ids = (int*)_data[0];
	vec3* _positions = (vec3*)_data[1];
	int* hits = (int*)_data[2];
	const int _max_bodies = *(int*)_data[3];
	if (*hits >= _max_bodies) {
		return;
	}
	dContactGeom contact[4];
	const int collisions = ::dCollide(o1, o2, 1, &contact[0], sizeof(contact[0]));
	if (collisions) {
		Object* _object = (Object*)::dGeomGetData(o1);
		_force_feedback_ids[*hits] = _object->force_feedback_id_;
		_positions[*hits] = vec3(contact[0].pos);
		(*hits)++;
	}
}



float PhysicsManagerODE::world_erp_;
float PhysicsManagerODE::world_cfm_;
loginstance(kPhysics, PhysicsManagerODE);



}
