
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0502
#endif // _WIN32_WINNT



#include <Windows.h>
#include "../../Include/BusLock.h"



namespace Lepra
{



bool BusLock::CompareAndSwap(long* pDestination, long pSource, long pComperand)
{
	bool lResult = false;
	__asm
	{
		mov		ebx, pDestination	// Load dest address into scratch register ebx.
		mov		ecx, pSource		// Load source into scratch register ecx.
		mov		eax, pComperand		// Load comperand into scratch eax.
		lock cmpxchg	[ebx], ecx		// [ebx] = ecx if [ebx] == eax. Sets ZF.
		sete		lResult			// Set lResult = 1 if ZF = 1.
	};
	return lResult;
//	return (::InterlockedCompareExchange((volatile LONG*)&pDest, pSrc, pComperand) == pComperand);
}

void BusLock::Add(int32* pDestination, int32 pAddend)
{
	::InterlockedExchangeAdd((volatile long*)pDestination, pAddend);
}

void BusLock::Add(int64* pDestination, int64 pAddend)
{
	// TODO: implement correctly!
	//::InterlockedExchangeAdd64(pDestination, pAddend); <- check how this would be implemented in kernel32.dll (my machine does probably not support an IPF-compliant CPU).
	pDestination += pAddend;
}



}
