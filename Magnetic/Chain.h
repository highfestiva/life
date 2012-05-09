
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Game.h"



namespace Magnetic
{



class Chain: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Chain(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Chain();

private:
	LOG_CLASS_DECLARE();
};



}
