
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "LepraTarget.h"
#include "LepraTypes.h"
#include "Log.h"
#include "MemoryLeakTracker.h"



namespace Lepra
{



// The data policy can be used in setters and in constructors where pointers to 
// large amounts of data are passed.
enum DataPolicy
{
	FULL_COPY = 0, // Copy all the data.
	COPY_REFERENCE, // Reference the same data.
	TAKE_OWNERSHIP // Reference the data and delete it when done.
};

// If the data contain pointers to subdata, use these policies for that data.
enum SubDataPolicy
{
	KEEP_REFERENCE = 0, // Just keep the reference. 
	TAKE_SUBDATA_OWNERSHIP
};


// Two global functions that are responsible of initializing and cleaning up
// all global (static) instances.
void Init();
void Shutdown();



}
