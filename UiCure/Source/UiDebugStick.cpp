
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "../../UiLepra/Include/UiTouchStick.h"
#include "../../UiTbc/Include/UiPainter.h"
#include "../Include/UiDebugStick.h"
#include "../Include/UiGameUiManager.h"

#define STICK_DIAMETER	20



namespace UiCure
{



void DebugStick::Init(GameUiManager* pInputManager)
{
	if (!mUiManager)
	{
		mUiManager = pInputManager;
		::memset(mTouchSticks, 0, sizeof(mTouchSticks));
		::memset(mPreviousValues, 0, sizeof(mPreviousValues));
		::memset(mIsDefaultValueSet, 0, sizeof(mIsDefaultValueSet));
	}
	mIsUpdated = false;
}

void DebugStick::Draw()
{
	for (int y = 0; y < STICK_RESOLUTION; ++y)
	{
		for (int x = 0; x < STICK_RESOLUTION; ++x)
		{
			if (mTouchSticks[y][x])
			{
				mUiManager->GetPainter()->SetColor(WHITE);
				PixelRect lRect = mTouchSticks[y][x]->GetArea();
				mUiManager->GetPainter()->DrawRect(lRect, 1);
				mUiManager->GetPainter()->SetColor(RED);
				float tx;
				float ty;
				bool lIsPressing;
				mTouchSticks[y][x]->GetValue(tx, ty, lIsPressing);
				int ix = (int)(tx*(lRect.GetWidth()/2-STICK_DIAMETER/2)+lRect.GetCenterX()) - STICK_DIAMETER/2;
				int iy = (int)(ty*(lRect.GetHeight()/2-STICK_DIAMETER/2)+lRect.GetCenterY()) - STICK_DIAMETER/2;
				mUiManager->GetPainter()->DrawArc(ix, iy, STICK_DIAMETER, STICK_DIAMETER, 0, 360, lIsPressing);
			}
		}
	}
}

bool DebugStick::IsUpdated()
{
	return mIsUpdated;
}



DebugStick::DebugStick():
	mStickX(-1),
	mStickY(-1)
{
}

DebugStick::DebugStick(int pPosX, int pPosY)
{
	Place(pPosX, pPosY);
}

void DebugStick::Place(int pPosX, int pPosY)
{
	deb_assert(mUiManager);
	deb_assert(pPosX >= 0 && pPosX < STICK_RESOLUTION);
	deb_assert(pPosY >= 0 && pPosY < STICK_RESOLUTION);
	mStickX = pPosX;
	mStickY = pPosY;
	InitStick();

	float x;
	float y;
	bool lIsPressing;
	mTouchSticks[mStickY][mStickX]->GetValue(x, y, lIsPressing);
	mValue = mPreviousValues[mStickY][mStickX];
	if (lIsPressing && (!Math::IsEpsEqual(x, mValue.x) || !Math::IsEpsEqual(y, mValue.y)))
	{
		mValue.Set(x, y);
		mPreviousValues[mStickY][mStickX] = mValue;
		mIsUpdated = true;
	}
}

void DebugStick::SetDefaultValue(float x, float y)
{
	if (!mIsDefaultValueSet[mStickY][mStickX])
	{
		mIsDefaultValueSet[mStickY][mStickX] = true;
		mTouchSticks[mStickY][mStickX]->SetValue(x, y);
		mValue.Set(x, y);
		mPreviousValues[mStickY][mStickX] = mValue;
		mIsUpdated = true;
	}
}

void DebugStick::InitStick()
{
	if (mTouchSticks[mStickY][mStickX])
	{
		return;
	}
	int w = mUiManager->GetCanvas()->GetWidth() / STICK_RESOLUTION;
	int h = mUiManager->GetCanvas()->GetHeight() / STICK_RESOLUTION;
	PixelRect lRect(w*mStickX, h*mStickY, w*(mStickX+1), h*(mStickY+1));
	mTouchSticks[mStickY][mStickX] = new UiLepra::Touch::TouchstickInputDevice(mUiManager->GetInputManager(), UiLepra::Touch::TouchstickInputDevice::MODE_RELATIVE_CENTER_NOSPRING, lRect, 0, STICK_DIAMETER/2);
}


GameUiManager* DebugStick::mUiManager = 0;
UiLepra::Touch::TouchstickInputDevice* DebugStick::mTouchSticks[STICK_RESOLUTION][STICK_RESOLUTION];
Vector2DF DebugStick::mPreviousValues[STICK_RESOLUTION][STICK_RESOLUTION];
bool DebugStick::mIsDefaultValueSet[STICK_RESOLUTION][STICK_RESOLUTION];
bool DebugStick::mIsUpdated = false;



}
