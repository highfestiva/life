
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "Life.h"



namespace Life
{



class SystemUtil
{
public:
	static void SaveRtvar(Cure::RuntimeVariableScope* pScope, const str& pRtvarName);
	static void LoadRtvar(Cure::RuntimeVariableScope* pScope, const str& pRtvarName);
};



}
