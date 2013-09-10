
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "HomingProjectile.h"
#include "../../Cure/Include/ContextManager.h"



namespace Life
{



HomingProjectile::HomingProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager, pLauncher),
	mTarget(0)
{
}

HomingProjectile::~HomingProjectile()
{
}

void HomingProjectile::SetTarget(Cure::GameObjectId pTarget)
{
	mTarget = pTarget;
}



void HomingProjectile::OnTick()
{
	Parent::OnTick();

	Cure::ContextObject* lObject = GetManager()->GetObject(mTarget);
	if (lObject && lObject->GetPhysics()->GetEngineCount() >= 1)
	{
		const Cure::ObjectPositionalData* lPositionalData = 0;
		UpdateFullPosition(lPositionalData);
		if (lPositionalData)
		{
			Vector3DF lDelta = (lObject->GetPosition() - lPositionalData->mPosition.mTransformation.GetPosition()).GetNormalized();
			if (mMaxVelocity > 0)
			{
				const float t = lDelta.GetLength() / mMaxVelocity;
				lDelta += lObject->GetVelocity() * t;
			}
			const float xy = lDelta.ProjectOntoPlane(Vector3DF(0,0,1)).GetLength();
			QuaternionF q;
			q.SetEulerAngles(-::atan2(lDelta.x, lDelta.y), ::atan2(lDelta.z, xy), 0);
			const float v = lPositionalData->mPosition.mVelocity.GetLength();
			const Vector3DF lVelocity = q * Vector3DF(0, v, 0);
			SetRootOrientation(q);
			SetRootVelocity(lVelocity);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, HomingProjectile);



}
