
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Sunlight.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiRuntimeVariableName.h"



namespace Bound
{



Sunlight::Sunlight(UiCure::GameUiManager* pUiManager):
	mUiManager(pUiManager),
	mLightAverageDirection(0,0,-1)
{
	const bool lPixelShadersEnabled = mUiManager->GetRenderer()->IsPixelShadersEnabled();
	mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
		UiTbc::Renderer::LIGHT_MOVABLE, mLightAverageDirection,
		Vector3DF(0.6f,0.6f,0.6f) * (lPixelShadersEnabled? 1.0f : 1.5f), 100);
}

Sunlight::~Sunlight()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);
}

void Sunlight::Tick(const QuaternionF& pCameraOrientation)
{
	float x, y, z;
	CURE_RTVAR_GET(x, =(float), mUiManager->GetVariableScope(), RTVAR_CTRL_ACCELEROMETER_X,  0.0);
	CURE_RTVAR_GET(y, =(float), mUiManager->GetVariableScope(), RTVAR_CTRL_ACCELEROMETER_Y, -1.0);
	CURE_RTVAR_GET(z, =(float), mUiManager->GetVariableScope(), RTVAR_CTRL_ACCELEROMETER_Z,  0.0);
	Vector3DF d(x,-z,y);
	d = pCameraOrientation*d.GetNormalized();
	d = Math::Lerp(mLightAverageDirection, d, 0.5f);
	if (d.GetDistanceSquared(mLightAverageDirection) > 1e-5f)
	{
		mLightAverageDirection = d;
		mUiManager->GetRenderer()->SetLightDirection(mLightId, mLightAverageDirection);
	}
}



}
