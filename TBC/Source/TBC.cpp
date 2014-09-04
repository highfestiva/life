// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/Tbc.h"
#include <ode/ode.h>



namespace Tbc
{



void Init()
{
	::dInitODE2(0);
}

void Shutdown()
{
	::dCloseODE();
}



}
