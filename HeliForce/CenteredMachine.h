
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/ExplodingMachine.h"
#include "HeliForce.h"



namespace HeliForce
{



class CenteredMachine: public Life::ExplodingMachine
{
	typedef Life::ExplodingMachine Parent;
public:
	CenteredMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher);
	virtual ~CenteredMachine();

protected:
	virtual bool TryComplete();
	virtual void OnMicroTick(float pFrameTime);
	virtual void OnDie();
};



}
