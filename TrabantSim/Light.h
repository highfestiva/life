
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/hirestimer.h"
#include "../uitbc/include/uirenderer.h"
#include "trabantsim.h"



namespace UiCure {
class GameUiManager;
}



namespace TrabantSim {



class Light {
public:
	Light(UiCure::GameUiManager* ui_manager);
	virtual ~Light();
	void Tick(const quat& camera_orientation);

private:
	UiCure::GameUiManager* ui_manager_;
	uitbc::Renderer::LightID light_id_;
    vec3 light_average_direction_;
};



}
