
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life.h"



namespace life {



class ScreenPart {
public:
	virtual PixelRect GetRenderArea() const = 0;
	virtual float UpdateFrustum(float fov) = 0;	// Returns actual FoV.
};



}
