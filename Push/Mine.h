
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Push.h"



namespace Life
{
class Launcher;
}



namespace Push
{



class Mine: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Mine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher);
	virtual ~Mine();

private:
	Life::Launcher* mLauncher;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
