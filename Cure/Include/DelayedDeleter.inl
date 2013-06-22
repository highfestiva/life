
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/DelayedDeleter.h"



namespace Cure
{



template<class _T>
DelayedDeleter<_T>::DelayedDeleter(ResourceManager* pResourceManager, ContextManager* pManager, _T* pObject):
	Parent(pResourceManager, _T("DelayedDeleter")),
	mObject(pObject)
{
	pManager->AddLocalObject(this);
	GetManager()->PostKillObject(GetInstanceId());
}

template<class _T>
DelayedDeleter<_T>::~DelayedDeleter()
{
	delete mObject;
	mObject = 0;
}



}
