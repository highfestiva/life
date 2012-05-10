
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Racket.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "Game.h"



namespace Magnetic
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
		mPath = new Cure::ContextPath(GetResourceManager(), _T("ContextPath"));
		GetManager()->AddLocalObject(mPath);
	}
	return mPath;
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Racket);



}