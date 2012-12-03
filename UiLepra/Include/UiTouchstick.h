
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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

	// Sets the uncalibrated value of this device.
	// (Only useful with analogue elements).
	void SetValue(int pValue);

	str GetCalibration() const;
	bool SetCalibration(const str& pData);

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

	TouchstickInputDevice(InputManager* pManager, InputMode pMode, const PixelRect& pArea, int pAngle);
	virtual ~TouchstickInputDevice();
	bool IsOwnedByManager() const;

	void Move(const PixelRect& pArea, int pAngle);
	const PixelRect& GetArea() const;

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
	PixelCoord mStart;
	bool mIsPressing;
	int mAngle;
	/*int mMovedDistance;
	HiResTimer mPressedTimer;
	HiResTimer mReleasedTimer;*/

	LOG_CLASS_DECLARE();
};



}
}
