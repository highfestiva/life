
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Life.h"
#include <assert.h>
#include "../Cure/Include/FloatAttribute.h"
#include "../Cure/Include/IntAttribute.h"



namespace Life
{



Cure::ContextObjectAttribute* CreateObjectAttribute(Cure::ContextObject* pObject, const str& pAttributeName)
{
	if (strutil::StartsWith(pAttributeName, _T("float_")))
	{
		return new Cure::FloatAttribute(pObject, pAttributeName, 0);
	}
	else if (strutil::StartsWith(pAttributeName, _T("int_")))
	{
		return new Cure::IntAttribute(pObject, pAttributeName, 0);
	}
	(void)pObject;
	(void)pAttributeName;
	assert(false);
	return 0;
}



}
