
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/lepraassert.h"
#include "../include/resourcetracker.h"



namespace lepra {



ResourceTracker::ResourceTracker() {
}

void ResourceTracker::Add(const HashedString& resource_name, int value) {
	ScopeLock lock(&lock_);
	CounterMap::iterator x = counter_map_.find(resource_name);
	if (x == counter_map_.end()) {
		counter_map_.insert(CounterMap::value_type(resource_name, value));
	} else {
		x->second += value;
	}
}

ResourceTracker::CounterMap ResourceTracker::GetAll() const {
	ScopeLock lock(&lock_);
	return counter_map_;
}



ResourceTracker g_resource_tracker;



}
