
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/Transformation.h"
#include "Life.h"



namespace Tbc
{
class ChunkyBoneGeometry;
}



namespace Life
{



class Launcher
{
public:
	virtual void Shoot(Cure::ContextObject* pAvatar, int pWeapon) = 0;
	virtual void Detonate(Cure::ContextObject* pExplosive, const Tbc::ChunkyBoneGeometry* pExplosiveGeometry, const vec3& pPosition, const vec3& pVelocity, const vec3& pNormal, float pStrength) = 0;
	virtual void OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject) = 0;
};



}
