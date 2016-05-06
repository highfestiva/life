
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "cppcontextobject.h"
#include "contextmanager.h"



namespace cure {



template<class _T>
class DelayedDeleter: public CppContextObject {
	typedef CppContextObject Parent;
public:
	DelayedDeleter(ResourceManager* resource_manager, ContextManager* manager, _T* object);
	virtual ~DelayedDeleter();

protected:
	_T* object_;

	logclass();
};



}



#include "delayeddeleter.inl"
