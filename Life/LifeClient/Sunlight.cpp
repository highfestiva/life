
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Sunlight.h"



namespace Life
{



Sunlight::Sunlight(UiTbc::Renderer* pRenderer):
	mRenderer(pRenderer),
	mAngle(0)
{
	mLightId = pRenderer->AddDirectionalLight(
		UiTbc::Renderer::LIGHT_STATIC, Vector3DF(0, 0.5f, -1),
		Color::Color(255, 255, 255), 1.5f, 20);
}

Sunlight::~Sunlight()
{
	mRenderer->RemoveLight(mLightId);
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
	const float lPhysicalSunHeight = (sin(mAngle) + 1.4f) * 0.1f;
	const float lLightSunHeight = sin(mAngle)*1.2f + 1.6f;
	mCamSunDirection.x = -sin(mAngle);
	mCamSunDirection.y = -cos(mAngle);
	mCamSunDirection.z = lPhysicalSunHeight;
	SetDirection(-mCamSunDirection.x, -mCamSunDirection.y, -lLightSunHeight);
	const float r = 1.5f;
	const float g = r * (sin(mAngle)*0.05f + 0.95f);
	const float b = r * (sin(mAngle)*0.1f + 0.9f);
	mRenderer->SetLightColor(mLightId, r, g, b);
}

const Vector3DF& Sunlight::GetDirection() const
{
	return mCamSunDirection;
}

void Sunlight::SetDirection(float x, float y, float z)
{
	mRenderer->SetLightDirection(mLightId, x, y, z);
}

void Sunlight::SetColor(float r, float g, float b)
{
	mRenderer->SetLightColor(mLightId, r, g, b);
}



}
