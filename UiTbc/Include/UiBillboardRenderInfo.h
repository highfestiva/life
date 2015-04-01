
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiTbc.h"



namespace UiTbc
{



struct BillboardRenderInfo
{
public:
	inline BillboardRenderInfo(float pAngle, const vec3& pPosition, float pScale, const vec3& pColor, float pOpacity, int pUVIndex):
		mAngle(pAngle),
		mPosition(pPosition),
		mScale(pScale),
		mColor(pColor),
		mOpacity(pOpacity),
		mUVIndex(pUVIndex)
	{
	}

	float mAngle;
	vec3 mPosition;
	float mScale;
	vec3 mColor;
	float mOpacity;
	int mUVIndex;
};

typedef std::vector<BillboardRenderInfo> BillboardRenderInfoArray;


}
