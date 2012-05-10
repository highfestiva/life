
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Ball.h"
//#include "../Cure/Include/ContextManager.h"
//#include "../Cure/Include/ContextPath.h"



namespace Magnetic
{



Ball::Ball(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager)
{
}

Ball::~Ball()
{
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Ball);



}
