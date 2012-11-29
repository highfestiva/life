
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

	TouchstickInputDevice(InputManager* pManager, InputMode pMode, const PixelRect& pArea);
	virtual ~TouchstickInputDevice();

	void SetTap(const PixelCoord& pCoord, bool pIsPress);
	void GetValue(float& x, float& y, bool& pIsPressing);

private:
	bool IncludesCoord(const PixelCoord& pCoord) const;

	virtual void Activate();
	virtual void Release();
	virtual void PollEvents();

	InputMode mMode;
	PixelRect mArea;
	PixelCoord mStart;
	PixelCoord mLast;
	bool mIsPressing;
	/*int mMovedDistance;
	HiResTimer mPressedTimer;
	HiResTimer mReleasedTimer;*/

	LOG_CLASS_DECLARE();
};



}
}
