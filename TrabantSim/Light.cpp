
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Light.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiRuntimeVariableName.h"
#include "RtVar.h"



namespace TrabantSim
{



Light::Light(UiCure::GameUiManager* pUiManager):
	mUiManager(pUiManager),
	mLightAverageDirection(-0.2f,1,-1)
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
	if (!mUiManager->CanRender())
	{
		return;
	}
	float ax,az;
	v_get(ax, =(float), mUiManager->GetVariableScope(), RTVAR_UI_3D_LIGHTANGLEX, -1.4);
	v_get(az, =(float), mUiManager->GetVariableScope(), RTVAR_UI_3D_LIGHTANGLEZ, 0.1);
	quat q;
	q.RotateAroundOwnX(ax);
	q.RotateAroundOwnZ(az);
	vec3 d = q*vec3(0,1,0);
	d = Math::Lerp(mLightAverageDirection, d, 0.5f);
	if (d.GetDistanceSquared(mLightAverageDirection) > 1e-5f)
	{
		mLightAverageDirection = d;
		mUiManager->GetRenderer()->SetLightDirection(mLightId, mLightAverageDirection);
	}
}



}
