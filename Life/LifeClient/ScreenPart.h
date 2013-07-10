
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life.h"



namespace Life
{



class ScreenPart
{
public:
	virtual PixelRect GetRenderArea() const = 0;
	virtual float UpdateFrustum(float pFov) = 0;	// Returns actual FoV.
};



}
