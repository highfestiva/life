
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "LepraTypes.h"



namespace Lepra
{



class BusLock
{
public:
	// Set pDestination = pSource if pDestination == pComperand.
	// Returns true if a swap was performed.
	static bool CompareAndSwap(int32* pDestination, int32 pSource, int32 pComperand);

	static void Add(int32* pDestination, int32 pAddend);
	static void Add(int64* pDestination, int64 pAddend);
};



}
