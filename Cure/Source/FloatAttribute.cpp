
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/floatattribute.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/packer.h"
#include "../include/contextmanager.h"
#include "../include/gamemanager.h"



namespace cure {



FloatAttribute::FloatAttribute(ContextObject* context_object, const str& name, float value):
	Parent(context_object, name),
	is_updated_(true),
	value_(value) {
}

FloatAttribute::~FloatAttribute() {
}



float FloatAttribute::GetValue() const {
	return value_;
}

void FloatAttribute::SetValue(float value) {
	if (value_ != value) {
		value_ = value;
		is_updated_ = true;
		context_object_->OnAttributeUpdated(this);
	}
}



int FloatAttribute::QuerySend() const {
	return is_updated_? Parent::QuerySend()+sizeof(value_) : 0;
}

int FloatAttribute::Pack(uint8* destination) {
	const int parent_size = Parent::Pack(destination);
	destination += parent_size;
	PackerReal::Pack(destination, value_);
	is_updated_ = false;
	return parent_size + sizeof(value_);
}

int FloatAttribute::Unpack(const uint8* source, int max_size) {
	if (max_size < sizeof(value_)) {
		return -1;
	}
	PackerReal::Unpack(value_, source, max_size);
	is_updated_ = context_object_->GetManager()->GetGameManager()->IsServer();
	return sizeof(value_);
}



void FloatAttribute::operator=(const FloatAttribute&) {
	deb_assert(false);
}



}
