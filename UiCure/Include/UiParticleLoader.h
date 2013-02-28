
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiCure.h"



namespace Cure
{
class ResourceManager;
}
namespace UiTbc
{
class Renderer;
}



namespace UiCure
{



class ParticleLoader
{
public:
	ParticleLoader(Cure::ResourceManager* pResourceManager, UiTbc::Renderer* pRenderer, const str& pTextureName, size_t pSubTextureCount);

private:
	LOG_CLASS_DECLARE();
};



}
