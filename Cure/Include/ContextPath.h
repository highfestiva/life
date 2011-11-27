
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#pragma once
#include "CppContextObject.h"
#include "../../Lepra/Include/CubicDeCasteljauSpline.h"
#include <vector>



namespace Cure
{



class ContextPath: public CppContextObject
{
public:
	ContextPath(ResourceManager* pResourceManager, const str& pClassId);
	virtual ~ContextPath();

	virtual void SetTagIndex(int pIndex);

//private:
	typedef CubicDeCasteljauSpline<Vector3DF, float> Spline;
	typedef std::vector<Spline*> PathArray;

	PathArray mPathArray;
};



}
