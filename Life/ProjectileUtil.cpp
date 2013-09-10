
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "ProjectileUtil.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/GameManager.h"
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
		pProjectile->GetManager()->PostKillObject(pProjectile->GetInstanceId());
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
#ifdef LEPRA_DEBUG
		const TBC::ChunkyBoneGeometry* lBone = pShooter->GetPhysics()->GetBoneGeometry(lBoneIndex);
		deb_assert(lBone->GetBoneType() == TBC::ChunkyBoneGeometry::BONE_POSITION);
		//TBC::ChunkyBoneGeometry* lRootGeometry = pShooter->GetPhysics()->GetBoneGeometry(0);
		//QuaternionF q = pGameManager->GetPhysicsManager()->GetBodyOrientation(lRootGeometry->GetBodyId());
		//QuaternionF p = pShooter->GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		//mLog.Infof(_T("Shooting with body orientation (%f;%f;%f;%f) and initial orientation (%f;%f;%f;%f)."),
		//	q.GetA(), q.GetB(), q.GetC(), q.GetD(),
		//	p.GetA(), p.GetB(), p.GetC(), p.GetD());
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

void ProjectileUtil::Detonate(Cure::ContextObject* pGrenade, bool* pIsDetonated, Launcher* pLauncher, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength)
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
		pGrenade->GetManager()->PostKillObject(pGrenade->GetInstanceId());
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



LOG_CLASS_DEFINE(GAME_CONTEXT, ProjectileUtil);



}
