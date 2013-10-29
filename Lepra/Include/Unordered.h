
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#if defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && (__IPHONE_OS_VERSION_MAX_ALLOWED < 70000)

#include <tr1/unordered_map>
#include <tr1/unordered_set>
namespace std
{
using namespace tr1;
}

#else // !Old iPhone = supporting C++11

#include <unordered_map>
#include <unordered_set>

#endif // C++03 / C++11
