
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Level.h"



namespace Life
{



Level::Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager)
{
}

Level::~Level()
{
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Level);



}
