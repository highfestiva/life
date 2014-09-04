
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Level.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "Game.h"



namespace TireFire
{



Level::Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mPath(0)
{
}

Level::~Level()
{
	mPath = 0;
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



loginstance(GAME_CONTEXT_CPP, Level);



}
