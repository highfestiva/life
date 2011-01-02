
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Life.h"
#include <assert.h>



namespace Cure
{
class ContextObjectAttribute;
}



namespace Life
{



Cure::ContextObjectAttribute* CreateObjectAttribute(Cure::ContextObject* pObject, const str& pAttributeName)
{
	(void)pObject;
	(void)pAttributeName;
	assert(false);
	return 0;
}



}
