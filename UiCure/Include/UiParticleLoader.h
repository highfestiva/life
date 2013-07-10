
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	ParticleLoader(Cure::ResourceManager* pResourceManager, UiTbc::Renderer* pRenderer, const str& pTextureName, size_t pSubFeatureTextureCount, size_t pSubTotalTextureCount);

private:
	LOG_CLASS_DECLARE();
};



}
