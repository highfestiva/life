// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/tbc.h"
#include <ode/ode.h>



namespace tbc {



void Init() {
	::dInitODE2(0);
}

void Shutdown() {
	::dCloseODE();
}



}
