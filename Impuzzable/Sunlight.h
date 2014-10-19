
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/HiResTimer.h"
#include "../UiTbc/Include/UiRenderer.h"
#include "Impuzzable.h"



namespace UiCure
{
class GameUiManager;
}



namespace Impuzzable
{



class Sunlight
{
public:
	Sunlight(UiCure::GameUiManager* pUiManager);
	virtual ~Sunlight();
	void Tick(const quat& pCameraOrientation);

private:
	UiCure::GameUiManager* mUiManager;
	UiTbc::Renderer::LightID mLightId;
    vec3 mLightAverageDirection;
};



}
