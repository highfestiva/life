
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Lepra/Include/Transformation.h"
#include "Life.h"



namespace TBC
{
class ChunkyBoneGeometry;
}



namespace Life
{



class Launcher
{
public:
	virtual void Shoot(Cure::ContextObject* pAvatar, int pWeapon) = 0;
	virtual void Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition) = 0;
	virtual void OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject) = 0;
};



}
