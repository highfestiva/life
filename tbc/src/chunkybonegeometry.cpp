
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/chunkybonegeometry.h"
#include <algorithm>
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/cyclicarray.h"
#include "../../lepra/include/endian.h"
#include "../../lepra/include/packer.h"
#include "../include/chunkyphysics.h"



namespace tbc {



ChunkyBoneGeometry::ChunkyBoneGeometry(const BodyData& body_data):
	body_data_(body_data),
	joint_id_(INVALID_JOINT),
	body_id_(INVALID_BODY),
	extra_data_(0) {
	AddConnectorType(body_data.connector_type_);
}

ChunkyBoneGeometry::~ChunkyBoneGeometry() {
	// Ensure all resources has been released prior to delete.
	deb_assert(joint_id_ == INVALID_JOINT && body_id_ == INVALID_BODY);
}

void ChunkyBoneGeometry::RelocatePointers(const ChunkyPhysics* target, const ChunkyPhysics* source, const ChunkyBoneGeometry& original) {
	if (original.body_data_.parent_) {
		const int bone_index = source->GetIndex(original.body_data_.parent_);
		deb_assert(bone_index >= 0);
		body_data_.parent_ = target->GetBoneGeometry(bone_index);
	}
}



ChunkyBoneGeometry* ChunkyBoneGeometry::Create(const ChunkyBoneGeometry& original) {
	switch (original.GetGeometryType()) {
		case kGeometryCapsule:	return new ChunkyBoneCapsule((ChunkyBoneCapsule&)original);	break;
		case kGeometryCylinder:	return new ChunkyBoneCylinder((ChunkyBoneCylinder&)original);	break;
		case kGeometrySphere:	return new ChunkyBoneSphere((ChunkyBoneSphere&)original);	break;
		case kGeometryBox:	return new ChunkyBoneBox((ChunkyBoneBox&)original);		break;
		case kGeometryMesh:	return new ChunkyBoneMesh((ChunkyBoneMesh&)original);		break;
	}
	return 0;
}

ChunkyBoneGeometry* ChunkyBoneGeometry::Load(ChunkyPhysics* structure, const void* data, unsigned byte_count) {
	if (byte_count < sizeof(uint32)) {
		log_.Error("Could not load; very small data size.");
		deb_assert(false);
		return (0);
	}

	ChunkyBoneGeometry* geometry = 0;
	const uint32* _data = (const uint32*)data;
	BodyData _body_data(0, 0, 0);
	switch (Endian::BigToHost(_data[0])) {
		case kGeometryCapsule:	geometry = new ChunkyBoneCapsule(_body_data);	break;
		case kGeometryCylinder:	geometry = new ChunkyBoneCylinder(_body_data);	break;
		case kGeometrySphere:	geometry = new ChunkyBoneSphere(_body_data);	break;
		case kGeometryBox:	geometry = new ChunkyBoneBox(_body_data);	break;
		case kGeometryMesh:	geometry = new ChunkyBoneMesh(_body_data);	break;
	}
	if (geometry) {
		if (byte_count == geometry->GetChunkySize(_data)) {
			geometry->LoadChunkyData(structure, _data);
			deb_assert(geometry->GetChunkySize() == byte_count);
		} else {
			log_.Error("Could not load; wrong data size.");
			deb_assert(false);
			delete (geometry);
			geometry = 0;
		}
	}
	return (geometry);
}

bool ChunkyBoneGeometry::CreateJoint(ChunkyPhysics* structure, PhysicsManager* physics, unsigned physics_fps) {
	bool ok = false;
	if (body_data_.parent_) {
		if (GetBoneType() == kBonePosition) {
			// Need not do jack. It's not a physical object.
			ok = true;
		} else if (body_data_.joint_type_ == kJointExclude) {
			ok = physics->Attach(GetBodyId(), body_data_.parent_->GetBodyId());
		} else if (body_data_.joint_type_ == kJointFixed) {
			ok = physics->Attach(GetBodyId(), body_data_.parent_->GetBodyId());
		} else if (body_data_.joint_type_ == kJointSuspendHinge || body_data_.joint_type_ == kJointHinge2) {
			// Calculate axis from given euler angles.
			vec3 suspension_axis(-1, 0, 0);
			vec3 hinge_axis(0, 0, 1);
			quat rotator;
			rotator.SetEulerAngles(body_data_.parameter_[kParamEulerTheta], 0, body_data_.parameter_[kParamEulerPhi]);
			suspension_axis = rotator*suspension_axis;
			hinge_axis = rotator*hinge_axis;

			joint_id_ = physics->CreateHinge2Joint(body_data_.parent_->GetBodyId(),
				GetBodyId(), structure->GetTransformation(this).GetPosition(),
				suspension_axis, hinge_axis);
			physics->SetJointParams(joint_id_, body_data_.parameter_[kParamLowStop], body_data_.parameter_[kParamHighStop], 0);
			physics->SetSuspension(joint_id_, 1/(float)physics_fps, body_data_.parameter_[kParamSpringConstant],
				body_data_.parameter_[kParamSpringDamping]);
			physics->SetAngularMotorRoll(joint_id_, 0, 0);
			physics->SetAngularMotorTurn(joint_id_, 0, 0);
			ok = true;
		} else if (body_data_.joint_type_ == kJointHinge) {
			// Calculate axis from given euler angles.
			vec3 hinge_axis(0, 0, 1);
			quat hinge_rotator;
			hinge_rotator.SetEulerAngles(body_data_.parameter_[kParamEulerTheta], 0, body_data_.parameter_[kParamEulerPhi]);
			hinge_axis = hinge_rotator*hinge_axis;

			const xform& body_transform = structure->GetTransformation(this);
			const vec3 anchor = body_transform.GetPosition() + GetOriginalOffset();
			joint_id_ = physics->CreateHingeJoint(body_data_.parent_->GetBodyId(),
				GetBodyId(), anchor, hinge_axis);
			physics->SetJointParams(joint_id_, body_data_.parameter_[kParamLowStop], body_data_.parameter_[kParamHighStop], body_data_.bounce_);
			physics->SetAngularMotorTurn(joint_id_, 0, 0);
			//physics->GetAxis1(joint_id_, hinge_axis);
			ok = true;
		} else if (body_data_.joint_type_ == kJointSlider) {
			// Calculate axis from given euler angles.
			vec3 axis(0, 0, 1);
			quat rotator;
			rotator.SetEulerAngles(body_data_.parameter_[kParamEulerTheta], 0, body_data_.parameter_[kParamEulerPhi]);
			axis = rotator*axis;

			joint_id_ = physics->CreateSliderJoint(body_data_.parent_->GetBodyId(),
				GetBodyId(), axis);
			physics->SetJointParams(joint_id_, body_data_.parameter_[kParamLowStop], body_data_.parameter_[kParamHighStop], body_data_.bounce_);
			physics->SetMotorTarget(joint_id_, 0, 0);
			ok = true;
		} else if (body_data_.joint_type_ == kJointUniversal) {
			// Calculate axis from given euler angles.
			vec3 axis1(0, 0, 1);
			vec3 axis2(0, 1, 0);
			quat rotator;
			rotator.SetEulerAngles(body_data_.parameter_[kParamEulerTheta], 0, body_data_.parameter_[kParamEulerPhi]);
			axis1 = rotator*axis1;
			axis2 = rotator*axis2;

			const xform& body_transform = structure->GetTransformation(this);
			const vec3 anchor = body_transform.GetPosition() +
				vec3(body_data_.parameter_[kParamOffsetX], body_data_.parameter_[kParamOffsetY], body_data_.parameter_[kParamOffsetZ]);
			joint_id_ = physics->CreateUniversalJoint(body_data_.parent_->GetBodyId(),
				GetBodyId(), anchor, axis1, axis2);
			physics->SetJointParams(joint_id_, body_data_.parameter_[kParamLowStop], body_data_.parameter_[kParamHighStop], 0);
			/*physics->SetJointParams(joint_id_, body_data_.parameter_[kParamLowStop], body_data_.parameter_[kParamHighStop], 0);
			physics->SetSuspension(joint_id_, 1/(float)physics_fps, body_data_.parameter_[0],
				body_data_.parameter_[1]);
			physics->SetAngularMotorRoll(joint_id_, 0, 0);
			physics->SetAngularMotorTurn(joint_id_, 0, 0);*/
			ok = true;
		} else if (body_data_.joint_type_ == kJointBall) {
			const xform& body_transform = structure->GetTransformation(this);
			const vec3 anchor = body_transform.GetPosition() +
				vec3(body_data_.parameter_[kParamOffsetX], body_data_.parameter_[kParamOffsetY], body_data_.parameter_[kParamOffsetZ]);
			joint_id_ = physics->CreateBallJoint(body_data_.parent_->GetBodyId(),
				GetBodyId(), anchor);
			/*physics->SetJointParams(joint_id_, body_data_.parameter_[kParamLowStop], body_data_.parameter_[kParamHighStop], 0);
			physics->SetJointParams(joint_id_, body_data_.parameter_[kParamLowStop], body_data_.parameter_[kParamHighStop], 0);
			physics->SetSuspension(joint_id_, 1/(float)physics_fps, body_data_.parameter_[0],
				body_data_.parameter_[1]);
			physics->SetAngularMotorRoll(joint_id_, 0, 0);
			physics->SetAngularMotorTurn(joint_id_, 0, 0);*/
			ok = true;
		} else {
			deb_assert(false);
		}
	} else {
		deb_assert(body_data_.joint_type_ == kJointExclude);
		ok = true;
	}
	deb_assert(ok);
	return (ok);
}

void ChunkyBoneGeometry::RemovePhysics(PhysicsManager* physics) {
	if (joint_id_ != INVALID_JOINT) {
		if (physics) {
			physics->DeleteJoint(joint_id_);
		}
		joint_id_ = INVALID_JOINT;
	}
	if (body_id_ != INVALID_BODY) {
		if (physics) {
			physics->DeleteBody(body_id_);
		}
		body_id_ = INVALID_BODY;
	}
}



float ChunkyBoneGeometry::GetMass() const {
	return body_data_.mass_;
}

ChunkyBoneGeometry* ChunkyBoneGeometry::GetParent() const {
	return (body_data_.parent_);
}

ChunkyBoneGeometry::JointType ChunkyBoneGeometry::GetJointType() const {
	return (body_data_.joint_type_);
}

void ChunkyBoneGeometry::SetJointType(JointType joint_type) {
	body_data_.joint_type_ = joint_type;
}

bool ChunkyBoneGeometry::IsAffectedByGravity() const {
	return body_data_.is_affected_by_gravity_;
}

bool ChunkyBoneGeometry::IsCollideWithSelf() const {
	return (body_data_.parameter_[kParamCollideWithSelf] != 0);
}

bool ChunkyBoneGeometry::IsDetachable() const {
	return (body_data_.parameter_[kParamDetachable] != 0);
}

ChunkyBoneGeometry::BoneType ChunkyBoneGeometry::GetBoneType() const {
	deb_assert(body_data_.bone_type_ >= kBoneBody && body_data_.bone_type_ <= kBonePosition);
	return (body_data_.bone_type_);
}

PhysicsManager::JointID ChunkyBoneGeometry::GetJointId() const {
	return (joint_id_);
}

void ChunkyBoneGeometry::ResetJointId() {
	joint_id_ = INVALID_JOINT;
}

void ChunkyBoneGeometry::SetJointId(PhysicsManager::JointID joint_id) {
	joint_id_ = joint_id;
}

PhysicsManager::BodyID ChunkyBoneGeometry::GetBodyId() const {
	return (body_id_);
}

void ChunkyBoneGeometry::ResetBodyId() {
	body_id_ = INVALID_BODY;
}

bool ChunkyBoneGeometry::IsConnectorType(ConnectorType type) const {
	return (std::find(connector_array_.begin(), connector_array_.end(), type) != connector_array_.end());
}

void ChunkyBoneGeometry::AddConnectorType(ConnectorType type) {
	if (type != kConnectNone) {
		connector_array_.push_back(type);
	}
}

void ChunkyBoneGeometry::ClearConnectorTypes() {
	connector_array_.clear();
}

vec3 ChunkyBoneGeometry::GetOriginalOffset() const {
	return vec3(body_data_.parameter_[kParamOffsetX], body_data_.parameter_[kParamOffsetY], body_data_.parameter_[kParamOffsetZ]);
}

float ChunkyBoneGeometry::GetImpactFactor() const {
	return body_data_.parameter_[kParamImpactFactor];
}

const str& ChunkyBoneGeometry::GetMaterial() const {
	return material_;
}

void ChunkyBoneGeometry::SetMaterial(const str& material) {
	material_ = material;
}



float ChunkyBoneGeometry::GetExtraData() const {
	return (extra_data_);
}

void ChunkyBoneGeometry::SetExtraData(float extra_data) {
	extra_data_ = extra_data;
}

ChunkyBoneGeometry::BodyDataBase& ChunkyBoneGeometry::GetBodyData() {
	return body_data_;
}



unsigned ChunkyBoneGeometry::GetChunkySize(const void* data) const {
	unsigned __size = (unsigned)(sizeof(int32)*8 + sizeof(body_data_.parameter_) +
		sizeof(int32) + sizeof(int32)*connector_array_.size());

	if (data && connector_array_.empty() && material_.empty()) {	// Shouldn't go here if we have something in RAM already.
		const uint32* _data = (const uint32*)data;
		unsigned x = 8 + LEPRA_ARRAY_COUNT(body_data_.parameter_);
		const unsigned connector_size = Endian::BigToHost(_data[x]) * sizeof(ConnectorType);
		__size += connector_size;
		x += 1 + connector_size/sizeof(int32);
		__size += PackerUnicodeString::UnpackRaw(0, (const uint8*)&_data[x], 100);
	} else {
		__size += PackerUnicodeString::Pack(0, material_);
	}
	return (__size);
}

void ChunkyBoneGeometry::SaveChunkyData(const ChunkyPhysics* structure, void* data) const {
	uint32* _data = (uint32*)data;
	_data[0] = Endian::HostToBig(GetGeometryType());
	_data[1] = Endian::HostToBigF(body_data_.mass_);
	_data[2] = Endian::HostToBigF(body_data_.friction_);
	_data[3] = Endian::HostToBigF(body_data_.bounce_);
	_data[4] = body_data_.parent_? Endian::HostToBig(structure->GetIndex(body_data_.parent_)) : (unsigned)-1;
	_data[5] = Endian::HostToBig(body_data_.joint_type_);
	_data[6] = Endian::HostToBig(body_data_.is_affected_by_gravity_? 1 : 0);
	_data[7] = Endian::HostToBig(body_data_.bone_type_);
	int y = 8;
	for (int x = 0; (size_t)x < LEPRA_ARRAY_COUNT(body_data_.parameter_); ++x) {
		_data[y++] = Endian::HostToBigF(body_data_.parameter_[x]);
	}
	const int connector_types = (int)connector_array_.size();
	_data[y++] = Endian::HostToBig(connector_types);
	for (int x = 0; x < connector_types; ++x) {
		_data[y++] = Endian::HostToBig(connector_array_[x]);
	}
	PackerUnicodeString::Pack((uint8*)&_data[y], material_);
}

void ChunkyBoneGeometry::LoadChunkyData(ChunkyPhysics* structure, const void* data) {
	const uint32* _data = (const uint32*)data;

	deb_assert((GeometryType)Endian::BigToHost(_data[0]) == GetGeometryType());
	body_data_.mass_ = Endian::BigToHostF(_data[1]);
	body_data_.friction_ = Endian::BigToHostF(_data[2]);
	body_data_.bounce_ = Endian::BigToHostF(_data[3]);
	int parent_index = Endian::BigToHost(_data[4]);
	body_data_.parent_ = (parent_index < 0)? 0 : structure->GetBoneGeometry(parent_index);
	body_data_.joint_type_ = (JointType)Endian::BigToHost(_data[5]);
	body_data_.is_affected_by_gravity_ = Endian::BigToHost(_data[6])? true : false;
	body_data_.bone_type_ = (BoneType)Endian::BigToHost(_data[7]);
	int y = 8;
	for (int x = 0; (size_t)x < LEPRA_ARRAY_COUNT(body_data_.parameter_); ++x) {
		body_data_.parameter_[x] = Endian::BigToHostF(_data[y++]);
	}
	const int connector_types = Endian::BigToHost(_data[y++]);
	for (int x = 0; x < connector_types; ++x) {
		connector_array_.push_back((ConnectorType)Endian::BigToHost(_data[y++]));
	}
	PackerUnicodeString::Unpack(material_, (uint8*)&_data[y], 100);
}



ChunkyBoneCapsule::ChunkyBoneCapsule(const BodyData& body_data):
	Parent(body_data),
	radius_(0),
	length_(0) {
}

bool ChunkyBoneCapsule::CreateBody(PhysicsManager* physics, bool is_root,
	int force_listener_id, PhysicsManager::BodyType type,
	const xform& transform) {
	RemovePhysics(physics);
	body_id_ = physics->CreateCapsule(is_root, transform, body_data_.mass_, radius_, length_, type,
		body_data_.friction_, body_data_.bounce_, force_listener_id, body_data_.bone_type_==kBoneTrigger);
	return (body_id_ != INVALID_BODY);
}

unsigned ChunkyBoneCapsule::GetChunkySize(const void* data) const {
	return (Parent::GetChunkySize(data) + sizeof(float32)*2);
}

void ChunkyBoneCapsule::SaveChunkyData(const ChunkyPhysics* structure, void* data) const {
	Parent::SaveChunkyData(structure, data);

	uint32* _data = (uint32*)&((const char*)data)[Parent::GetChunkySize()];
	_data[0] = Endian::HostToBigF(radius_);
	_data[1] = Endian::HostToBigF(length_);
}

vec3 ChunkyBoneCapsule::GetShapeSize() const {
	return (vec3(radius_*2, radius_*2, length_+radius_*2));
}

void ChunkyBoneCapsule::LoadChunkyData(ChunkyPhysics* structure, const void* data) {
	Parent::LoadChunkyData(structure, data);

	const uint32* _data = (const uint32*)&((const char*)data)[Parent::GetChunkySize()];
	radius_ = Endian::BigToHostF(_data[0]);
	length_ = Endian::BigToHostF(_data[1]);
}

ChunkyBoneGeometry::GeometryType ChunkyBoneCapsule::GetGeometryType() const {
	return kGeometryCapsule;
}



ChunkyBoneCylinder::ChunkyBoneCylinder(const BodyData& body_data):
	Parent(body_data) {
}

bool ChunkyBoneCylinder::CreateBody(PhysicsManager* physics, bool is_root,
	int force_listener_id, PhysicsManager::BodyType type,
	const xform& transform) {
	RemovePhysics(physics);
	body_id_ = physics->CreateCylinder(is_root, transform, body_data_.mass_, radius_, length_, type,
		body_data_.friction_, body_data_.bounce_, force_listener_id, body_data_.bone_type_==kBoneTrigger);
	return (body_id_ != INVALID_BODY);
}

vec3 ChunkyBoneCylinder::GetShapeSize() const {
	return (vec3(radius_*2, radius_*2, length_));
}

ChunkyBoneGeometry::GeometryType ChunkyBoneCylinder::GetGeometryType() const {
	return kGeometryCylinder;
}



ChunkyBoneSphere::ChunkyBoneSphere(const BodyData& body_data):
	Parent(body_data),
	radius_(0) {
}

bool ChunkyBoneSphere::CreateBody(PhysicsManager* physics, bool is_root,
	int force_listener_id, PhysicsManager::BodyType type,
	const xform& transform) {
	RemovePhysics(physics);
	body_id_ = physics->CreateSphere(is_root, transform, body_data_.mass_, radius_, type, body_data_.friction_,
		body_data_.bounce_, force_listener_id, body_data_.bone_type_==kBoneTrigger);
	return (body_id_ != INVALID_BODY);
}

unsigned ChunkyBoneSphere::GetChunkySize(const void* data) const {
	return (Parent::GetChunkySize(data) + sizeof(float32));
}

void ChunkyBoneSphere::SaveChunkyData(const ChunkyPhysics* structure, void* data) const {
	Parent::SaveChunkyData(structure, data);

	uint32* _data = (uint32*)&((const char*)data)[Parent::GetChunkySize()];
	_data[0] = Endian::HostToBigF(radius_);
}

vec3 ChunkyBoneSphere::GetShapeSize() const {
	return (vec3(radius_*2, radius_*2, radius_*2));
}

void ChunkyBoneSphere::LoadChunkyData(ChunkyPhysics* structure, const void* data) {
	Parent::LoadChunkyData(structure, data);

	const uint32* _data = (const uint32*)&((const char*)data)[Parent::GetChunkySize()];
	radius_ = Endian::BigToHostF(_data[0]);
}

ChunkyBoneGeometry::GeometryType ChunkyBoneSphere::GetGeometryType() const {
	return (kGeometrySphere);
}



ChunkyBoneBox::ChunkyBoneBox(const BodyData& body_data):
	Parent(body_data) {
}

bool ChunkyBoneBox::CreateBody(PhysicsManager* physics, bool is_root,
	int force_listener_id, PhysicsManager::BodyType type,
	const xform& transform) {
	RemovePhysics(physics);
	body_id_ = physics->CreateBox(is_root, transform, body_data_.mass_, size_, type, body_data_.friction_,
		body_data_.bounce_, force_listener_id, body_data_.bone_type_==kBoneTrigger);
	return (body_id_ != INVALID_BODY);
}

unsigned ChunkyBoneBox::GetChunkySize(const void* data) const {
	return (Parent::GetChunkySize(data) + sizeof(float32)*3);
}

void ChunkyBoneBox::SaveChunkyData(const ChunkyPhysics* structure, void* data) const {
	Parent::SaveChunkyData(structure, data);

	uint32* _data = (uint32*)&((const char*)data)[Parent::GetChunkySize()];
	_data[0] = Endian::HostToBigF(size_.x);
	_data[1] = Endian::HostToBigF(size_.y);
	_data[2] = Endian::HostToBigF(size_.z);
}

vec3 ChunkyBoneBox::GetShapeSize() const {
	return (size_);
}

void ChunkyBoneBox::LoadChunkyData(ChunkyPhysics* structure, const void* data) {
	Parent::LoadChunkyData(structure, data);

	const uint32* _data = (const uint32*)&((const char*)data)[Parent::GetChunkySize()];
	size_.x = Endian::BigToHostF(_data[0]);
	size_.y = Endian::BigToHostF(_data[1]);
	size_.z = Endian::BigToHostF(_data[2]);
}

ChunkyBoneGeometry::GeometryType ChunkyBoneBox::GetGeometryType() const {
	return (kGeometryBox);
}



ChunkyBoneMesh::ChunkyBoneMesh(const BodyData& body_data):
	Parent(body_data),
	vertex_count_(0),
	vertices_(0),
	triangle_count_(0),
	indices_(0) {
}

ChunkyBoneMesh::~ChunkyBoneMesh() {
	Clear();
}

bool ChunkyBoneMesh::CreateBody(PhysicsManager* physics, bool is_root,
	int force_listener_id, PhysicsManager::BodyType type,
	const xform& transform) {
	RemovePhysics(physics);
	deb_assert(triangle_count_ > 1);
	body_id_ = physics->CreateTriMesh(is_root, vertex_count_, vertices_, triangle_count_, indices_,
		transform, body_data_.mass_, type, body_data_.friction_, body_data_.bounce_, force_listener_id,
		body_data_.bone_type_==kBoneTrigger);
	return (body_id_ != INVALID_BODY);
}

unsigned ChunkyBoneMesh::GetChunkySize(const void* data) const {
	unsigned __size = Parent::GetChunkySize(data);
	uint32 vertex_count;
	uint32 triangle_count;
	if (vertex_count_) {	// Checking size when already loaded?
		vertex_count = vertex_count_;
		triangle_count = triangle_count_;
	} else {
		const uint32* _data = (const uint32*)&((const uint8*)data)[__size];
		vertex_count = Endian::BigToHost(_data[0]);
		triangle_count = Endian::BigToHost(_data[1]);
	}
	__size += sizeof(vertex_count_)*2 + vertex_count*sizeof(vertices_[0])*3 +
		triangle_count*sizeof(indices_[0])*3;
	return (__size);
}

void ChunkyBoneMesh::SaveChunkyData(const ChunkyPhysics* structure, void* data) const {
	Parent::SaveChunkyData(structure, data);

	uint32* _data = (uint32*)&((char*)data)[Parent::GetChunkySize()];
	_data[0] = Endian::HostToBig(vertex_count_);
	_data[1] = Endian::HostToBig(triangle_count_);
	uint32 base = 2;
	uint32 x;
	for (x = 0; x < vertex_count_*3; ++x) {
		_data[base+x] = Endian::HostToBigF(vertices_[x]);
	}
	base += x;
	for (x = 0; x < triangle_count_*3; ++x) {
		_data[base+x] = Endian::HostToBig(indices_[x]);
	}
}

vec3 ChunkyBoneMesh::GetShapeSize() const {
	return (vec3(10,10,10));	// Implement if you want to be able to debug mesh EXTENTS. Doesn't seem very interesting...
}

void ChunkyBoneMesh::LoadChunkyData(ChunkyPhysics* structure, const void* data) {
	Parent::LoadChunkyData(structure, data);

	const uint32* _data = (const uint32*)&((const char*)data)[Parent::GetChunkySize()];
	vertex_count_ = Endian::BigToHost(_data[0]);
	triangle_count_ = Endian::BigToHost(_data[1]);
	deb_assert(!vertices_ && !indices_);
	vertices_ = new float[vertex_count_*3];
	indices_ = new uint32[triangle_count_*3];
	uint32 base = 2;
	uint32 x;
	for (x = 0; x < vertex_count_*3; ++x) {
		vertices_[x] = Endian::BigToHostF(_data[base+x]);
	}
	base += x;
	for (x = 0; x < triangle_count_*3; ++x) {
		indices_[x] = Endian::BigToHost(_data[base+x]);
	}
}

void ChunkyBoneMesh::Clear() {
	vertex_count_ = 0;
	delete (vertices_);
	vertices_ = 0;
	triangle_count_ = 0;
	delete (indices_);
	indices_ = 0;
}

ChunkyBoneGeometry::GeometryType ChunkyBoneMesh::GetGeometryType() const {
	return (kGeometryMesh);
}

void ChunkyBoneMesh::RelocatePointers(const ChunkyPhysics* target, const ChunkyPhysics* source, const ChunkyBoneGeometry& original) {
	Parent::RelocatePointers(target, source, original);

	const float* original_vertices = vertices_;
	const uint32* original_indices = indices_;
	vertices_ = new float[vertex_count_*3];
	indices_ = new uint32[triangle_count_*3];
	::memcpy(vertices_, original_vertices, vertex_count_*3*sizeof(vertices_[0]));
	::memcpy(indices_, original_indices, triangle_count_*3*sizeof(indices_[0]));
}



loginstance(kPhysics, ChunkyBoneGeometry);



}
