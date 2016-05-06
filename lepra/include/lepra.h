
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratarget.h"
#include "lepratypes.h"
#include "log.h"
#include "memoryleaktracker.h"



namespace lepra {



// The data policy can be used in setters and in constructors where pointers to
// large amounts of data are passed.
enum DataPolicy {
	kFullCopy = 0, // Copy all the data.
	kCopyReference, // Reference the same data.
	TAKE_OWNERSHIP // Reference the data and delete it when done.
};

// If the data contain pointers to subdata, use these policies for that data.
enum SubDataPolicy {
	kKeepReference = 0, // Just keep the reference.
	kTakeSubdataOwnership
};


// Two global functions that are responsible of initializing and cleaning up
// all global (static) instances.
void Init();
void Shutdown();



}
