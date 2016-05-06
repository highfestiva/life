
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once
#include "lepratarget.h"



#ifdef LEPRA_DEBUG

#include <assert.h>
#define deb_assert(x) assert(x)

#else // Release

#define deb_assert(x)

#endif // Debug / release
