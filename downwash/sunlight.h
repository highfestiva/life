
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/hirestimer.h"
#include "../uitbc/include/uirenderer.h"
#include "downwash.h"



namespace UiCure {
class GameUiManager;
}



namespace Downwash {



class Sunlight {
public:
	Sunlight(UiCure::GameUiManager* ui_manager);
	virtual ~Sunlight();

private:
	UiCure::GameUiManager* ui_manager_;
	uitbc::Renderer::LightID light_id_;
};



}
