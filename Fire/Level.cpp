
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Level.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/GameManager.h"



namespace Fire
{



Level::Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Cure::ContextForceListener* pGravelEmitter):
	Parent(pResourceManager, pClassId, pUiManager, pGravelEmitter),
	mPath(0),
	mLevelSpeed(1)
{
}

Level::~Level()
{
}

void Level::OnLoaded()
{
	Parent::OnLoaded();

	const TBC::ChunkyClass::Tag* lSpeedTag = FindTag(_T("driver"), 1, 0);
	if (lSpeedTag)
	{
		mLevelSpeed = lSpeedTag->mFloatValueList[0];
	}
	const TBC::ChunkyClass::Tag* lGravityTag = FindTag(_T("behavior"), 3, 0);
	if (lGravityTag)
	{
		Vector3DF lGravity(lGravityTag->mFloatValueList[0], lGravityTag->mFloatValueList[1], lGravityTag->mFloatValueList[2]);
		mManager->GetGameManager()->GetPhysicsManager()->SetGravity(lGravity);
	}
	else
	{
		deb_assert(false);
	}
}

Cure::ContextPath* Level::QueryPath()
{
	if (!mPath)
	{
		mPath = new Cure::ContextPath(GetResourceManager(), _T("ContextPath"));
		GetManager()->AddLocalObject(mPath);
	}
	return mPath;
}

float Level::GetLevelSpeed() const
{
	return mLevelSpeed;
}



}
