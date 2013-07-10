
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiInput.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "UiLepra.h"



namespace UiLepra
{
namespace Touch
{


class TouchstickInputDevice;



class TouchstickInputElement: public InputElement
{
	typedef InputElement Parent;
public:
	TouchstickInputElement(InputElement::Type pType, Interpretation pInterpretation, int pTypeIndex, TouchstickInputDevice* pParentDevice);
	virtual ~TouchstickInputElement();

private:
	LOG_CLASS_DECLARE();
};



class TouchstickInputDevice: public InputDevice
{
	typedef InputDevice Parent;
public:
	enum InputMode
	{
		MODE_RELATIVE_CENTER = 0,
		MODE_RELATIVE_START,
		MODE_RELATIVE_LAST,
		MODE_COUNT
	};

	static TouchstickInputDevice* GetByCoordinate(InputManager* pManager, const PixelCoord& pCoord);

	TouchstickInputDevice(InputManager* pManager, InputMode pMode, const PixelRect& pArea, int pAngle, int pFingerRadius);
	virtual ~TouchstickInputDevice();
	bool IsOwnedByManager() const;

	void Move(const PixelRect& pArea, int pAngle);
	const PixelRect& GetArea() const;
	int GetFingerRadius() const;

	void ResetTap();
	void SetTap(const PixelCoord& pCoord, bool pIsPress);
	void GetValue(float& x, float& y, bool& pIsPressing);

private:
	void AddElement(InputElement* pElement);

	bool IncludesCoord(const PixelCoord& pCoord) const;

	virtual void Activate();
	virtual void Release();
	virtual void PollEvents();

	InputMode mMode;
	PixelRect mArea;
	PixelRect mTouchArea;
	PixelCoord mStart;
	bool mIsPressing;
	int mAngle;
	int mFingerRadius;
	/*int mMovedDistance;
	HiResTimer mPressedTimer;
	HiResTimer mReleasedTimer;*/

	LOG_CLASS_DECLARE();
};



}
}
