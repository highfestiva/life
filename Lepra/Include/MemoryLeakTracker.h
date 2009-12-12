
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "LepraTarget.h"
#include <stddef.h>

//#define MEMLEAK_DETECT

#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS) && defined(MEMLEAK_DETECT)

#include "SpinLock.h"
#include <crtdbg.h>

void* operator new(size_t pSize, const char* pFileName, int pLine);
void operator delete(void* pPointer, const char* pFileName, int pLine);
//void* operator new(size_t pSize);
void operator delete(void* pPointer);
void operator delete[](void* pPointer);

#define new new(__FILE__, __LINE__)

namespace Lepra
{

class MemoryLeakTracker
{
public:

	friend class MemoryLeakDetector;

	static void AddTrack(void* pAddr, unsigned long pSize, const char* pFName, unsigned long pNum);
	static bool RemoveTrack(void* pAddr);
	static void DumpLeaks();

	static int smMaxAllocated;
	static int smCurrentlyAllocated;
	static SpinLock smSpinLock;
	static bool lLeaksDumped;
};

} // End namespace.

#else // !(defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS) && defined(MEMLEAK_DETECT))

//#ifdef LEPRA_WINDOWS
//void* operator new(size_t pSize);
//void operator delete(void* pPointer);
//void operator delete[](void* pPointer);
//#endif // defined(LEPRA_WINDOWS)

#endif // <Memleak detection>/!<Memleak detection>
