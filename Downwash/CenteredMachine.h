
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "BaseMachine.h"
#include "Downwash.h"



namespace Downwash
{



class CenteredMachine: public BaseMachine
{
	typedef BaseMachine Parent;
public:
	CenteredMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher);
	virtual ~CenteredMachine();

protected:
	virtual bool TryComplete();
	virtual void OnMicroTick(float pFrameTime);

	logclass();
};



}
