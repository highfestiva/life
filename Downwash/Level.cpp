
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Level.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"



namespace Downwash
{



Level::Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Cure::ContextForceListener* pGravelEmitter):
	Parent(pResourceManager, pClassId, pUiManager, pGravelEmitter),
	mPath(0)
{
}

Level::~Level()
{
}

void Level::OnLoaded()
{
	Parent::OnLoaded();

	const Tbc::ChunkyClass::Tag* lTag = FindTag("textures", 0, 1);
	if (lTag)
	{
		mBackgroundName = lTag->mStringValueList[0];
	}
}

const str& Level::GetBackgroundName() const
{
	return mBackgroundName;
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



}
