
// Author: Jonas Bystrm
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uilepra.h"
#ifdef LEPRA_IOS
#include "uimacopengldisplay.h"
#include "../uiinput.h"



namespace uilepra {



class IosInputDevice;



class IosInputElement: public InputElement {
	typedef InputElement Parent;
public:
	IosInputElement(InputElement::Type type, Interpretation interpretation, int type_index,
		IosInputDevice* parent_device);
	virtual ~IosInputElement();

private:
	enum {
		kMaxInt = 0x7FFFFFFF,
		kMinInt  = 0x80000000,
	};

	logclass();
};



class IosInputDevice: public InputDevice {
public:
	IosInputDevice(InputManager* manager);
	virtual ~IosInputDevice();

	virtual void Activate(bool disable_win_mgr);
	virtual void Release();

	virtual void PollEvents();

	void AddElement(InputElement* element);

protected:
private:
	int rel_axis_count_;
	int abs_axis_count_;
	int analogue_count_;
	int button_count_;

	logclass();
};



class IosInputManager: public InputManager, public MacObserver {
	typedef InputManager Parent;
public:
	// Declared as friend in order to get access to screen width and height.
	friend class IosInputElement;

	IosInputManager(MacOpenGLDisplay* display_manager);
	virtual ~IosInputManager();

	bool IsInitialized();
	virtual void Refresh();

	MacDisplayManager* GetDisplayManager() const;

	virtual void ActivateKeyboard();
	virtual void ReleaseKeyboard();

	virtual const InputDevice* GetKeyboard() const;
	virtual InputDevice* GetKeyboard();

	// Returns the mouse device, if there is one... NULL otherwise.
	virtual const InputDevice* GetMouse() const;
	virtual InputDevice* GetMouse();

	virtual void SetCursorVisible(bool visible);

	virtual float GetCursorX();
	virtual float GetCursorY();
	void SetMousePosition(int x, int y);

private:
	virtual void OnEvent(LEPRA_APPLE_EVENT* e);

	void AddObserver();
	void RemoveObserver();

	MacOpenGLDisplay* display_manager_;

	bool initialized_;

	// The entire display area (not just the user's window).
	int screen_width_;
	int screen_height_;

	// Mouse related stuff.
	float cursor_x_;
	float cursor_y_;

	// Default devices.
	InputDevice* keyboard_;
	InputDevice* mouse_;
	int type_count_[InputDevice::kTypeCount];

	logclass();
};



}

#endif // !iOS
