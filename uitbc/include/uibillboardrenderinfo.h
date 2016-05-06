
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uitbc.h"



namespace uitbc {



struct BillboardRenderInfo {
public:
	inline BillboardRenderInfo(float angle, const vec3& position, float scale, const vec3& color, float opacity, int uv_index):
		angle_(angle),
		position_(position),
		scale_(scale),
		color_(color),
		opacity_(opacity),
		uv_index_(uv_index) {
	}

	float angle_;
	vec3 position_;
	float scale_;
	vec3 color_;
	float opacity_;
	int uv_index_;
};

typedef std::vector<BillboardRenderInfo> BillboardRenderInfoArray;


}
