
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Sunlight.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiRuntimeVariableName.h"



namespace Impuzzable
{



Sunlight::Sunlight(UiCure::GameUiManager* pUiManager):
	mUiManager(pUiManager),
	mLightAverageDirection(0,0,-1)
{
	const bool lPixelShadersEnabled = mUiManager->GetRenderer()->IsPixelShadersEnabled();
	mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
		UiTbc::Renderer::LIGHT_MOVABLE, mLightAverageDirection,
		vec3(0.6f,0.6f,0.6f) * (lPixelShadersEnabled? 1.0f : 1.5f), 100);
}

Sunlight::~Sunlight()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);
}

void Sunlight::Tick(const quat& pCameraOrientation)
{
	vec3 d = mUiManager->GetAccelerometer();
	d = pCameraOrientation*d.GetNormalized();
	d = Math::Lerp(mLightAverageDirection, d, 0.5f);
	if (d.GetDistanceSquared(mLightAverageDirection) > 1e-5f)
	{
		mLightAverageDirection = d;
		mUiManager->GetRenderer()->SetLightDirection(mLightId, mLightAverageDirection);
	}
}



}
