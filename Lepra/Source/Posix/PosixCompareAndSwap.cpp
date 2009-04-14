/*
	File:	PosixCompareAndSwap.cpp
	Author:	Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/



#include <pthread.h>
#include "../../Include/Lepra.h"



namespace Lepra
{



#ifndef LEPRA_GCC_X86
pthread_mutex_t g_CASMutex = PTHREAD_MUTEX_INITIALIZER;
#endif // !LEPRA_GCC_X86



bool CompareAndSwap(long* pDest, long pSrc, long pComperand)
{
	bool lResult = false;
#ifdef LEPRA_GCC_X86
	// Optimized with X86 assembler.
	__asm __volatile__
	(
		// TODO: This code is not compiled nor tested yet, so expect bugs.
		"movl		pDest,%ebx		\n\t"	// Load address of destination into scratch ebx.
		"movl		pSrc,%ecx		\n\t"	// Load source into scratch register ecx.
		"movl		pComperand,%eax	\n\t"	// Load comperand into scratch eax.			
		"lock cmpxchg	%ecx,(%ebx)		\n\t"	// Exchange %%ecx with [%%ebx] if %%eax == [%%ebx].
		"sete		lResult		\n\t"	// Store result.
	);
#else // !LEPRA_GCC_X86
	pthread_mutex_lock(&g_CASMutex);
	if (pDest == pComperand)
	{
		pDest = pSrc;
		lResult = true;
	}
	pthread_mutex_unlock(&g_CASMutex);
#endif // LEPRA_GCC_X86/!LEPRA_GCC_X86
	return (lResult);
}



}
