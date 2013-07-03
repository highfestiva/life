
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../UiCure/Include/UiMachine.h"
#include "../Life.h"



namespace Life
{



class Launcher;



class Mine: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Mine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher);
	virtual ~Mine();

private:
	Launcher* mLauncher;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
