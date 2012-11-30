
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiTouchstick.h"



namespace UiLepra
{
namespace Touch
{



using namespace Lepra;



TouchstickInputDevice* TouchstickInputDevice::GetByCoordinate(InputManager* pManager, const PixelCoord& pCoord)
{
	const InputManager::DeviceList& lList = pManager->GetDeviceList();
	InputManager::DeviceList::const_iterator x;
	for (x = lList.begin(); x != lList.end(); ++x)
	{
		TouchstickInputDevice* lStick = dynamic_cast<TouchstickInputDevice*>(*x);
		if (lStick)
		{
			if (lStick->IncludesCoord(pCoord))
			{
				return lStick;
			}
		}
	}
	return 0;
}



TouchstickInputDevice::TouchstickInputDevice(InputManager* pManager, InputMode pMode, const PixelRect& pArea, int pAngle):
	Parent(pManager),
	mMode(pMode),
	mArea(pArea),
	mAngle(pAngle)
{
	if (mAngle < -45)
	{
		mAngle += 360;
	}
	GetManager()->AddInputDevice(this);
}

TouchstickInputDevice::~TouchstickInputDevice()
{
	GetManager()->RemoveInputDevice(this);
}



void TouchstickInputDevice::SetTap(const PixelCoord& pCoord, bool pIsPress)
{
	if (mIsPressing)
	{
		mLast = pCoord;
		log_volatile(mLog.Debugf(_T("Movin': (%i; %i)"), pCoord.x, pCoord.y));
	}
	else
	{
		mStart = pCoord;
		mLast = pCoord;
	}
	mIsPressing = pIsPress;
}

void TouchstickInputDevice::GetValue(float& x, float& y, bool& pIsPressing)
{
	const float dx = mArea.GetWidth() * 0.5f;
	const float dy = mArea.GetHeight() * 0.5f;
	float rx;
	float ry;
	switch (mMode)
	{
		default:
		case MODE_RELATIVE_CENTER:
		{
			rx = (mLast.x - mArea.GetCenterX()) / dx;
			ry = (mLast.y - mArea.GetCenterY()) / dy;
		}
		break;
		case MODE_RELATIVE_START:
		{
			rx = (mLast.x - mStart.x) / dx;
			ry = (mLast.y - mStart.y) / dy;
		}
		break;
		case MODE_RELATIVE_LAST:
		{
			rx = (mLast.x - mStart.x) / dx;
			ry = (mLast.y - mStart.y) / dy;
			mStart = mLast;
		}
		break;
	}
	rx = Math::Clamp(rx, -1.0f, +1.0f);
	ry = Math::Clamp(ry, -1.0f, +1.0f);
	// Handle the angles.
	if (mAngle < 45)
	{
	}
	else if (mAngle < 135)
	{
		rx = -rx;
		std::swap(rx, ry);
	}
	else if (mAngle < 225)
	{
		rx = -rx;
		ry = -ry;
	}
	else
	{
		std::swap(rx, ry);
	}
	x = rx;
	y = ry;
	pIsPressing = mIsPressing;
}



bool TouchstickInputDevice::IncludesCoord(const PixelCoord& pCoord) const
{
	return mArea.IsInside(pCoord.x, pCoord.y);
}



void TouchstickInputDevice::Activate()
{
}

void TouchstickInputDevice::Release()
{
}

void TouchstickInputDevice::PollEvents()
{
}



LOG_CLASS_DEFINE(UI_INPUT, TouchstickInputDevice);



}
}
