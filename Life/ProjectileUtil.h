
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
	static bool GetBarrel(Cure::ContextObject* pProjectile, xform& pTransform, vec3& pVelocity);
	static bool GetBarrelByShooter(Cure::CppContextObject* pShooter, xform& pTransform, vec3& pVelocity);
	static void StartBullet(Cure::ContextObject* pBullet, float pMuzzleVelocity, bool pUseBarrel);
	static void BulletMicroTick(Cure::ContextObject* pBullet, float pFrameTime, float pMaxVelocity, float pAcceleration);
	static void Detonate(Cure::ContextObject* pGrenade, bool* pIsDetonated, Launcher* pLauncher, const vec3& pPosition, const vec3& pVelocity, const vec3& pNormal, float pStrength, float pDeleteDelay);
	static void OnBulletHit(Cure::ContextObject* pBullet, bool* pIsDetonated, Launcher* pLauncher, Cure::ContextObject* pTarget);
	static float GetShotSounds(Cure::ContextManager* pManager, const strutil::strvec& pSoundNames, str& pLaunchSoundName, str& pShreekSoundName);

	static vec3 CalculateInitialProjectileDirection(const vec3& pDistance, float pAcceleration, float pTerminalSpeed, const vec3& pGravity, float pAccelerationGravityRecip);
private:
	logclass();
};



}
