
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushBarrel.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/GameManager.h"



namespace Push
{



void PushBarrel::GetInfo(const Cure::GameManager* pGameManager, Cure::GameObjectId pAvatarId, TransformationF& pTransform, Vector3DF& pVelocity)
{
	const Cure::CppContextObject* lAvatar = (const Cure::CppContextObject*)pGameManager->GetContext()->GetObject(pAvatarId);
	if (lAvatar)
	{
		pTransform.SetOrientation(lAvatar->GetOrientation());
		pTransform.GetOrientation().RotateAroundOwnX(-PIF/2);
		pTransform.SetPosition(lAvatar->GetPosition());
		pVelocity = lAvatar->GetVelocity();
		std::vector<int> lBodyArray;
		lBodyArray.push_back(-1);
		const TBC::ChunkyClass::Tag* lTag = lAvatar->FindTag(_T("context_path"), 0, 0, lBodyArray, true);
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
			const QuaternionF lRootOrientation = pGameManager->GetPhysicsManager()->GetBodyOrientation(lRootBodyId);
			const Vector3DF lMuzzleOffset = lAvatar->GetPhysics()->GetOriginalBoneTransformation(lBoneIndex).GetPosition();
			pTransform.GetPosition() += lRootOrientation * lMuzzleOffset;
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT, PushBarrel);



}
