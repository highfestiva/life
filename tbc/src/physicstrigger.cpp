
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/physicstrigger.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/endian.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/packer.h"
#include "../include/chunkybonegeometry.h"
#include "../include/chunkyphysics.h"



namespace tbc {



bool PhysicsTrigger::EngineTrigger::operator==(const EngineTrigger& other) const {
	return (engine_ == other.engine_ &&
		delay_ == other.delay_ &&
		function_ == other.function_);
}

size_t PhysicsTrigger::EngineTrigger::Hash() const {
	return ((size_t)engine_ + *(int*)&delay_ + (size_t)HashString(function_.c_str()));
}



PhysicsTrigger::PhysicsTrigger():
	trigger_type_(kTriggerInvalid),
	//mGroupIndex(-1),
	priority_(-1)
	//mTriggerNode(0)
{
}

PhysicsTrigger::~PhysicsTrigger() {
}

void PhysicsTrigger::RelocatePointers(const ChunkyPhysics* target, const ChunkyPhysics* source, const PhysicsTrigger& original) {
	size_t cnt = trigger_array_.size();
	for (size_t x = 0; x < cnt; ++x) {
		const int bone_index = source->GetIndex(original.trigger_array_[x]);
		deb_assert(bone_index >= 0);
		trigger_array_[x] = target->GetBoneGeometry(bone_index);
	}

	cnt = connection_array_.size();
	for (size_t x = 0; x < cnt; ++x) {
		const int engine_index = source->GetEngineIndex(original.connection_array_[x].engine_);
		deb_assert(engine_index >= 0);
		connection_array_[x].engine_ = target->GetEngine(engine_index);
	}
}



PhysicsTrigger* PhysicsTrigger::Load(ChunkyPhysics* structure, const void* data, unsigned byte_count) {
	if (byte_count < sizeof(uint32)*6 /*+ ...*/) {
		log_.Error("Could not load; wrong data size.");
		deb_assert(false);
		return (0);
	}

	PhysicsTrigger* trigger = new PhysicsTrigger;
	trigger->LoadChunkyData(structure, data);
	if (trigger->GetChunkySize() != byte_count) {
		log_.Error("Corrupt data or error in loading algo.");
		deb_assert(false);
		delete (trigger);
		trigger = 0;
	}
	return (trigger);
}



PhysicsTrigger::Type PhysicsTrigger::GetType() const {
	return (trigger_type_);
}

PhysicsManager::BodyID PhysicsTrigger::GetPhysicsTriggerId(int trigger_geometry_index) const {
	deb_assert((size_t)trigger_geometry_index < trigger_array_.size());
	deb_assert(trigger_array_[trigger_geometry_index]->GetBodyId() != 0);
	return trigger_array_[trigger_geometry_index]->GetBodyId();
}

/*int PhysicsTrigger::GetGroupIndex() const {
	return (mGroupIndex);
}*/

int PhysicsTrigger::GetPriority() const {
	return (priority_);
}

const str& PhysicsTrigger::GetFunction() const {
	return (function_);
}



void PhysicsTrigger::AddTriggerGeometry(ChunkyBoneGeometry* geometry) {
	trigger_array_.push_back(geometry);
}

int PhysicsTrigger::GetTriggerGeometryCount() const {
	return trigger_array_.size();
}

ChunkyBoneGeometry* PhysicsTrigger::GetTriggerGeometry(int index) const {
	deb_assert((size_t)index < trigger_array_.size());
	return trigger_array_[index];
}

void PhysicsTrigger::AddControlledEngine(PhysicsEngine* engine, float delay, str function) {
	EngineTrigger connection;
	connection.engine_ = engine;
	connection.delay_ = delay;
	connection.function_ = function;
	connection_array_.push_back(connection);
}

int PhysicsTrigger::GetControlledEngineCount() const {
	return (connection_array_.size());
}

const PhysicsTrigger::EngineTrigger& PhysicsTrigger::GetControlledEngine(int index) const {
	deb_assert((size_t)index < connection_array_.size());
	return (connection_array_[index]);
}



unsigned PhysicsTrigger::GetChunkySize() const {
	size_t string_size = PackerUnicodeString::Pack(0, function_);
	ConnectionArray::const_iterator x = connection_array_.begin();
	for (; x != connection_array_.end(); ++x) {
		string_size += PackerUnicodeString::Pack(0, x->function_);
	}
	return ((unsigned)(sizeof(uint32) * 4 +
		sizeof(uint32) * trigger_array_.size() +
		(sizeof(uint32)+sizeof(float)) * connection_array_.size() +
		string_size));
}

void PhysicsTrigger::SaveChunkyData(const ChunkyPhysics* structure, void* data) const {
	uint32* _data = (uint32*)data;
	int i = 0;
	_data[i++] = Endian::HostToBig(trigger_type_);
	i += PackerUnicodeString::Pack((uint8*)&_data[i], function_);
	//_data[i++] = Endian::HostToBig(mGroupIndex);
	_data[i++] = Endian::HostToBig(priority_);
	_data[i++] = Endian::HostToBig((uint32)trigger_array_.size());
	for (int x = 0; x < (int)trigger_array_.size(); ++x) {
		const ChunkyBoneGeometry* bone = trigger_array_[x];
		_data[i++] = Endian::HostToBig(structure->GetIndex(bone));
	}
	_data[i++] = Endian::HostToBig((uint32)connection_array_.size());
	for (int x = 0; x < (int)connection_array_.size(); ++x) {
		const EngineTrigger& connection = connection_array_[x];
		_data[i++] = Endian::HostToBig(structure->GetEngineIndex(connection.engine_));
		_data[i++] = Endian::HostToBigF(connection.delay_);
		int string_raw_length = PackerUnicodeString::Pack((uint8*)&_data[i], connection.function_);
		deb_assert(string_raw_length % sizeof(_data[0]) == 0);
		i += string_raw_length / sizeof(_data[0]);
	}
}

void PhysicsTrigger::LoadChunkyData(ChunkyPhysics* structure, const void* data) {
	const uint32* _data = (const uint32*)data;

	int i = 0;
	trigger_type_ = (Type)Endian::BigToHost(_data[i++]);
	i += PackerUnicodeString::Unpack(function_, (uint8*)&_data[i], 1024) / sizeof(_data[0]);
	//mGroupIndex = Endian::BigToHost(_data[i++]);
	priority_ = Endian::BigToHost(_data[i++]);
	const int bone_trigger_count = Endian::BigToHost(_data[i++]);
	for (int x = 0; x < bone_trigger_count; ++x) {
		const uint32 phys_trigger_index = Endian::BigToHost(_data[i++]);
		ChunkyBoneGeometry* bone_trigger = structure->GetBoneGeometry(phys_trigger_index);
		deb_assert(bone_trigger);
		AddTriggerGeometry(bone_trigger);
	}
	const int engine_count = Endian::BigToHost(_data[i++]);
	for (int x = 0; x < engine_count; ++x) {
		PhysicsEngine* _engine = structure->GetEngine(Endian::BigToHost(_data[i++]));
		deb_assert(_engine);
		float _delay = Endian::BigToHostF(_data[i++]);
		str _function;
		int string_raw_length = PackerUnicodeString::Unpack(_function, (uint8*)&_data[i], 1024);
		deb_assert(string_raw_length % sizeof(_data[0]) == 0);
		i += string_raw_length / sizeof(_data[0]);
		AddControlledEngine(_engine, _delay, _function);
	}
}



loginstance(kPhysics, PhysicsTrigger);



}
