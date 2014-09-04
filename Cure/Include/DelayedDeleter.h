
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "CppContextObject.h"
#include "ContextManager.h"



namespace Cure
{



template<class _T>
class DelayedDeleter: public CppContextObject
{
	typedef CppContextObject Parent;
public:
	DelayedDeleter(ResourceManager* pResourceManager, ContextManager* pManager, _T* pObject);
	virtual ~DelayedDeleter();

protected:
	_T* mObject;

	logclass();
};



}



#include "DelayedDeleter.inl"
