
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "Thread.h"



#if defined(LEPRA_WINDOWS)

#include "Win32/Win32Thread.h"
#define FastLock	Win32Lock
#define FastCondition	Win32Condition
#define FastSemaphore	Win32Semaphore
#define FastRwLock	Win32RwLock

#elif defined(LEPRA_POSIX)

#include "Posix/PosixThread.h"
#define FastLock	PosixLock
#define FastCondition	PosixCondition
#define FastSemaphore	PosixSemaphore
#define FastRwLock	PosixRwLock

#else

#error "Not implemented for target system!"

#endif // LEPRA_WINDOWS/LEPRA_POSIX
