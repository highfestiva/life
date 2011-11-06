
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Cutie.h"
#include "../Lepra/Include/Math.h"
/*#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/TimeManager.h"
#include "../TBC/Include/ChunkyBoneGeometry.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiProps.h"
#include "Game.h"*/



namespace GrenadeRun
{



Cutie::Cutie(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mHealth(1)
{
}

Cutie::~Cutie()
{
}



void Cutie::DrainHealth(float pDrain)
{
	mHealth = Math::Clamp(mHealth-pDrain, 0.0f, 1.0f);
}

float Cutie::GetHealth() const
{
	return mHealth;
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Cutie);



}
