
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../UiCure/Include/UiCppContextObject.h"
#include "GfxContextResource.h"



namespace Life
{



GfxContextObjectInfoResource::GfxContextObjectInfoResource(Cure::ResourceManager* pManager,
	const Lepra::String& pName, ContextObjectInfo pInfo):
	ContextObjectInfoResource(pManager, pName, pInfo)
{
}

Cure::ResourceLoadState GfxContextObjectInfoResource::PostProcess()
{
	UiCure::CppContextObject* lObject = (UiCure::CppContextObject*)GetRamData();
	Cure::ResourceLoadState  lLoadState = lObject->StartLoadGraphics(mInfo.mLoadingResource) ?
		Cure::RESOURCE_LOAD_COMPLETE : Cure::RESOURCE_LOAD_ERROR;
	if (lLoadState == Cure::RESOURCE_LOAD_COMPLETE)
	{
		lLoadState = Parent::PostProcess();
	}
	return (lLoadState);
}



}
