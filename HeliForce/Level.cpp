
// Author: Jonas Byström
// Copyright (c) 2002-2013, Righteous Games



#include "Level.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"



namespace HeliForce
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

	const TBC::ChunkyClass::Tag* lTag = FindTag(_T("textures"), 0, 1);
	assert(lTag);
	mBackgroundName = lTag->mStringValueList[0];
}

const str& Level::GetBackgroundName() const
{
	return mBackgroundName;
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



}
