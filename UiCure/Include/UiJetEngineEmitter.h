
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uicppcontextobject.h"



namespace UiCure {



class JetEngineEmitter {
public:
	JetEngineEmitter(cure::ResourceManager* resource_manager, GameUiManager* ui_manager);
	virtual ~JetEngineEmitter();

	void EmitFromTag(const CppContextObject* object, const uitbc::ChunkyClass::Tag& tag, float frame_time);

private:
	void DrawOvershoot(const vec3& position, float distance_scale_factor, const vec3& radius, const vec3& color, float opacity, const vec3& camera_direction);

	cure::ResourceManager* resource_manager_;
	GameUiManager* ui_manager_;
	float interleave_timeout_;

	logclass();
};



}
