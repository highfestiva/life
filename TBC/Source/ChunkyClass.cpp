
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



const Lepra::String& ChunkyClass::GetPhysicsBaseName() const
{
	return (mPhysicsBaseName);
}

Lepra::String& ChunkyClass::GetPhysicsBaseName()
{
	return (mPhysicsBaseName);
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ChunkyClass);



}
