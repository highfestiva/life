
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/LepraTypes.h"
#include "../../Lepra/Include/Log.h"



namespace Cure
{
class RuntimeVariableScope;
}



namespace UiCure
{



using namespace Lepra;

void Init();
void Shutdown();
void SetDefault(Cure::RuntimeVariableScope* pSettings);
Cure::RuntimeVariableScope* GetSettings();



}
