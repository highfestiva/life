
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Life.h"
#include "../Cure/Include/FloatAttribute.h"
#include "../Cure/Include/IntAttribute.h"
#include "../Lepra/Include/LepraAssert.h"



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
	deb_assert(false);
	return 0;
}



}
