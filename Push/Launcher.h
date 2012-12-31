
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "Push.h"



namespace Push
{



class Launcher
{
public:
	virtual void GetBarrel(TransformationF& pTransform, Vector3DF& pVelocity) const = 0;

	virtual void Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry) = 0;
};



}
