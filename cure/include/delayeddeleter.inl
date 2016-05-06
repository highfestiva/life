
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../include/delayeddeleter.h"



namespace cure {



template<class _T>
DelayedDeleter<_T>::DelayedDeleter(ResourceManager* resource_manager, ContextManager* manager, _T* object):
	Parent(resource_manager, "DelayedDeleter"),
	object_(object) {
	manager->AddLocalObject(this);
	GetManager()->PostKillObject(GetInstanceId());
}

template<class _T>
DelayedDeleter<_T>::~DelayedDeleter() {
	delete object_;
	object_ = 0;
}



}
