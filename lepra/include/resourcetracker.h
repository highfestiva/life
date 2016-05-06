
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "thread.h"
#include "string.h"
#include "unordered.h"



namespace lepra {



class ResourceTracker;



class ResourceTracker {
public:
	typedef std::unordered_map<HashedString, int, HashedStringHasher> CounterMap;

	ResourceTracker();
	void Add(const HashedString& resource_name, int value);
	CounterMap GetAll() const;

protected:
	mutable Lock lock_;
	CounterMap counter_map_;
};



extern ResourceTracker g_resource_tracker;



#define LEPRA_ADD_RESOURCE(name, val)	\
	static HashedString __lTrackName##name(#name);	\
	g_resource_tracker.Add(__lTrackName##name, val);
#define LEPRA_DO_ACQUIRE_RESOURCE(name)	LEPRA_ADD_RESOURCE(name, +1)
#define LEPRA_DO_RELEASE_RESOURCE(name)	LEPRA_ADD_RESOURCE(name, -1)

#define kLepraTrackResources	1
#if kLepraTrackResources
#define LEPRA_ACQUIRE_RESOURCE(name)	LEPRA_DO_ACQUIRE_RESOURCE(name)
#define LEPRA_RELEASE_RESOURCE(name)	LEPRA_DO_RELEASE_RESOURCE(name)
#else // !Tracking
#define LEPRA_ACQUIRE_RESOURCE(name)
#define LEPRA_RELEASE_RESOURCE(name)
#endif // Tracking / !tracking



}
