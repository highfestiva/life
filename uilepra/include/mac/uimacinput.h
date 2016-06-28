
// Author: Jonas Bystrm
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uilepra.h"
#ifndef LEPRA_IOS
#include "../uiinput.h"
#import <AppKit/AppKit.h>
#include <HID_Utilities.h>
#include <HID_Queue_Utilities.h>
#import "uimacdisplaymanager.h"



namespace uilepra {



class MacInputDevice;



class MacInputElement: public InputElement {
	typedef InputElement Parent;
public:
	MacInputElement(InputElement::Type type, Interpretation interpretation, int type_index,
		MacInputDevice* parent_device, pRecElement element);
	virtual ~MacInputElement();

	pRecElement GetNativeElement() const;

private:
	enum {
		kMaxInt = 0x7FFFFFFF,
		kMinInt  = 0x80000000,
	};

	pRecElement element_;

	logclass();
};



class MacInputDevice: public InputDevice {
public:
	MacInputDevice(pRecDevice native_device, InputManager* manager);
	virtual ~MacInputDevice();

	virtual void Activate(bool disable_win_mgr);
	virtual void Release();

	virtual void PollEvents();

protected:
private:
	void EnumElements();

	pRecDevice native_device_;

	int rel_axis_count_;
	int abs_axis_count_;
	int analogue_count_;
	int button_count_;

	logclass();
};



class MacInputManager: public InputManager, public MacObserver {
	typedef InputManager Parent;
public:
	// Declared as friend in order to get access to screen width and height.
	friend class MacInputElement;

	MacInputManager(MacDisplayManager* display_manager);
	virtual ~MacInputManager();

	bool IsInitialized();
	virtual void Refresh();

	MacDisplayManager* GetDisplayManager() const;

	virtual const InputDevice* GetKeyboard() const;
	virtual InputDevice* GetKeyboard();

	// Returns the mouse device, if there is one... NULL otherwise.
	virtual const InputDevice* GetMouse() const;
	virtual InputDevice* GetMouse();

	virtual void SetCursorVisible(bool visible);

	virtual float GetCursorX();
	virtual float GetCursorY();
	virtual void SetMousePosition(int x, int y);

	static KeyCode ConvertMacKeyCodeToKeyCode(unsigned mac_key_code);
	static unichar ConvertChar(unichar c);
	static KeyCode ConvertCharToKeyCode(unichar c, bool is_numpad, bool& is_char);

protected:
	virtual void OnEvent(NSEvent* event);
	//void SetKey(KeyCode w_param, long l_param, bool is_down);

private:
	// The DirectInput device enumeration callback.
	//static BOOL CALLBACK EnumDeviceCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID ref);

	void EnumDevices();

	void AddObserver();
	void RemoveObserver();

	MacDisplayManager* display_manager_;

	bool enum_error_;
	bool initialized_;

	NSUInteger key_modifiers_;

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
