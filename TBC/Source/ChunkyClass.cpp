
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



const str& ChunkyClass::GetPhysicsBaseName() const
{
	return (mPhysicsBaseName);
}

str& ChunkyClass::GetPhysicsBaseName()
{
	return (mPhysicsBaseName);
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ChunkyClass);



}
