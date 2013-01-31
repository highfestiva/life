
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "BombPlane.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/FloatAttribute.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Life/Launcher.h"
#include "../../Life/ProjectileUtil.h"



namespace Push
{



BombPlane::BombPlane(Cure::ResourceManager* pResourceManager, const str& pClassId, const Vector3DF& pTarget):
	Parent(pResourceManager, pClassId),
	mTarget(pTarget)
{
}

BombPlane::~BombPlane()
{
}



void BombPlane::OnLoaded()
{
	SetEnginePower(0, 1.0f);	// Jet.
	SetEnginePower(1, 1.0f);	// Hoover.

	Parent::OnLoaded();
}

void BombPlane::OnTick()
{
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, BombPlane);



}
