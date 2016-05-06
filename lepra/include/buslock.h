
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"



namespace lepra {



class BusLock {
public:
	// Set destination = source if destination == comperand.
	// Returns true if a swap was performed.
	static bool CompareAndSwap(int32* destination, int32 source, int32 comperand);

	static void Add(int32* destination, int32 addend);
	static void Add(int64* destination, int64 addend);
};



}
