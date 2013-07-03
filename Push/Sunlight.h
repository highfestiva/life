
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/HiResTimer.h"
#include "../UiTBC/Include/UiRenderer.h"
#include "Push.h"



namespace UiCure
{
class GameUiManager;
}



namespace Push
{



class Sunlight
{
public:
	Sunlight(UiCure::GameUiManager* pUiManager);
	virtual ~Sunlight();

	void Tick(float pFactor);
	const Vector3DF& GetDirection() const;
	void AddSunColor(Vector3DF& pBaseColor, float pFactor) const;
	float GetTimeOfDay() const;	// Value from 0->1. 0 is 9 am.

	void SetDirection(const Vector3DF& pDirection);
	void SetColor(const Vector3DF& pColor);

private:
	UiCure::GameUiManager* mUiManager;
	UiTbc::Renderer::LightID mLightId;
	Vector3DF mCamSunDirection;
	HiResTimer mTime;
	float mAngle;
};



}
