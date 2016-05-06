
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratarget.h"
#include <stddef.h>

//#define MEMLEAK_DETECT



#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS) && defined(MEMLEAK_DETECT)

void* operator new(size_t size, const char* file_name, int line);
void operator delete(void* pointer, const char* file_name, int line);
//void* operator new(size_t size);
void operator delete(void* pointer);
void operator delete[](void* pointer);

#define new new(__FILE__, __LINE__)

#endif // <Memleak detect>
