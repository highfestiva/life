
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/contextobjectattribute.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/packer.h"
#include "../include/contextobject.h"



namespace cure {



ContextObjectAttribute::ContextObjectAttribute(ContextObject* context_object, const str& name):
	context_object_(context_object),
	name_(name),
	network_type_(kTypeServerBroadcast) {
	context_object_->AddAttribute(this);
}

ContextObjectAttribute::~ContextObjectAttribute() {
	context_object_ = 0;
}



const str& ContextObjectAttribute::GetName() const {
	return name_;
}



void ContextObjectAttribute::SetCreator(const Factory& factory) {
	factory_ = factory;
}



int ContextObjectAttribute::QuerySend() const {
	return PackerUnicodeString::Pack(0, name_);
}

int ContextObjectAttribute::Pack(uint8* destination) {
	return PackerUnicodeString::Pack(destination, name_);
}

int ContextObjectAttribute::Unpack(ContextObject* context_object, const uint8* source, int max_size) {
	str attribute_name;
	int size = PackerUnicodeString::Unpack(attribute_name, source, max_size);
	if (size < 0) {
		return -1;
	}

	ContextObjectAttribute* attribute = context_object->GetAttribute(attribute_name);
	if (!attribute) {
		deb_assert(factory_);
		attribute = factory_(context_object, attribute_name);
		if (!attribute) {
			return -1;
		}
	}
	int params_size = attribute->Unpack(&source[size], max_size-size);
	if (params_size < 0) {
		return -1;
	}
	size += params_size;
	return size;
}



ContextObjectAttribute::NetworkType ContextObjectAttribute::GetNetworkType() const {
	return network_type_;
}

void ContextObjectAttribute::SetNetworkType(NetworkType network_type) {
	network_type_ = network_type;
}



void ContextObjectAttribute::operator=(const ContextObjectAttribute&) {
	deb_assert(false);
}



ContextObjectAttribute::Factory ContextObjectAttribute::factory_ = 0;



}
