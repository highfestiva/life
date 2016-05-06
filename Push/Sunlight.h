
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/hirestimer.h"
#include "../uitbc/include/uirenderer.h"
#include "push.h"



namespace UiCure {
class GameUiManager;
}



namespace Push {



class Sunlight {
public:
	Sunlight(UiCure::GameUiManager* ui_manager);
	virtual ~Sunlight();

	void Tick(float factor);
	const vec3& GetDirection() const;
	void AddSunColor(vec3& base_color, float factor) const;
	float GetTimeOfDay() const;	// Value from 0->1. 0 is 9 am.

	void SetDirection(const vec3& direction);
	void SetColor(const vec3& color);

private:
	UiCure::GameUiManager* ui_manager_;
	uitbc::Renderer::LightID light_id_;
	vec3 cam_sun_direction_;
	HiResTimer time_;
	float angle_;
};



}
