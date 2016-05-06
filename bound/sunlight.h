
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/hirestimer.h"
#include "../uitbc/include/uirenderer.h"
#include "bound.h"



namespace UiCure {
class GameUiManager;
}



namespace Bound {



class Sunlight {
public:
	Sunlight(UiCure::GameUiManager* ui_manager);
	virtual ~Sunlight();
	void Tick(const quat& camera_orientation);

private:
	UiCure::GameUiManager* ui_manager_;
	uitbc::Renderer::LightID light_id_;
    vec3 light_average_direction_;
};



}
