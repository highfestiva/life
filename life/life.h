
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/cure.h"



namespace cure {
class ContextObject;
class ContextObjectAttribute;
}



namespace life {



using namespace lepra;

const int PHYSICS_FPS = kCureStandardFrameRate;	// TODO: REMOVE ME!!!

cure::ContextObjectAttribute* CreateObjectAttribute(cure::ContextObject* object, const str& attribute_name);



}
