
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
