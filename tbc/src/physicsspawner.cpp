
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/physicsspawner.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/endian.h"
#include "../../lepra/include/packer.h"
#include "../include/chunkybonegeometry.h"
#include "../include/chunkyphysics.h"



namespace tbc {



PhysicsSpawner::PhysicsSpawner():
	spawner_type_(kSpawnerInvalid),
	number_(0),
	is_ease_down_(false),
	total_object_probability_(1) {
}

PhysicsSpawner::~PhysicsSpawner() {
}

void PhysicsSpawner::RelocatePointers(const ChunkyPhysics* target, const ChunkyPhysics* source, const PhysicsSpawner& original) {
	const int count = original.GetSpawnPointCount();
	spawner_node_array_.clear();
	for (int x = 0; x < count; ++x) {
		const int bone_index = source->GetIndex(original.spawner_node_array_[x]);
		deb_assert(bone_index >= 0);
		spawner_node_array_.push_back(target->GetBoneGeometry(bone_index));
	}
}



PhysicsSpawner* PhysicsSpawner::Load(ChunkyPhysics* structure, const void* data, unsigned byte_count) {
	if (byte_count < sizeof(uint32)*9 /*+ ...*/) {
		log_.Error("Could not load; wrong data size.");
		deb_assert(false);
		return (0);
	}

	PhysicsSpawner* spawner = new PhysicsSpawner;
	spawner->LoadChunkyData(structure, data);
	if (spawner->GetChunkySize() != byte_count) {
		log_.Error("Corrupt data or error in loading algo.");
		deb_assert(false);
		delete (spawner);
		spawner = 0;
	}
	return spawner;
}



PhysicsSpawner::Type PhysicsSpawner::GetType() const {
	return (spawner_type_);
}

const str& PhysicsSpawner::GetFunction() const {
	return (function_);
}

int PhysicsSpawner::GetSpawnPointCount() const {
	return spawner_node_array_.size();
}

xform PhysicsSpawner::GetSpawnPoint(const ChunkyPhysics* structure, const vec3& scaled_point, int index, vec3& initial_velocity) const {
	initial_velocity = initial_velocity_;

	deb_assert((size_t)index < spawner_node_array_.size() && index >= 0);
	tbc::ChunkyBoneGeometry* spawn_geometry = spawner_node_array_[index];
	vec3 point = spawn_geometry->GetShapeSize();
	point.x = (scaled_point.x-0.5f) * point.x;
	point.y = (scaled_point.y-0.5f) * point.y;
	point.z = (scaled_point.z-0.5f) * point.z;
	const xform& transformation = structure->GetTransformation(spawn_geometry);
	quat q = transformation.GetOrientation();
	point = q * point;
	q.RotateAroundOwnY(PIF);	// This is so since the level ("the spawner") has a 180 degree orientation offset compared to objects.
	return xform(q, transformation.GetPosition()+point);
}

float PhysicsSpawner::GetNumber() const {
	return number_;
}

const PhysicsSpawner::IntervalArray& PhysicsSpawner::GetIntervals() const {
	return interval_array_;
}

const str PhysicsSpawner::GetSpawnObject(float probability_threshold) const {
	probability_threshold *= total_object_probability_;
	SpawnObjectArray::const_iterator x = spawn_object_array_.begin();
	for (; x != spawn_object_array_.end(); ++x) {
		if (probability_threshold <= x->probability_) {
			return x->spawn_object_;
		}
		probability_threshold -= x->probability_;
	}
	return str();
}

bool PhysicsSpawner::IsEaseDown() const {
	return is_ease_down_;
}



unsigned PhysicsSpawner::GetChunkySize() const {
	size_t string_size = PackerUnicodeString::Pack(0, function_);
	SpawnObjectArray::const_iterator x = spawn_object_array_.begin();
	for (; x != spawn_object_array_.end(); ++x) {
		string_size += PackerUnicodeString::Pack(0, x->spawn_object_);
	}
	return ((unsigned)(sizeof(uint32) * 4 +
		sizeof(uint32) * spawner_node_array_.size() +
		sizeof(float) * 3 +
		sizeof(float) +
		sizeof(float) * interval_array_.size() +
		sizeof(float) * spawn_object_array_.size() +
		sizeof(uint32) +	// bool IsEaseDown
		string_size));
}

void PhysicsSpawner::SaveChunkyData(const ChunkyPhysics* structure, void* data) const {
	uint32* _data = (uint32*)data;
	int i = 0;
	_data[i++] = Endian::HostToBig(spawner_type_);
	i += PackerUnicodeString::Pack((uint8*)&_data[i], function_);
	_data[i++] = Endian::HostToBig((uint32)spawner_node_array_.size());
	for (int z = 0; (size_t)z < spawner_node_array_.size(); ++z) {
		_data[i++] = Endian::HostToBig(structure->GetIndex(spawner_node_array_[z]));
	}
	_data[i++] = Endian::HostToBigF(number_);
	_data[i++] = Endian::HostToBig((uint32)interval_array_.size());
	IntervalArray::const_iterator y = interval_array_.begin();
	for (; y != interval_array_.end(); ++y) {
		_data[i++] = Endian::HostToBigF(*y);
	}
	SpawnObjectArray::const_iterator x = spawn_object_array_.begin();
	for (; x != spawn_object_array_.end(); ++x) {
		int string_raw_length = PackerUnicodeString::Pack((uint8*)&_data[i], x->spawn_object_);
		deb_assert(string_raw_length % sizeof(_data[0]) == 0);
		i += string_raw_length / sizeof(_data[0]);
		_data[i++] = Endian::HostToBigF(x->probability_);
	}
	_data[i++] = Endian::HostToBig(is_ease_down_? 1 : 0);
}

void PhysicsSpawner::LoadChunkyData(ChunkyPhysics* structure, const void* data) {
	const uint32* _data = (const uint32*)data;

	int i = 0;
	spawner_type_ = (Type)Endian::BigToHost(_data[i++]);
	i += PackerUnicodeString::Unpack(function_, (uint8*)&_data[i], 1024) / sizeof(_data[0]);
	spawner_node_array_.clear();
	const int spawner_node_count = Endian::BigToHost(_data[i++]);
	for (int x = 0; x < spawner_node_count; ++x) {
		const int body_index = Endian::BigToHost(_data[i++]);
		spawner_node_array_.push_back(structure->GetBoneGeometry(body_index));
	}
	const float x = Endian::BigToHostF(_data[i++]);
	const float y = Endian::BigToHostF(_data[i++]);
	const float z = Endian::BigToHostF(_data[i++]);
	initial_velocity_.Set(x, y, z);
	number_ = Endian::BigToHostF(_data[i++]);
	const int interval_count = Endian::BigToHost(_data[i++]);
	for (int j = 0; j < interval_count; ++j) {
		interval_array_.push_back(Endian::BigToHostF(_data[i++]));
	}
	total_object_probability_ = 0;
	const int spawn_object_count = Endian::BigToHost(_data[i++]);
	for (int j = 0; j < spawn_object_count; ++j) {
		str _spawn_object;
		const int string_raw_length = PackerUnicodeString::Unpack(_spawn_object, (uint8*)&_data[i], 1024);
		deb_assert(string_raw_length % sizeof(_data[0]) == 0);
		i += string_raw_length / sizeof(_data[0]);
		const float _probability = Endian::BigToHostF(_data[i++]);
		total_object_probability_ += _probability;
		spawn_object_array_.push_back(SpawnObject(_spawn_object, _probability));
	}
	is_ease_down_ = Endian::BigToHost(_data[i++])? true : false;
}



PhysicsSpawner::SpawnObject::SpawnObject(const str& spawn_object, float probability):
	spawn_object_(spawn_object),
	probability_(probability) {
}



loginstance(kPhysics, PhysicsSpawner);



}
