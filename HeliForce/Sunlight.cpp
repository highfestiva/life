
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Sunlight.h"
#include "../UiCure/Include/UiGameUiManager.h"



namespace HeliForce
{



Sunlight::Sunlight(UiCure::GameUiManager* pUiManager):
	mUiManager(pUiManager)
{
	const bool lPixelShadersEnabled = mUiManager->GetRenderer()->IsPixelShadersEnabled();
	mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
		UiTbc::Renderer::LIGHT_MOVABLE, Vector3DF(-0.2f, +0.1f, -1),
		Vector3DF(1,1,1) * (lPixelShadersEnabled? 1.0f : 1.5f), 60);
}

Sunlight::~Sunlight()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);
}



}
