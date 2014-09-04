
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life.h"
#include "ExplodingMachine.h"



namespace Life
{



class Launcher;



class Mine: public ExplodingMachine
{
public:
	typedef ExplodingMachine Parent;

	Mine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher);
	virtual ~Mine();

	void EnableDeleteDetonation(bool pEnable);
	virtual void OnDie();

private:
	bool mEnableDeleteDetonation;

	logclass();
};



}
