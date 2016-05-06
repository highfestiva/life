
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uiinput.h"
#include "../../lepra/include/graphics2d.h"
#include "uilepra.h"



namespace uilepra {
namespace touch {


class TouchstickInputDevice;



class TouchstickInputElement: public InputElement {
	typedef InputElement Parent;
public:
	TouchstickInputElement(InputElement::Type type, Interpretation interpretation, int type_index, TouchstickInputDevice* parent_device);
	virtual ~TouchstickInputElement();

	float GetValueScaled() const;
	void SetValueScaled(float value);
	void SetScale(float minimum, float maximum);

private:
	float scale_;
	float offset_;
	logclass();
};



class TouchstickInputDevice: public InputDevice {
	typedef InputDevice Parent;
public:
	enum InputMode {
		kModeRelativeCenter = 0,
		kModeRelativeStart,
		kModeRelativeLast,
		kModeRelativeCenterNospring,	// Doesn't go back to center when releasing.
		kModeCount
	};

	static TouchstickInputDevice* GetByCoordinate(InputManager* manager, const PixelCoord& coord);

	TouchstickInputDevice(InputManager* manager, InputMode mode, const PixelRect& area, int angle, int finger_radius);
	virtual ~TouchstickInputDevice();
	bool IsOwnedByManager() const;

	void Move(const PixelRect& area, int angle);
	const PixelRect& GetArea() const;
	int GetFingerRadius() const;
	void SetFingerRadius(int finger_radius);

	void ResetTap();
	void SetTap(const PixelCoord& coord, bool is_press);
	void GetValue(float& x, float& y, bool& is_pressing);
	void SetValue(float x, float y);
	void SetValueScale(float min_x, float max_x, float min_y, float max_y);

private:
	void AddElement(InputElement* element);

	bool IncludesCoord(const PixelCoord& coord) const;

	virtual void Activate();
	virtual void Release();
	virtual void PollEvents();

	InputMode mode_;
	PixelRect area_;
	PixelRect touch_area_;
	PixelCoord start_;
	bool is_pressing_;
	int angle_;
	int finger_radius_;
	/*int moved_distance_;
	HiResTimer mPressedTimer;
	HiResTimer mReleasedTimer;*/

	logclass();
};



}
}
