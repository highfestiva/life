
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiTbc/Include/UiRenderer.h"
#include "../Life.h"



namespace Life
{



class Sunlight
{
public:
	Sunlight(UiTbc::Renderer* pRenderer);
	virtual ~Sunlight();

	void Tick(float pFactor);
	const Vector3DF& GetDirection() const;
	void SetDirection(float x, float y, float z);
	void SetColor(float r, float g, float b);

private:
	UiTbc::Renderer* mRenderer;
	UiTbc::Renderer::LightID mLightId;
	Vector3DF mCamSunDirection;
	HiResTimer mTime;
	float mAngle;
};



}
