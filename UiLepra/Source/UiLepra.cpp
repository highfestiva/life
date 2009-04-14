
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/UiCore.h"
#include "../Include/UiInput.h"
#include "../Include/UiLepra.h"
#include "../Include/UiSoundManager.h"



namespace UiLepra
{



void Init()
{
	Lepra::Init();
	Core::Init();
}

void Shutdown()
{
	Core::Shutdown();
	Lepra::Shutdown();
}



}
