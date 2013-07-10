
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Game.h"



namespace Bounce
{



class Ball: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Ball(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Ball();

private:
	virtual void OnMicroTick(float pFrameTime);
	virtual void OnLoaded();

	LOG_CLASS_DECLARE();
};



}
