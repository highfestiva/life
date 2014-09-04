
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/HiResTimer.h"
#include "../UiTbc/Include/UiRenderer.h"
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
	const vec3& GetDirection() const;
	void AddSunColor(vec3& pBaseColor, float pFactor) const;
	float GetTimeOfDay() const;	// Value from 0->1. 0 is 9 am.

	void SetDirection(const vec3& pDirection);
	void SetColor(const vec3& pColor);

private:
	UiCure::GameUiManager* mUiManager;
	UiTbc::Renderer::LightID mLightId;
	vec3 mCamSunDirection;
	HiResTimer mTime;
	float mAngle;
};



}
