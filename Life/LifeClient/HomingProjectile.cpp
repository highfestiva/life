
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
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
			vec3 lDelta = (lObject->GetPosition() - lPositionalData->mPosition.mTransformation.GetPosition()).GetNormalized();
			if (mMaxVelocity > 0)
			{
				const float t = lDelta.GetLength() / mMaxVelocity;
				lDelta += lObject->GetVelocity() * t;
			}
			const float xy = lDelta.ProjectOntoPlane(vec3(0,0,1)).GetLength();
			quat q;
			q.SetEulerAngles(-::atan2(lDelta.x, lDelta.y), ::atan2(lDelta.z, xy), 0);
			const float v = lPositionalData->mPosition.mVelocity.GetLength();
			const vec3 lVelocity = q * vec3(0, v, 0);
			SetRootOrientation(q);
			SetRootVelocity(lVelocity);
		}
	}
}



loginstance(GAME_CONTEXT_CPP, HomingProjectile);



}
