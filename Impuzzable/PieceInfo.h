
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "Impuzzable.h"



namespace Impuzzable
{



struct PieceInfo
{
	PieceInfo();
	virtual ~PieceInfo();

	str mGfxMesh;
	strutil::strvec mPhysGeometries;
};



}
