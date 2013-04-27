
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../UiCure/Include/UiMachine.h"
#include "../Life.h"



namespace Life
{



class Launcher;



class ExplodingMachine: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	ExplodingMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher);
	virtual ~ExplodingMachine();

protected:
	void OnTick();

private:
	Launcher* mLauncher;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
