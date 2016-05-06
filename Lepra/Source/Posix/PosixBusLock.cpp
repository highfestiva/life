
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include <pthread.h>
#include "../../include/buslock.h"



namespace lepra {



#if 1 //ndef LEPRA_GCC_X86_32
static pthread_mutex_t g_cas_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif // !i386



bool BusLock::CompareAndSwap(int32* dest, int32 src, int32 comperand) {
	bool result = false;
#if 0 //def LEPRA_GCC_X86_32
	// Optimized with i386 assembler.
	__asm __volatile__
	(
		// TODO: This code is not compiled nor tested yet, so expect bugs.
		"movl		pDest,%ebx	\n\t"	// Load address of destination into scratch ebx.
		"movl		pSrc,%ecx	\n\t"	// Load source into scratch register ecx.
		"movl		pComperand,%eax	\n\t"	// Load comperand into scratch eax.
		"lock cmpxchg	%ecx,(%ebx)	\n\t"	// Exchange %%ecx with [%%ebx] if %%eax == [%%ebx].
		"sete		lResult		\n\t"	// Store result.
	);
#else // Not i386.
	pthread_mutex_lock(&g_cas_mutex);
	if (*dest == comperand) {
		*dest = src;
		result = true;
	}
	pthread_mutex_unlock(&g_cas_mutex);
#endif // i386 / other.
	return (result);
}



void BusLock::Add(int32* destination, int32 addend) {
	// TODO: implement correctly!
	// On Win32 we do ::InterlockedExchangeAdd((volatile long*)destination, addend);
	*destination += addend;
}

void BusLock::Add(int64* destination, int64 addend) {
	// TODO: implement correctly!
	//::InterlockedExchangeAdd64(destination, addend); <- check how this would be implemented in kernel32.dll (my machine does probably not support an IPF-compliant CPU).
	*destination += addend;
}



}
