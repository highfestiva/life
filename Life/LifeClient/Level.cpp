
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Level.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "GameClientSlaveManager.h"
#include "Props.h"
#include "RtVar.h"



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
