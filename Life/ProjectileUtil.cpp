
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "ProjectileUtil.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/Random.h"
#include "Launcher.h"



namespace Life
{



bool ProjectileUtil::GetBarrel(Cure::ContextObject* pProjectile, TransformationF& pTransform, Vector3DF& pVelocity)
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

bool ProjectileUtil::GetBarrelByShooter(Cure::CppContextObject* pShooter, TransformationF& pTransform, Vector3DF& pVelocity)
{
	pTransform.SetOrientation(pShooter->GetOrientation());
	pTransform.GetOrientation().RotateAroundOwnX(-PIF/2);
	pTransform.SetPosition(pShooter->GetPosition());
	pVelocity = pShooter->GetVelocity();
	const TBC::ChunkyClass::Tag* lTag = pShooter->FindTag(_T("muzzle"), 0, 0);
	if (lTag)
	{
		const int lBoneIndex = lTag->mBodyIndexList[0];
		const TBC::ChunkyBoneGeometry* lBone = pShooter->GetPhysics()->GetBoneGeometry(lBoneIndex);
		deb_assert(lBone->GetBoneType() == TBC::ChunkyBoneGeometry::BONE_POSITION);
#ifdef LEPRA_DEBUG
		//TBC::ChunkyBoneGeometry* lRootGeometry = pShooter->GetPhysics()->GetBoneGeometry(0);
		//QuaternionF q = pGameManager->GetPhysicsManager()->GetBodyOrientation(lRootGeometry->GetBodyId());
		//QuaternionF p = pShooter->GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		//mLog.Infof(_T("Shooting with body orientation (%f;%f;%f;%f) and initial orientation (%f;%f;%f;%f)."),
		//	q.a, q.b, q.c, q.d,
		//	p.a, p.b, p.c, p.d);
#endif // Debug
		const int lParentIndex = pShooter->GetPhysics()->GetIndex(lBone->GetParent());
		const TBC::PhysicsManager::BodyID lParentBodyId = pShooter->GetPhysics()->GetBoneGeometry(lParentIndex)->GetBodyId();
		const QuaternionF lParentOrientation = pShooter->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyOrientation(lParentBodyId);
		const Vector3DF lMuzzleOffset = pShooter->GetPhysics()->GetOriginalBoneTransformation(lBoneIndex).GetPosition();
		pTransform.GetPosition() += lParentOrientation * lMuzzleOffset;
		pTransform.SetOrientation(lParentOrientation);
	}
	return true;
}

void ProjectileUtil::StartBullet(Cure::ContextObject* pBullet, float pMuzzleVelocity, bool pUseBarrel)
{
	TransformationF lTransform;
	if (pUseBarrel)
	{
		Vector3DF lParentVelocity;
		if (!GetBarrel(pBullet, lTransform, lParentVelocity))
		{
			return;
		}
		Vector3DF lVelocity = lTransform.GetOrientation() * Vector3DF(0, 0, pMuzzleVelocity);
		lVelocity += lParentVelocity;
		pBullet->SetRootOrientation(lTransform.GetOrientation());
		pBullet->SetRootVelocity(lVelocity);
		lTransform.GetPosition() += lTransform.GetOrientation() * Vector3DF(0, 0, 2);
	}
	else
	{
		lTransform = pBullet->GetInitialTransform();
	}
	const TBC::ChunkyBoneGeometry* lGeometry = pBullet->GetPhysics()->GetBoneGeometry(pBullet->GetPhysics()->GetRootBone());
	pBullet->GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(lGeometry->GetTriggerId(), lTransform);

	pBullet->GetManager()->EnableMicroTickCallback(pBullet);	// Used hires movement/collision detection.
}

void ProjectileUtil::BulletMicroTick(Cure::ContextObject* pBullet, float pFrameTime, float pMaxVelocity, float pAcceleration)
{
	const TBC::ChunkyBoneGeometry* lRootGeometry = pBullet->GetPhysics()->GetBoneGeometry(0);
	TBC::PhysicsManager::TriggerID lTrigger = lRootGeometry->GetTriggerId();
	TransformationF lTransform;
	pBullet->GetManager()->GetGameManager()->GetPhysicsManager()->GetTriggerTransform(lTrigger, lTransform);
	Vector3DF lVelocity = pBullet->GetVelocity();
	lTransform.GetPosition() += lVelocity * pFrameTime;
	lTransform.GetOrientation() = pBullet->GetOrientation();
	pBullet->GetManager()->GetGameManager()->GetPhysicsManager()->SetTriggerTransform(lTrigger, lTransform);
	if (pAcceleration && lVelocity.GetLengthSquared() < pMaxVelocity*pMaxVelocity)
	{
		const Vector3DF lForward(0, pAcceleration*pFrameTime, 0);
		lVelocity += lTransform.GetOrientation() * lForward;
		pBullet->SetRootVelocity(lVelocity);
	}
}

void ProjectileUtil::Detonate(Cure::ContextObject* pGrenade, bool* pIsDetonated, Launcher* pLauncher, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal,
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

	TBC::ChunkyPhysics* lPhysics = pGrenade->GetPhysics();
	if (lPhysics)
	{
		pLauncher->Detonate(pGrenade, lPhysics->GetBoneGeometry(0), pPosition, pVelocity, pNormal, pStrength);
		if (pDeleteDelay == 0)
		{
			pGrenade->GetManager()->PostKillObject(pGrenade->GetInstanceId());
		}
		else if (pDeleteDelay > 0)
		{
			pGrenade->GetManager()->GetGameManager()->DeleteContextObjectDelay(pGrenade, pDeleteDelay);
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
		CURE_RTVAR_GET(lPitch, = (float), pManager->GetGameManager()->GetVariableScope(), RTVAR_PHYSICS_RTR, 1.0);
		return lPitch;
	}
	return 0;
}



Vector3DF ProjectileUtil::CalculateInitialProjectileDirection(const Vector3DF& pDistance, float pAcceleration, float pTerminalSpeed, const Vector3DF& pGravity)
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
	//const Vector3DF g = pGravity.GetNormalized();
	//const float f = 1 + pDistance*g/l;
	//d *= f;
	// 4
	const Vector3DF lTarget = pDistance - d*pGravity;
	const float xy = ::sqrt(lTarget.x*lTarget.x + lTarget.y*lTarget.y);
	//const float zy = ::sqrt(lTarget.z*lTarget.z + lTarget.y*lTarget.y);
	return Vector3DF(::atan2(-lTarget.x, lTarget.y), ::atan2(lTarget.z, xy), 0);
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ProjectileUtil);



}
