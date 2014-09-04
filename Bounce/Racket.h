
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Game.h"



namespace Cure
{
class ContextPath;
}



namespace Bounce
{



class Racket: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Racket(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Racket();

	Cure::ContextPath* QueryPath();

private:
	Cure::ContextPath* mPath;

	logclass();
};



}
