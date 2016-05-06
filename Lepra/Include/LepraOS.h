
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#pragma once
#include "lepratarget.h"



#if defined(LEPRA_WINDOWS)
#include "win32/win32target.h"
#elif defined(LEPRA_POSIX)
#include "posix/posixtarget.h"
#else
#error "No recognized target platform specified!"
#endif // LEPRA_WINDOWS/LEPRA_POSIX/?
