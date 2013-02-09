
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Lepra/Include/Transformation.h"
#include "Life.h"



namespace Cure
{
class ContextObject;
class GameManager;
}



namespace Life
{



class Launcher;



class ProjectileUtil
{
public:
	static bool GetBarrel(Cure::ContextObject* pProjectile, TransformationF& pTransform, Vector3DF& pVelocity);
	static void StartBullet(Cure::ContextObject* pBullet, float pMuzzleVelocity, bool pUseBarrel);
	static void BulletMicroTick(Cure::ContextObject* pBullet, float pFrameTime, float pMaxVelocity, float pAcceleration);
	static void Detonate(Cure::ContextObject* pGrenade, bool* pIsDetonated, Launcher* pLauncher, const Vector3DF& pPosition, float pStrength);
	static void OnBulletHit(Cure::ContextObject* pBullet, bool* pIsDetonated, Launcher* pLauncher, Cure::ContextObject* pTarget);

private:
	LOG_CLASS_DECLARE();
};



}
