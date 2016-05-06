
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/chunkyclass.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/endian.h"
#include "../../lepra/include/hashutil.h"
#include "../../lepra/include/packer.h"



namespace tbc {



ChunkyClass::ChunkyClass() {
}

ChunkyClass::~ChunkyClass() {
}



const str& ChunkyClass::GetPhysicsBaseName() const {
	return (physics_base_name_);
}

str& ChunkyClass::GetPhysicsBaseName() {
	return (physics_base_name_);
}



bool ChunkyClass::UnpackTag(uint8* buffer, unsigned _size) {
	bool ok = true;
	int index = 0;
	Tag _tag;
	_tag.tag_name_ = "<unknown>";
	if (ok) {
		int str_size = PackerUnicodeString::Unpack(_tag.tag_name_, &buffer[index], _size-index);
		str_size = (str_size+3)&(~3);
		ok = (index+str_size < (int)_size);
		deb_assert(ok);
		index += str_size;
	}
	if (ok) {
		const int32 float_value_count = Endian::BigToHost(*(int32*)&buffer[index]);
		index += sizeof(float_value_count);
		for (int x = 0; x < float_value_count; ++x) {
			const float value = Endian::BigToHostF(*(uint32*)&buffer[index]);
			index += sizeof(value);
			_tag.float_value_list_.push_back(value);
		}
	}
	if (ok) {
		const int32 string_value_count = Endian::BigToHost(*(int32*)&buffer[index]);
		index += sizeof(string_value_count);
		for (int x = 0; ok && x < string_value_count; ++x) {
			str value;
			int str_size = PackerUnicodeString::Unpack(value, &buffer[index], _size-index);
			str_size = (str_size+3)&(~3);
			ok = (index+str_size < (int)_size);
			deb_assert(ok);
			if (!ok) {
				log_.Errorf("String index %i had wrong length (%i).", x, str_size);
			}
			index += str_size;
			_tag.string_value_list_.push_back(value);
		}
	}
	if (ok) {
		const int32 body_index_count = Endian::BigToHost(*(int32*)&buffer[index]);
		index += sizeof(body_index_count);
		for (int x = 0; x < body_index_count; ++x) {
			const int32 body_index = Endian::BigToHost(*(int32*)&buffer[index]);
			deb_assert(body_index >= 0 && body_index < 200);
			index += sizeof(body_index);
			_tag.body_index_list_.push_back(body_index);
		}
	}
	if (ok) {
		const int32 engine_index_count = Endian::BigToHost(*(int32*)&buffer[index]);
		index += sizeof(engine_index_count);
		for (int x = 0; x < engine_index_count; ++x) {
			const int32 engine_index = Endian::BigToHost(*(int32*)&buffer[index]);
			deb_assert(engine_index >= 0 && engine_index < 20);
			index += sizeof(engine_index);
			_tag.engine_index_list_.push_back(engine_index);
		}
	}
	if (ok) {
		const int32 mesh_index_count = Endian::BigToHost(*(int32*)&buffer[index]);
		index += sizeof(mesh_index_count);
		for (int x = 0; x < mesh_index_count; ++x) {
			const int32 mesh_index = Endian::BigToHost(*(int32*)&buffer[index]);
			//deb_assert(mesh_index >= 0 && mesh_index < (int32)GetMeshCount());
			index += sizeof(mesh_index);
			_tag.mesh_index_list_.push_back(mesh_index);
		}
	}
	ok = (index == (int)_size);
	deb_assert(ok);
	if (ok) {
		AddTag(_tag);
	} else {
		log_.Errorf("File error: could not unpack class tag of type %s.", _tag.tag_name_.c_str());
	}
	return (ok);
}

void ChunkyClass::AddTag(const Tag& tag) {
	tag_array_.push_back(tag);
}

void ChunkyClass::RemoveTag(size_t tag_index) {
	if (tag_index >= GetTagCount()) {
		deb_assert(false);
		return;
	}
	tag_array_.erase(tag_array_.begin()+tag_index);
}

size_t ChunkyClass::GetTagCount() const {
	return (tag_array_.size());
}

const ChunkyClass::Tag& ChunkyClass::GetTag(size_t tag_index) const {
	deb_assert(tag_index < GetTagCount());
	return (tag_array_[tag_index]);
}

const ChunkyClass::Tag* ChunkyClass::GetTag(const str& tag_name) const {
	for (size_t x = 0, n = GetTagCount(); x < n; ++x) {
		const ChunkyClass::Tag& _tag = tag_array_[x];
		if (_tag.tag_name_ == tag_name) {
			return &_tag;
		}
	}
	return 0;
}



void ChunkyClass::AddPhysRoot(int phys_index) {
	phys_root_set_.insert(phys_index);
}

bool ChunkyClass::IsPhysRoot(int phys_index) const {
	return (HashUtil::FindSetObjectDefault(phys_root_set_, phys_index, -1) == phys_index);
}



loginstance(kGameContext, ChunkyClass);



}
