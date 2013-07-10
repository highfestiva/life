// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/TBC.h"
#include <ode/ode.h>



namespace TBC
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
