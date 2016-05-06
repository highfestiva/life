
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratarget.h"
#ifdef LEPRA_IOS
#include <Availability.h>
#endif // iOS


#if defined(LEPRA_IOS) && (__IPHONE_OS_VERSION_MIN_REQUIRED < 70000) // Compiling for iOS 4.3-6.1 = C++03.

#include <tr1/unordered_map>
#include <tr1/unordered_set>
namespace std {
using namespace tr1;
}

#else // Compiling for iOS 7.0+ = supporting C++11.

#include <unordered_map>
#include <unordered_set>

#endif // C++03 / C++11
