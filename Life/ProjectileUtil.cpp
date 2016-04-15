
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "ProjectileUtil.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/Random.h"
#include "Launcher.h"



namespace Life
{



bool ProjectileUtil::GetBarrel(Cure::ContextObject* pProjectile, xform& pTransform, vec3& pVelocity)
{
	deb_assert(pProjectile);
	//deb_assert(pProjectile->GetOwnerInstanceId());
	Cure::CppContextObject* lShooter = (Cure::CppContextObject*)pProjectile->GetManager()->GetObject(pProjectile->GetOwnerInstanceId());
	if (!lShooter)
	{
		//pProjectile->GetManager()->PostKillObject(pProjectile->GetInstanceId());
		return false;
	}
	return GetBarrelByShooter(lShooter, pTransform, pVelocity);
}

bool ProjectileUtil::GetBarrelByShooter(Cure::CppContextObject* pShooter, xform& pTransform, vec3& pVelocity)
{
	pTransform.SetOrientation(pShooter->GetOrientation());
	pTransform.GetOrientation().RotateAroundOwnX(-PIF/2);
	pTransform.SetPosition(pShooter->GetPosition());
	pVelocity = pShooter->GetVelocity();
	const Tbc::ChunkyClass::Tag* lTag = pShooter->FindTag("muzzle", 0, 0);
	if (lTag)
	{
		const int lBoneIndex = lTag->mBodyIndexList[0];
		const Tbc::ChunkyBoneGeometry* lBone = pShooter->GetPhysics()->GetBoneGeometry(lBoneIndex);
		deb_assert(lBone->GetBoneType() == Tbc::ChunkyBoneGeometry::BONE_POSITION);
#ifdef LEPRA_DEBUG
		//Tbc::ChunkyBoneGeometry* lRootGeometry = pShooter->GetPhysics()->GetBoneGeometry(0);
		//quat q = pGameManager->GetPhysicsManager()->GetBodyOrientation(lRootGeometry->GetBodyId());
		//quat p = pShooter->GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		//mLog.Infof("Shooting with body orientation (%f;%f;%f;%f and initial orientation (%f;%f;%f;%f)."),
		//	q.a, q.b, q.c, q.d,
		//	p.a, p.b, p.c, p.d);
#endif // Debug
		const int lParentIndex = pShooter->GetPhysics()->GetIndex(lBone->GetParent());
		const Tbc::PhysicsManager::BodyID lParentBodyId = pShooter->GetPhysics()->GetBoneGeometry(lParentIndex)->GetBodyId();
		const quat lParentOrientation = pShooter->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyOrientation(lParentBodyId);
		const vec3 lMuzzleOffset = pShooter->GetPhysics()->GetOriginalBoneTransformation(lBoneIndex).GetPosition();
		pTransform.GetPosition() += lParentOrientation * lMuzzleOffset;
		pTransform.SetOrientation(lParentOrientation);
	}
	return true;
}

void ProjectileUtil::StartBullet(Cure::ContextObject* pBullet, float pMuzzleVelocity, bool pUseBarrel)
{
	xform lTransform;
	if (pUseBarrel)
	{
		vec3 lParentVelocity;
		if (!GetBarrel(pBullet, lTransform, lParentVelocity))
		{
			return;
		}
		vec3 lVelocity = lTransform.GetOrientation() * vec3(0, 0, pMuzzleVelocity);
		lVelocity += lParentVelocity;
		pBullet->SetRootOrientation(lTransform.GetOrientation());
		pBullet->SetRootVelocity(lVelocity);
		lTransform.GetPosition() += lTransform.GetOrientation() * vec3(0, 0, 2);
	}
	else
	{
		lTransform = pBullet->GetInitialTransform();
	}
	const Tbc::ChunkyBoneGeometry* lGeometry = pBullet->GetPhysics()->GetBoneGeometry(pBullet->GetPhysics()->GetRootBone());
	pBullet->GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(lGeometry->GetBodyId(), lTransform);
}

void ProjectileUtil::Detonate(Cure::ContextObject* pGrenade, bool* pIsDetonated, Launcher* pLauncher, const vec3& pPosition, const vec3& pVelocity, const vec3& pNormal,
	float pStrength, float pDeleteDelay)
{
	/*if (pOtherObject->GetInstanceId() == GetOwnerInstanceId())
	{
		return;
	}*/
	if (*pIsDetonated)
	{
		return;
	}
	*pIsDetonated = true;

	Tbc::ChunkyPhysics* lPhysics = pGrenade->GetPhysics();
	if (lPhysics)
	{
		pLauncher->Detonate(pGrenade, lPhysics->GetBoneGeometry(0), pPosition, pVelocity, pNormal, pStrength);
		if (pDeleteDelay == 0)
		{
			pGrenade->GetManager()->PostKillObject(pGrenade->GetInstanceId());
		}
		else if (pDeleteDelay > 0)
		{
			pGrenade->GetManager()->DelayKillObject(pGrenade, pDeleteDelay);
		}
	}
}

void ProjectileUtil::OnBulletHit(Cure::ContextObject* pBullet, bool* pIsDetonated, Launcher* pLauncher, Cure::ContextObject* pTarget)
{
	if (pTarget->GetInstanceId() == pBullet->GetOwnerInstanceId())	// Can't hit oneself.
	{
		return;
	}
	if (*pIsDetonated)
	{
		return;
	}
	*pIsDetonated = true;

	pLauncher->OnBulletHit(pBullet, pTarget);
	pBullet->GetManager()->PostKillObject(pBullet->GetInstanceId());
}

float ProjectileUtil::GetShotSounds(Cure::ContextManager* pManager, const strutil::strvec& pSoundNames, str& pLaunchSoundName, str& pShreekSoundName)
{
	const size_t lSoundCount = pSoundNames.size() / 2;	// First half are launch sounds, last half are shreek sounds.
	if (lSoundCount)
	{
		pLaunchSoundName = pSoundNames[Random::GetRandomNumber()%lSoundCount];
		pShreekSoundName = pSoundNames[Random::GetRandomNumber()%lSoundCount + lSoundCount];
		float lPitch;
		v_get(lPitch, = (float), pManager->GetGameManager()->GetVariableScope(), RTVAR_PHYSICS_RTR, 1.0);
		return lPitch;
	}
	return 0;
}



vec3 ProjectileUtil::CalculateInitialProjectileDirection(const vec3& pDistance, float pAcceleration, float pTerminalSpeed, const vec3& pGravity, float pAccelerationGravityRecip)
{
	// 1. How long time, t, will it take the missile to accelerate to the endpoint?
	// 2. Given t, how much (d) will the missile fall during it's travel (excluding g for optimization)?
	// 3. Compensate for projectile acceleration in gravitational direction.
	// 4. Create a quaternion pointing to dir+d.
	const float l = pDistance.GetLength();
	const float a = pAcceleration;
	const float vt = pTerminalSpeed;
	const float r = ::exp(l*a/(vt*vt));
	float d = 0;
	deb_assert(r >= 1);
	if (r >= 1)
	{
		const float t = vt/a*Math::acosh(r);	// Derived from "free fall with air resistance" in Wikipedia. Thanks a bunch!
		// 2
		d = t*t*0.5f;
	}
	// 3
	const vec3 g = pGravity.GetNormalized(pAccelerationGravityRecip);
	const float f = 1 + pDistance*g/l;
	d *= f;
	// 4
	const vec3 lTarget = pDistance - d*pGravity;
	const float xy = ::sqrt(lTarget.x*lTarget.x + lTarget.y*lTarget.y);
	//const float zy = ::sqrt(lTarget.z*lTarget.z + lTarget.y*lTarget.y);
	return vec3(::atan2(-lTarget.x, lTarget.y), ::atan2(lTarget.z, xy), 0);
}



loginstance(GAME_CONTEXT, ProjectileUtil);



}
