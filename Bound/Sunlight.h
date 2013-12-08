
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/HiResTimer.h"
#include "../UiTBC/Include/UiRenderer.h"
#include "Bound.h"



namespace UiCure
{
class GameUiManager;
}



namespace Bound
{



class Sunlight
{
public:
	Sunlight(UiCure::GameUiManager* pUiManager);
	virtual ~Sunlight();
	void Tick(const QuaternionF& pCameraOrientation);

private:
	UiCure::GameUiManager* mUiManager;
	UiTbc::Renderer::LightID mLightId;
    Vector3DF mLightAverageDirection;
};



}
