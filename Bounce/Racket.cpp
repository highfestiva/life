
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Racket.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "Game.h"



namespace Bounce
{



Racket::Racket(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mPath(0)
{
}

Racket::~Racket()
{
	mPath = 0;
}



Cure::ContextPath* Racket::QueryPath()
{
	if (!mPath)
	{
		mPath = new Cure::ContextPath(GetResourceManager(), "ContextPath");
		GetManager()->AddLocalObject(mPath);
	}
	return mPath;
}



loginstance(GAME_CONTEXT_CPP, Racket);



}
