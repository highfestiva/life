
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "pch.h"
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

	const Tbc::ChunkyClass::Tag* lSpeedTag = FindTag("driver", 1, 0);
	if (lSpeedTag)
	{
		mLevelSpeed = lSpeedTag->mFloatValueList[0];
	}
	const Tbc::ChunkyClass::Tag* lGravityTag = FindTag("behavior", 3, 0);
	if (lGravityTag)
	{
		vec3 lGravity(lGravityTag->mFloatValueList[0], lGravityTag->mFloatValueList[1], lGravityTag->mFloatValueList[2]);
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
		mPath = new Cure::ContextPath(GetResourceManager(), "ContextPath");
		GetManager()->AddLocalObject(mPath);
	}
	return mPath;
}

float Level::GetLevelSpeed() const
{
	return mLevelSpeed;
}



}
