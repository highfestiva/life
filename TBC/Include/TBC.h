/*
	Lepra::File:   TBC.h
	Class:  TBC
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#define LEPRA_INCLUDE_NO_OS
#include "../../Lepra/Include/Lepra.h"
#undef LEPRA_INCLUDE_NO_OS

namespace TBC
{
// Two global functions that are responsible of initializing and cleaning up
// all global (static) instances.
void Init();
void Shutdown();
}
