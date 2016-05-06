
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "sunlight.h"
#include "../cure/include/runtimevariable.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiruntimevariablename.h"



namespace HoverTank {



Sunlight::Sunlight(UiCure::GameUiManager* ui_manager):
	ui_manager_(ui_manager),
	angle_(0) {
	const bool pixel_shaders_enabled = ui_manager_->GetRenderer()->IsPixelShadersEnabled();
	light_id_ = ui_manager_->GetRenderer()->AddDirectionalLight(
		uitbc::Renderer::kLightMovable, vec3(0, 0.5f, -1),
		vec3(1,1,1) * (pixel_shaders_enabled? 1.0f : 1.5f), 60);
}

Sunlight::~Sunlight() {
	ui_manager_->GetRenderer()->RemoveLight(light_id_);
}



void Sunlight::Tick(float factor) {
	angle_ += (float)time_.PopTimeDiff() * factor * 0.01f;	// TODO: use global game time, not this local-start-relative shit.
	if (angle_ > 2*PIF) {
		angle_ = ::fmod(angle_, 2*PIF);
	}
	// "Physical" and "light" height actual differ, otherwise sun would never
	// be seen in the camera, since it would be to far up in the sky.
	const float light_sun_height = sin(angle_)*1.2f + 1.6f;
	cam_sun_direction_.x = sin(angle_);
	cam_sun_direction_.y = cos(angle_);
	cam_sun_direction_.z = -light_sun_height;
	SetDirection(cam_sun_direction_);
	const float r = 1.5f;
	const float g = r * (sin(angle_)*0.05f + 0.95f);
	const float b = r * (sin(angle_)*0.1f + 0.9f);
	ui_manager_->GetRenderer()->SetLightColor(light_id_, vec3(r, g, b));
	const double ambient_factor = sin(angle_)*0.5+0.5;
	v_internal(ui_manager_->GetVariableScope(), kRtvarUi3DAmbientredfactor, ambient_factor);
	v_internal(ui_manager_->GetVariableScope(), kRtvarUi3DAmbientgreenfactor, ambient_factor);
	v_internal(ui_manager_->GetVariableScope(), kRtvarUi3DAmbientbluefactor, ambient_factor);
}

const vec3& Sunlight::GetDirection() const {
	return cam_sun_direction_;
}

void Sunlight::AddSunColor(vec3& base_color, float factor) const {
	float color_curve = sin(angle_)*0.3f*factor + 1;
	color_curve *= color_curve;
	base_color.x = Math::Clamp(base_color.x * color_curve, 0.0f, 1.0f);
	base_color.y = Math::Clamp(base_color.y * color_curve, 0.0f, 1.0f);
	base_color.z = Math::Clamp(base_color.z * color_curve, 0.0f, 1.0f);
}

float Sunlight::GetTimeOfDay() const {
	return angle_ / PIF * 2;
}

void Sunlight::SetDirection(const vec3& direction) {
	if (ui_manager_->CanRender()) {
		ui_manager_->GetRenderer()->SetLightDirection(light_id_, direction);
	}
}

void Sunlight::SetColor(const vec3& color) {
	ui_manager_->GetRenderer()->SetLightColor(light_id_, color);
}



}
