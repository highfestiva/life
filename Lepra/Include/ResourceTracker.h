
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <hash_map>
#include "String.h"
#include "Thread.h"



namespace Lepra
{



class ResourceTracker;



class ResourceTracker
{
public:
	typedef std::hash_map<HashedString, int> CounterMap;

	ResourceTracker();
	void Add(const HashedString& pResourceName, int pValue);
	CounterMap GetAll() const;

protected:
	Lock* mLock;
	CounterMap mCounterMap;
};



extern ResourceTracker gResourceTracker;



#define LEPRA_ADD_RESOURCE(name, val)	\
	static HashedString __lTrackName##name(_T(#name));	\
	gResourceTracker.Add(__lTrackName##name, val);
#define LEPRA_DO_ACQUIRE_RESOURCE(name)	LEPRA_ADD_RESOURCE(name, +1)
#define LEPRA_DO_RELEASE_RESOURCE(name)	LEPRA_ADD_RESOURCE(name, -1)

#define LEPRA_TRACK_RESOURCES	1
#if LEPRA_TRACK_RESOURCES
#define LEPRA_ACQUIRE_RESOURCE(name)	LEPRA_DO_ACQUIRE_RESOURCE(name)
#define LEPRA_RELEASE_RESOURCE(name)	LEPRA_DO_RELEASE_RESOURCE(name)
#else // !Tracking
#define LEPRA_ACQUIRE_RESOURCE(name)
#define LEPRA_RELEASE_RESOURCE(name)
#endif // Tracking / !tracking



}
