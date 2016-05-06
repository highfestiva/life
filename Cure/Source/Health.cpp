
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/health.h"
#include "../include/contextobject.h"
#include "../include/floatattribute.h"

#define kHealthName	"float_health"



namespace cure {



FloatAttribute* Health::GetAttribute(const ContextObject* object) {
	return (FloatAttribute*)object->GetAttribute(kHealthName);
}

float Health::Get(const ContextObject* object, float _default) {
	FloatAttribute* health = GetAttribute(object);
	if (health) {
		return health->GetValue();
	}
	return _default;
}

float Health::Add(ContextObject* object, float delta, bool low_limit) {
	float _value = Get(object) + delta;
	if (low_limit) {
		_value = std::max(0.0f, _value);
	}
	Set(object, _value);
	return _value;
}

void Health::Set(ContextObject* object, float value) {
	FloatAttribute* health = GetAttribute(object);
	if (health) {
		health->SetValue(value);
	} else {
		new FloatAttribute(object, kHealthName, value);
	}
}

void Health::DeleteAttribute(ContextObject* object) {
	object->DeleteAttribute(kHealthName);
}



}
