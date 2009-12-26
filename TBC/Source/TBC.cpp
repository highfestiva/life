// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
