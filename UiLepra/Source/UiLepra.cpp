
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
