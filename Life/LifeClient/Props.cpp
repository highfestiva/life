
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Props.h"



namespace Life
{



Props::Props(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager)
{
	SetPhysicsTypeOverride(PHYSICS_OVERRIDE_BONES);
}

Props::~Props()
{
}



void Props::OnPhysicsTick()
{
	Parent::Parent::OnPhysicsTick();
}



}
