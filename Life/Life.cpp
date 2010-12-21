
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Life.h"
#include <assert.h>
#include "../Cure/Include/ContextObjectAttribute.h"
#include "RaceScore.h"



namespace Life
{



void InitLife()
{
	Cure::ContextObjectAttribute::SetCreator(&CreateObjectAttribute);
}

Cure::ContextObjectAttribute* CreateObjectAttribute(Cure::ContextObject* pObject, const str& pAttributeName)
{
	if (strutil::StartsWith(pAttributeName, _T("race_timer_")))
	{
		return new RaceScore(pObject, pAttributeName);
	}
	assert(false);
	return 0;
}



}
