
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ChunkyClass.h"



namespace TBC
{



ChunkyClass::ChunkyClass()
{
}

ChunkyClass::~ChunkyClass()
{
}



void ChunkyClass::SetPhysicsBaseName(const Lepra::String& pBaseName)
{
	mPhysicsBaseName = pBaseName;
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ChunkyClass);



}
