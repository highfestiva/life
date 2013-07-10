
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/Cure.h"



namespace Cure
{
class ContextObject;
class ContextObjectAttribute;
}



namespace Life
{



using namespace Lepra;

const int PHYSICS_FPS = CURE_STANDARD_FRAME_RATE;	// TODO: REMOVE ME!!!

Cure::ContextObjectAttribute* CreateObjectAttribute(Cure::ContextObject* pObject, const str& pAttributeName);



}
