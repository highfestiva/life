
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiTBC.h"



namespace UiTbc
{



struct BillboardRenderInfo
{
public:
	inline BillboardRenderInfo(float pAngle, const Vector3DF& pPosition, float pScale, const Vector3DF& pColor, float pOpacity, int pUVIndex):
		mAngle(pAngle),
		mPosition(pPosition),
		mScale(pScale),
		mColor(pColor),
		mOpacity(pOpacity),
		mUVIndex(pUVIndex)
	{
	}

	float mAngle;
	Vector3DF mPosition;
	float mScale;
	Vector3DF mColor;
	float mOpacity;
	int mUVIndex;
};

typedef std::vector<BillboardRenderInfo> BillboardRenderInfoArray;


}
