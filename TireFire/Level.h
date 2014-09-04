
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Game.h"



namespace Cure
{
class ContextPath;
}



namespace TireFire
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

	logclass();
};



}
