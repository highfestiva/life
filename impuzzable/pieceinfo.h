
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "impuzzable.h"



namespace Impuzzable {



struct PieceInfo {
	PieceInfo();
	virtual ~PieceInfo();

	str gfx_mesh_;
	strutil::strvec phys_geometries_;
};



}
