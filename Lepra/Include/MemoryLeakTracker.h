
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "LepraTarget.h"
#include <stddef.h>

//#define MEMLEAK_DETECT



#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS) && defined(MEMLEAK_DETECT)

void* operator new(size_t pSize, const char* pFileName, int pLine);
void operator delete(void* pPointer, const char* pFileName, int pLine);
//void* operator new(size_t pSize);
void operator delete(void* pPointer);
void operator delete[](void* pPointer);

#define new new(__FILE__, __LINE__)

#endif // <Memleak detect>
