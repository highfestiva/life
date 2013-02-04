
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "ProjectileUtil.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/GameManager.h"
#include "Launcher.h"



namespace Life
{



void ProjectileUtil::GetBarrel(Cure::ContextObject* pProjectile, TransformationF& pTransform, Vector3DF& pVelocity)
{
	assert(pProjectile);
	assert(pProjectile->GetOwnerInstanceId());
	Cure::CppContextObject* lAvatar = (Cure::CppContextObject*)pProjectile->GetManager()->GetObject(pProjectile->GetOwnerInstanceId());
	if (!lAvatar)
	{
		return;
	}

	pTransform.SetOrientation(lAvatar->GetOrientation());
	pTransform.GetOrientation().RotateAroundOwnX(-PIF/2);
	pTransform.SetPosition(lAvatar->GetPosition());
	pVelocity = lAvatar->GetVelocity();
	const TBC::ChunkyClass::Tag* lTag = lAvatar->FindTag(_T("context_path"), 0, 0);
	if (lTag)
	{
		const int lBoneIndex = lTag->mBodyIndexList[0];
#ifdef LEPRA_DEBUG
		const TBC::ChunkyBoneGeometry* lBone = lAvatar->GetPhysics()->GetBoneGeometry(lBoneIndex);
		assert(lBone->GetBoneType() == TBC::ChunkyBoneGeometry::BONE_POSITION);
		//TBC::ChunkyBoneGeometry* lRootGeometry = lAvatar->GetPhysics()->GetBoneGeometry(0);
		//QuaternionF q = pGameManager->GetPhysicsManager()->GetBodyOrientation(lRootGeometry->GetBodyId());
		//QuaternionF p = lAvatar->GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		//mLog.Infof(_T("Shooting with body orientation (%f;%f;%f;%f) and initial orientation (%f;%f;%f;%f)."),
		//	q.GetA(), q.GetB(), q.GetC(), q.GetD(),
		//	p.GetA(), p.GetB(), p.GetC(), p.GetD());
#endif // Debug
		const TBC::PhysicsManager::BodyID lRootBodyId = lAvatar->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		const QuaternionF lRootOrientation = lAvatar->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyOrientation(lRootBodyId);
		const Vector3DF lMuzzleOffset = lAvatar->GetPhysics()->GetOriginalBoneTransformation(lBoneIndex).GetPosition();
		pTransform.GetPosition() += lRootOrientation * lMuzzleOffset;
	}
}

void ProjectileUtil::StartBullet(Cure::ContextObject* pBullet, float pMuzzleVelocity, bool pUseBarrel)
{
	TransformationF lTransform;
	if (pUseBarrel)
	{
		Vector3DF lParentVelocity;
		GetBarrel(pBullet, lTransform, lParentVelocity);
		Vector3DF lVelocity = lTransform.GetOrientation() * Vector3DF(0, 0, pMuzzleVelocity);
		lVelocity += lParentVelocity;
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
	TBC::PhysicsManager::BodyID lBody = lRootGeometry->GetTriggerId();
	TransformationF lTransform;
	pBullet->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lBody, lTransform);
	Vector3DF lVelocity = pBullet->GetVelocity();
	lTransform.GetPosition() += lVelocity * pFrameTime;
	pBullet->GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyPosition(lBody, lTransform.GetPosition());
	if (pAcceleration && lVelocity.GetLengthSquared() < pMaxVelocity*pMaxVelocity)
	{
		lVelocity += lTransform.GetOrientation() * Vector3DF(0, 0, pAcceleration*pFrameTime);
		pBullet->SetRootVelocity(lVelocity);
	}
}

void ProjectileUtil::Detonate(Cure::ContextObject* pGrenade, bool* pIsDetonated, Launcher* pLauncher, const Vector3DF& pPosition, float pStrength)
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
		pLauncher->Detonate(pGrenade, lPhysics->GetBoneGeometry(0), pPosition, pStrength);
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
