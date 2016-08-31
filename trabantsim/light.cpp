
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "light.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiruntimevariablename.h"
#include "rtvar.h"



namespace TrabantSim {



Light::Light(UiCure::GameUiManager* ui_manager):
	ui_manager_(ui_manager),
	light_average_direction_(-0.2f,1,-1) {
	const bool pixel_shaders_enabled = ui_manager_->GetRenderer()->IsPixelShadersEnabled();
	light_id_ = ui_manager_->GetRenderer()->AddDirectionalLight(
		uitbc::Renderer::kLightMovable, light_average_direction_,
		vec3(0.6f,0.6f,0.6f) * (pixel_shaders_enabled? 1.0f : 1.5f), 100);
}

Light::~Light() {
	ui_manager_->GetRenderer()->RemoveLight(light_id_);
}

void Light::Tick(const quat& camera_orientation) {
	(void)camera_orientation;
	if (!ui_manager_->CanRender()) {
		return;
	}
	float ax,az;
	v_get(ax, =(float), ui_manager_->GetVariableScope(), kRtvarUi3DLightanglex, -1.4);
	v_get(az, =(float), ui_manager_->GetVariableScope(), kRtvarUi3DLightanglez, 0.1);
	quat q;
	q.RotateAroundOwnZ(az);
	q.RotateAroundOwnX(ax);
	vec3 d = q*vec3(0,1,0);
	d = Math::Lerp(light_average_direction_, d, 0.5f);
	if (d.GetDistanceSquared(light_average_direction_) > 1e-5f) {
		light_average_direction_ = d;
		ui_manager_->GetRenderer()->SetLightDirection(light_id_, light_average_direction_);
	}
}



}
