
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "life.h"



namespace life {



class SystemUtil {
public:
	static void SaveRtvar(cure::RuntimeVariableScope* scope, const str& rtvar_name);
	static void LoadRtvar(cure::RuntimeVariableScope* scope, const str& rtvar_name);
};



}
