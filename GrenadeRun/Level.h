
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Game.h"



namespace Cure
{
class ContextPath;
}



namespace GrenadeRun
{



class Level: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Level();

	Cure::ContextPath* QueryPath();

private:
	Cure::ContextPath* mPath;

	LOG_CLASS_DECLARE();
};



}
