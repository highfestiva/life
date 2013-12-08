
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Sunlight.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiRuntimeVariableName.h"



namespace Bound
{



Sunlight::Sunlight(UiCure::GameUiManager* pUiManager):
	mUiManager(pUiManager)
{
	const bool lPixelShadersEnabled = mUiManager->GetRenderer()->IsPixelShadersEnabled();
	mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
		UiTbc::Renderer::LIGHT_MOVABLE, Vector3DF(0, 0, -1),
		Vector3DF(0.6f,0.6f,0.6f) * (lPixelShadersEnabled? 1.0f : 1.5f), 10);
}

Sunlight::~Sunlight()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);
}

void Sunlight::Tick()
{
	float x, y, z;
	CURE_RTVAR_GET(x, =(float), mUiManager->GetVariableScope(), RTVAR_CTRL_ACCELEROMETER_X,  0.0);
	CURE_RTVAR_GET(y, =(float), mUiManager->GetVariableScope(), RTVAR_CTRL_ACCELEROMETER_Y, -9.8);
	CURE_RTVAR_GET(z, =(float), mUiManager->GetVariableScope(), RTVAR_CTRL_ACCELEROMETER_Z,  0.0);
	Vector3DF d(x,-z,y);
	d.Normalize();
	mUiManager->GetRenderer()->SetLightDirection(mLightId, d);
}



}
