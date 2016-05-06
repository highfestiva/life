
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "sunlight.h"
#include "../cure/include/runtimevariable.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiruntimevariablename.h"



namespace Bound {



Sunlight::Sunlight(UiCure::GameUiManager* ui_manager):
	ui_manager_(ui_manager),
	light_average_direction_(0,0,-1) {
	const bool pixel_shaders_enabled = ui_manager_->GetRenderer()->IsPixelShadersEnabled();
	light_id_ = ui_manager_->GetRenderer()->AddDirectionalLight(
		uitbc::Renderer::kLightMovable, light_average_direction_,
		vec3(0.6f,0.6f,0.6f) * (pixel_shaders_enabled? 1.0f : 1.5f), 100);
}

Sunlight::~Sunlight() {
	ui_manager_->GetRenderer()->RemoveLight(light_id_);
}

void Sunlight::Tick(const quat& camera_orientation) {
	vec3 d = ui_manager_->GetAccelerometer();
	d = camera_orientation*d.GetNormalized();
	d = Math::Lerp(light_average_direction_, d, 0.5f);
	if (d.GetDistanceSquared(light_average_direction_) > 1e-5f) {
		light_average_direction_ = d;
		ui_manager_->GetRenderer()->SetLightDirection(light_id_, light_average_direction_);
	}
}



}
