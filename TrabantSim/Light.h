
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/HiResTimer.h"
#include "../UiTbc/Include/UiRenderer.h"
#include "TrabantSim.h"



namespace UiCure
{
class GameUiManager;
}



namespace TrabantSim
{



class Light
{
public:
	Light(UiCure::GameUiManager* pUiManager);
	virtual ~Light();
	void Tick(const quat& pCameraOrientation);

private:
	UiCure::GameUiManager* mUiManager;
	UiTbc::Renderer::LightID mLightId;
    vec3 mLightAverageDirection;
};



}
