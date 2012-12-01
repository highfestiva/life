
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiTouchstick.h"



namespace UiLepra
{
namespace Touch
{



using namespace Lepra;



TouchstickInputElement::TouchstickInputElement(Type pType, Interpretation pInterpretation, int pTypeIndex, TouchstickInputDevice* pParentDevice):
	InputElement(pType, pInterpretation, pTypeIndex, pParentDevice)
{
}

TouchstickInputElement::~TouchstickInputElement()
{
}

void TouchstickInputElement::SetValue(int)
{
}

str TouchstickInputElement::GetCalibration() const
{
	return str();
}

bool TouchstickInputElement::SetCalibration(const str&)
{
	return true;
}



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

	TouchstickInputElement* lButton = new TouchstickInputElement(InputElement::DIGITAL, InputElement::BUTTON1, 0, this);
	TouchstickInputElement* x = new TouchstickInputElement(InputElement::ANALOGUE, InputElement::ABSOLUTE_AXIS, 0, this);
	TouchstickInputElement* y = new TouchstickInputElement(InputElement::ANALOGUE, InputElement::ABSOLUTE_AXIS, 1, this);
	SetIdentifier(_T("Touchstick"));
	lButton->SetIdentifier(_T("Button"));
	x->SetIdentifier(_T("AxisX"));
	y->SetIdentifier(_T("AxisY"));
	AddElement(lButton);
	AddElement(x);
	AddElement(y);

	GetManager()->AddInputDevice(this);
}

TouchstickInputDevice::~TouchstickInputDevice()
{
	GetManager()->RemoveInputDevice(this);
}

bool TouchstickInputDevice::IsOwnedByManager() const
{
	return false;
}

void TouchstickInputDevice::SetTap(const PixelCoord& pCoord, bool pIsPress)
{
	if (mIsPressing)
	{
		log_volatile(mLog.Debugf(_T("Movin': (%i; %i)"), pCoord.x, pCoord.y));
	}
	else
	{
		mStart = pCoord;
	}
	mIsPressing = pIsPress;

	const float dx = mArea.GetWidth() * 0.5f;
	const float dy = mArea.GetHeight() * 0.5f;
	float rx;
	float ry;
	switch (mMode)
	{
		default:
		case MODE_RELATIVE_CENTER:
		{
			rx = (pCoord.x - mArea.GetCenterX()) / dx;
			ry = (pCoord.y - mArea.GetCenterY()) / dy;
		}
		break;
		case MODE_RELATIVE_START:
		{
			rx = (pCoord.x - mStart.x) / dx;
			ry = (pCoord.y - mStart.y) / dy;
		}
		break;
		case MODE_RELATIVE_LAST:
		{
			rx = (pCoord.x - mStart.x) / dx;
			ry = (pCoord.y - mStart.y) / dy;
			mStart = pCoord;
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

	mElementArray[0]->SetValue(mIsPressing? 1.0f : 0.0f);
	mElementArray[1]->SetValue(mIsPressing? rx : 0.0f);
	mElementArray[2]->SetValue(mIsPressing? ry : 0.0f);
}



void TouchstickInputDevice::AddElement(InputElement* pElement)
{
	mElementArray.push_back(pElement);
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
