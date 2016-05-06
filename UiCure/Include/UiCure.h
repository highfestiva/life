
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/lepratypes.h"
#include "../../lepra/include/log.h"



namespace cure {
class RuntimeVariableScope;
}



namespace UiCure {



using namespace lepra;

void Init();
void Shutdown();
void SetDefault(cure::RuntimeVariableScope* settings);
cure::RuntimeVariableScope* GetSettings();



}
