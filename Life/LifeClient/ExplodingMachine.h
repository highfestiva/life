
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



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
	virtual void OnTick();
	virtual void OnDie();

	Launcher* mLauncher;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}