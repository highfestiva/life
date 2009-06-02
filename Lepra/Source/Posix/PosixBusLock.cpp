
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include <pthread.h>
#include "../../Include/BusLock.h"



namespace Lepra
{



#ifndef LEPRA_GCC_X86_32
static pthread_mutex_t g_CASMutex = PTHREAD_MUTEX_INITIALIZER;
#endif // !i386



bool BusLock::CompareAndSwap(long* pDest, long pSrc, long pComperand)
{
	bool lResult = false;
#ifdef LEPRA_GCC_X86_32
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
	pthread_mutex_lock(&g_CASMutex);
	if (*pDest == pComperand)
	{
		*pDest = pSrc;
		lResult = true;
	}
	pthread_mutex_unlock(&g_CASMutex);
#endif // i386 / other.
	return (lResult);
}



void BusLock::Add(int32* pDestination, int32 pAddend)
{
	// TODO: implement correctly!
	// On Win32 we do ::InterlockedExchangeAdd((volatile long*)pDestination, pAddend);
	*pDestination += pAddend;
}

void BusLock::Add(int64* pDestination, int64 pAddend)
{
	// TODO: implement correctly!
	//::InterlockedExchangeAdd64(pDestination, pAddend); <- check how this would be implemented in kernel32.dll (my machine does probably not support an IPF-compliant CPU).
	*pDestination += pAddend;
}



}
