
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "Thread.h"



#if defined(LEPRA_WINDOWS)

#include "Win32/Win32Thread.h"
#define Lock			Win32Lock
#define CompatibleLock		Win32CompatibleLock
#define Condition		Win32Condition
#define CompatibleCondition	Win32CompatibleCondition
#define Semaphore		Win32Semaphore
//#define CompatibleSemaphore	Win32Semaphore
#define RWLock			Win32RWLock

#elif defined(LEPRA_POSIX)

#include "Posix/PosixThread.h"
#define Lock			PosixLock
#define CompatibleLock		PosixLock
#define Condition		PosixCondition
#define CompatibleCondition	PosixCondition
#define Semaphore		PosixSemaphore
//#define CompatibleSemaphore	PosixSemaphore
#define RWLock			Win32RWLock

#else

#error "Not implemented for target system!"

#endif // LEPRA_WINDOWS/LEPRA_POSIX
