
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "life.h"
#include "../cure/include/floatattribute.h"
#include "../cure/include/intattribute.h"
#include "../lepra/include/lepraassert.h"



namespace life {



cure::ContextObjectAttribute* CreateObjectAttribute(cure::ContextObject* object, const str& attribute_name) {
	if (strutil::StartsWith(attribute_name, "float_")) {
		return new cure::FloatAttribute(object, attribute_name, 0);
	} else if (strutil::StartsWith(attribute_name, "int_")) {
		return new cure::IntAttribute(object, attribute_name, 0);
	}
	(void)object;
	(void)attribute_name;
	deb_assert(false);
	return 0;
}



}
