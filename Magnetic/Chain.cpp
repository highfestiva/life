
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Chain.h"
//#include "../Cure/Include/ContextManager.h"
//#include "../Cure/Include/ContextPath.h"



namespace Magnetic
{



Chain::Chain(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager)
{
}

Chain::~Chain()
{
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Chain);



}
