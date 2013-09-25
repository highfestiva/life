
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/Transformation.h"
#include "Life.h"



namespace Cure
{
class ContextManager;
class ContextObject;
class CppContextObject;
class GameManager;
}



namespace Life
{



class Launcher;



class ProjectileUtil
{
public:
	static bool GetBarrel(Cure::ContextObject* pProjectile, TransformationF& pTransform, Vector3DF& pVelocity);
	static bool GetBarrelByShooter(Cure::CppContextObject* pShooter, TransformationF& pTransform, Vector3DF& pVelocity);
	static void StartBullet(Cure::ContextObject* pBullet, float pMuzzleVelocity, bool pUseBarrel);
	static void BulletMicroTick(Cure::ContextObject* pBullet, float pFrameTime, float pMaxVelocity, float pAcceleration);
	static void Detonate(Cure::ContextObject* pGrenade, bool* pIsDetonated, Launcher* pLauncher, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength, float pDeleteDelay);
	static void OnBulletHit(Cure::ContextObject* pBullet, bool* pIsDetonated, Launcher* pLauncher, Cure::ContextObject* pTarget);
	static float GetShotSounds(Cure::ContextManager* pManager, const strutil::strvec& pSoundNames, str& pLaunchSoundName, str& pShreekSoundName);

private:
	LOG_CLASS_DECLARE();
};



}
