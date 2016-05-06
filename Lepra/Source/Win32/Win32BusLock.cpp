
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/win32/win32target.h"
#include "../../include/buslock.h"



namespace lepra {



bool BusLock::CompareAndSwap(int32* destination, int32 source, int32 comperand) {
	bool result = false;
	__asm {
		mov		ebx, destination	// Load dest address into scratch register ebx.
		mov		ecx, source		// Load source into scratch register ecx.
		mov		eax, comperand		// Load comperand into scratch eax.
		lock cmpxchg	[ebx], ecx		// [ebx] = ecx if [ebx] == eax. Sets ZF.
		sete		result			// Set result = 1 if ZF = 1.
	};
	return result;
//	return (::InterlockedCompareExchange((volatile LONG*)&dest, src, comperand) == comperand);
}

void BusLock::Add(int32* destination, int32 addend) {
	::InterlockedExchangeAdd((volatile long*)destination, addend);
}

void BusLock::Add(int64* destination, int64 addend) {
	// TODO: implement correctly!
	//::InterlockedExchangeAdd64(destination, addend); <- check how this would be implemented in kernel32.dll (my machine does probably not support an IPF-compliant CPU).
	*destination += addend;
}



}
