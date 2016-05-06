
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uicure.h"



namespace cure {
class ResourceManager;
}
namespace uitbc {
class Renderer;
}



namespace UiCure {



class ParticleLoader {
public:
	ParticleLoader(cure::ResourceManager* resource_manager, uitbc::Renderer* renderer, const str& texture_name, size_t sub_feature_texture_count, size_t sub_total_texture_count);

private:
	logclass();
};



}
