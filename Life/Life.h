
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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

//#define LIFE_DEMO



Cure::ContextObjectAttribute* CreateObjectAttribute(Cure::ContextObject* pObject, const str& pAttributeName);



}
