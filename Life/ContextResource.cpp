
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Cure/Include/ContextObject.h"
#include "ContextResource.h"



namespace Life
{



ContextObjectInfoResource::ContextObjectInfoResource(Cure::ResourceManager* pManager,
	const Lepra::String& pName, ContextObjectInfo pInfo):
	ContextObjectResource(pManager, pName),
	mInfo(pInfo)
{
}

bool ContextObjectInfoResource::Load()
{
	assert(!IsUnique());
	bool lOk = Parent::Load();
	if (lOk)
	{
		Cure::ContextObject* lObject = GetRamData();
		lObject->SetManager(mInfo.mContextManager);
		lObject->SetInstanceId(mInfo.mInstanceId);
		lObject->SetNetworkObjectType(mInfo.mNetworkType);
	}
	return (lOk);
}



}
