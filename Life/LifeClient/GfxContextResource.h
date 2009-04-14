
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "../ContextResource.h"



namespace Life
{



class GfxContextObjectInfoResource: public ContextObjectInfoResource
{
	typedef Cure::ContextObjectResource Parent;
public:
	GfxContextObjectInfoResource(Cure::ResourceManager* pManager, const Lepra::String& pName, ContextObjectInfo pInfo);
	Cure::ResourceLoadState PostProcess();
};



typedef Cure::UserExtraCreateTypeResource<GfxContextObjectInfoResource, ContextObjectInfo> UserGfxContextObjectInfoResource;



}
