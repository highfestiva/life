
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#pragma once
#include "LepraTarget.h"



#if defined(LEPRA_WINDOWS)
#include "Win32/Win32Target.h"
#elif defined(LEPRA_POSIX)
#include "Posix/PosixTarget.h"
#else
#error "No recognized target platform specified!"
#endif // LEPRA_WINDOWS/LEPRA_POSIX/?
