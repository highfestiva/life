
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiTouchstick.h"



namespace UiLepra
{
namespace Touch
{



using namespace Lepra;



TouchstickInputElement::TouchstickInputElement(Type pType, Interpretation pInterpretation, int pTypeIndex, TouchstickInputDevice* pParentDevice):
	InputElement(pType, pInterpretation, pTypeIndex, pParentDevice),
	mScale(1),
	mOffset(0)
{
}

TouchstickInputElement::~TouchstickInputElement()
{
}

float TouchstickInputElement::GetValueScaled() const
{
	return (GetValue()-mOffset) / mScale;
}

void TouchstickInputElement::SetValueScaled(float pValue)
{
	SetValue(pValue*mScale + mOffset);
}

void TouchstickInputElement::SetScale(float pMinimum, float pMaximum)
{
	mOffset = (pMaximum+pMinimum) / 2;
	mScale  = (pMaximum-pMinimum) / 2;	// Default scale is one, between -1 and +1.
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

TouchstickInputDevice::TouchstickInputDevice(InputManager* pManager, InputMode pMode, const PixelRect& pArea, int pAngle, int pFingerRadius):
	Parent(pManager),
	mMode(pMode),
	mArea(pArea),
	mTouchArea(pArea),
	mAngle(pAngle),
	mFingerRadius(pFingerRadius)
{
	mTouchArea.Shrink(mFingerRadius);
	if (mAngle < -45)
	{
		mAngle += 360;
	}

	TouchstickInputElement* lButton = new TouchstickInputElement(InputElement::DIGITAL, InputElement::BUTTON, 0, this);
	TouchstickInputElement* x = new TouchstickInputElement(InputElement::ANALOGUE, InputElement::ABSOLUTE_AXIS, 0, this);
	TouchstickInputElement* y = new TouchstickInputElement(InputElement::ANALOGUE, InputElement::ABSOLUTE_AXIS, 1, this);
	SetIdentifier("Touchstick");
	lButton->SetIdentifier("Button");
	x->SetIdentifier("AxisX");
	y->SetIdentifier("AxisY");
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



void TouchstickInputDevice::Move(const PixelRect& pArea, int pAngle)
{
	mArea = pArea;
	mTouchArea = pArea;
	mTouchArea.Shrink(mFingerRadius);
	mAngle = pAngle;
	if (mAngle < -45)
	{
		mAngle += 360;
	}
}

const PixelRect& TouchstickInputDevice::GetArea() const
{
	return mArea;
}

int TouchstickInputDevice::GetFingerRadius() const
{
	return mFingerRadius;
}

void TouchstickInputDevice::SetFingerRadius(int pFingerRadius)
{
	mFingerRadius = pFingerRadius;
}

void TouchstickInputDevice::ResetTap()
{
	mElementArray[0]->SetValue(0.0f);
	mElementArray[1]->SetValue(0.0f);
	mElementArray[2]->SetValue(0.0f);
}

void TouchstickInputDevice::SetTap(const PixelCoord& pCoord, bool pIsPress)
{
	if (mIsPressing)
	{
		log_volatile(mLog.Debugf("Movin': (%i; %i)", pCoord.x, pCoord.y));
	}
	else
	{
		mStart = pCoord;
	}
	mIsPressing = pIsPress;

	const float dx = mTouchArea.GetWidth()*0.5f;
	const float dy = mTouchArea.GetHeight()*0.5f;
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
		ry = -ry;
		std::swap(rx, ry);
	}
	else if (mAngle < 225)
	{
		rx = -rx;
		ry = -ry;
	}
	else
	{
		rx = -rx;
		std::swap(rx, ry);
	}

	mElementArray[0]->SetValue(mIsPressing? 1.0f : 0.0f);
	if (mIsPressing)
	{
		((TouchstickInputElement*)mElementArray[1])->SetValueScaled(rx);
		((TouchstickInputElement*)mElementArray[2])->SetValueScaled(ry);
	}
	else if (mMode != MODE_RELATIVE_CENTER_NOSPRING)
	{
		mElementArray[1]->SetValue(0.0f);
		mElementArray[2]->SetValue(0.0f);
	}
}

void TouchstickInputDevice::GetValue(float& x, float& y, bool& pIsPressing)
{
	x = ((TouchstickInputElement*)mElementArray[1])->GetValueScaled();
	y = ((TouchstickInputElement*)mElementArray[2])->GetValueScaled();
	pIsPressing = mElementArray[0]->GetBooleanValue();

	if (mAngle < 45)
	{
	}
	else if (mAngle < 135)
	{
		x = -x;
		std::swap(x, y);
	}
	else if (mAngle < 225)
	{
		x = -x;
		y = -y;
	}
	else
	{
		y = -y;
		std::swap(x, y);
	}
}

void TouchstickInputDevice::SetValue(float x, float y)
{
	if (mAngle < 45)
	{
	}
	else if (mAngle < 135)
	{
		x = -x;
		std::swap(x, y);
	}
	else if (mAngle < 225)
	{
		x = -x;
		y = -y;
	}
	else
	{
		y = -y;
		std::swap(x, y);
	}
	((TouchstickInputElement*)mElementArray[1])->SetValueScaled(x);
	((TouchstickInputElement*)mElementArray[2])->SetValueScaled(y);
}

void TouchstickInputDevice::SetValueScale(float pMinX, float pMaxX, float pMinY, float pMaxY)
{
	((TouchstickInputElement*)mElementArray[1])->SetScale(pMinX, pMaxX);
	((TouchstickInputElement*)mElementArray[2])->SetScale(pMinY, pMaxY);
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



loginstance(UI_INPUT, TouchstickInputDevice);



}
}
