
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Sunlight.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiRuntimeVariableName.h"



namespace Life
{



Sunlight::Sunlight(UiCure::GameUiManager* pUiManager):
	mUiManager(pUiManager),
	mAngle(0)
{
	mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
		UiTbc::Renderer::LIGHT_MOVABLE, Vector3DF(0, 0.5f, -1),
		Color::Color(255, 255, 255), 1.5f, 60);
}

Sunlight::~Sunlight()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);
}



void Sunlight::Tick(float pFactor)
{
	mAngle += (float)mTime.PopTimeDiff() * pFactor * 0.01f;	// TODO: use global game time, not this local-start-relative shit.
	if (mAngle > 2*PIF)
	{
		mAngle -= 2*PIF;
	}
	// "Physical" and "light" height actual differ, otherwise sun would never
	// be seen in the camera, since it would be to far up in the sky.
	const float lLightSunHeight = sin(mAngle)*1.2f + 1.6f;
	mCamSunDirection.x = sin(mAngle);
	mCamSunDirection.y = cos(mAngle);
	mCamSunDirection.z = -lLightSunHeight;
	SetDirection(mCamSunDirection.x, mCamSunDirection.y, mCamSunDirection.z);
	const float r = 1.5f;
	const float g = r * (sin(mAngle)*0.05f + 0.95f);
	const float b = r * (sin(mAngle)*0.1f + 0.9f);
	mUiManager->GetRenderer()->SetLightColor(mLightId, r, g, b);
	const double lAmbientFactor = sin(mAngle)*0.5+0.5;
	CURE_RTVAR_INTERNAL(mUiManager->GetVariableScope(), RTVAR_UI_3D_AMBIENTREDFACTOR, lAmbientFactor);
	CURE_RTVAR_INTERNAL(mUiManager->GetVariableScope(), RTVAR_UI_3D_AMBIENTGREENFACTOR, lAmbientFactor);
	CURE_RTVAR_INTERNAL(mUiManager->GetVariableScope(), RTVAR_UI_3D_AMBIENTBLUEFACTOR, lAmbientFactor);
}

const Vector3DF& Sunlight::GetDirection() const
{
	return mCamSunDirection;
}

void Sunlight::AddSunColor(Vector3DF& pBaseColor, float pFactor) const
{
	float lColorCurve = sin(mAngle)*0.3f*pFactor + 1;
	lColorCurve *= lColorCurve;
	pBaseColor.x = Math::Clamp(pBaseColor.x * lColorCurve, 0.0f, 1.0f);
	pBaseColor.y = Math::Clamp(pBaseColor.y * lColorCurve, 0.0f, 1.0f);
	pBaseColor.z = Math::Clamp(pBaseColor.z * lColorCurve, 0.0f, 1.0f);
}

float Sunlight::GetTimeOfDay() const
{
	return mAngle / PIF * 2;
}

void Sunlight::SetDirection(float x, float y, float z)
{
	if (mUiManager->CanRender())
	{
		mUiManager->GetRenderer()->SetLightDirection(mLightId, x, y, z);
	}
}

void Sunlight::SetColor(float r, float g, float b)
{
	mUiManager->GetRenderer()->SetLightColor(mLightId, r, g, b);
}



}
