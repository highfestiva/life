
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/ContextObjectAttribute.h"



namespace Cure
{



ContextObjectAttribute::ContextObjectAttribute(ContextObject* pContextObject):
	mContextObject(pContextObject)
{
}

ContextObjectAttribute::~ContextObjectAttribute()
{
	mContextObject = 0;
}



ContextObject* ContextObjectAttribute::GetContextObject() const
{
	return (mContextObject);
}



}
