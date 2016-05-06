
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "sunlight.h"
#include "../uicure/include/uigameuimanager.h"



namespace Downwash {



Sunlight::Sunlight(UiCure::GameUiManager* ui_manager):
	ui_manager_(ui_manager) {
	const bool pixel_shaders_enabled = ui_manager_->GetRenderer()->IsPixelShadersEnabled();
	light_id_ = ui_manager_->GetRenderer()->AddDirectionalLight(
		uitbc::Renderer::kLightMovable, vec3(-0.2f, +0.2f, -1),
		vec3(1,1,1) * (pixel_shaders_enabled? 1.0f : 1.5f), 60);
}

Sunlight::~Sunlight() {
	ui_manager_->GetRenderer()->RemoveLight(light_id_);
}



}
