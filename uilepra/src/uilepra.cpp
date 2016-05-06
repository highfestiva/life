
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uicore.h"
#include "../include/uiinput.h"
#include "../include/uilepra.h"
#include "../include/uisoundmanager.h"



namespace uilepra {



void Init() {
	lepra::Init();
	Core::Init();
}

void Shutdown() {
	Core::Shutdown();
	lepra::Shutdown();
}



}
