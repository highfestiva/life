
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Light.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiRuntimeVariableName.h"



namespace Tv3d
{



Light::Light(UiCure::GameUiManager* pUiManager):
	mUiManager(pUiManager),
	mLightAverageDirection(-0.01f,0.05f,-1)
{
	const bool lPixelShadersEnabled = mUiManager->GetRenderer()->IsPixelShadersEnabled();
	mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
		UiTbc::Renderer::LIGHT_MOVABLE, mLightAverageDirection,
		vec3(0.6f,0.6f,0.6f) * (lPixelShadersEnabled? 1.0f : 1.5f), 100);
}

Light::~Light()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);
}

void Light::Tick(const quat& pCameraOrientation)
{
	(void)pCameraOrientation;
	/*if (!mUiManager->CanRender())
	{
		return;
	}
	vec3 d = mUiManager->GetAccelerometer();
	d = pCameraOrientation*d.GetNormalized();
	d = Math::Lerp(mLightAverageDirection, d, 0.5f);
	if (d.GetDistanceSquared(mLightAverageDirection) > 1e-5f)
	{
		mLightAverageDirection = d;
		mUiManager->GetRenderer()->SetLightDirection(mLightId, mLightAverageDirection);
	}*/
}



}
