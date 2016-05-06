
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/intattribute.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/packer.h"
#include "../include/contextmanager.h"
#include "../include/gamemanager.h"



namespace cure {



IntAttribute::IntAttribute(ContextObject* context_object, const str& name, int value):
	Parent(context_object, name),
	is_updated_(true),
	value_(value) {
}

IntAttribute::~IntAttribute() {
}



int IntAttribute::GetValue() const {
	return value_;
}

void IntAttribute::SetValue(int value) {
	if (value_ != value) {
		value_ = value;
		is_updated_ = true;
		context_object_->OnAttributeUpdated(this);
	}
}



int IntAttribute::QuerySend() const {
	return is_updated_? Parent::QuerySend()+sizeof(value_) : 0;
}

int IntAttribute::Pack(uint8* destination) {
	const int parent_size = Parent::Pack(destination);
	destination += parent_size;
	PackerInt32::Pack(destination, value_);
	is_updated_ = false;
	return parent_size + sizeof(value_);
}

int IntAttribute::Unpack(const uint8* source, int max_size) {
	if (max_size < sizeof(value_)) {
		return -1;
	}
	PackerInt32::Unpack(value_, source, max_size);
	is_updated_ = context_object_->GetManager()->GetGameManager()->IsServer();
	return sizeof(value_);
}



void IntAttribute::operator=(const IntAttribute&) {
	deb_assert(false);
}



}
