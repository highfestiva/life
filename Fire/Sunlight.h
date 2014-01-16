
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/HiResTimer.h"
#include "../UiTBC/Include/UiRenderer.h"
#include "Fire.h"



namespace UiCure
{
class GameUiManager;
}



namespace Fire
{



class Sunlight
{
public:
	Sunlight(UiCure::GameUiManager* pUiManager);
	virtual ~Sunlight();

private:
	UiCure::GameUiManager* mUiManager;
	UiTbc::Renderer::LightID mLightId;
};



}
