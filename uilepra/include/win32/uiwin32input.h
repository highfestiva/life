
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uiinput.h"
#define	DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include "../win32/uiwin32displaymanager.h"
#include "../uilepra.h"



namespace uilepra {



class Win32InputDevice;



class Win32InputElement: public InputElement {
	typedef InputElement Parent;
public:
	Win32InputElement(InputElement::Type type, Interpretation interpretation, int type_index,
		Win32InputDevice* parent_device, LPCDIDEVICEOBJECTINSTANCE element, unsigned field_offset);
	virtual ~Win32InputElement();

	const LPCDIDEVICEOBJECTINSTANCE GetDirectInputElement() const;
	const LPDIOBJECTDATAFORMAT GetDataFormat() const;

protected:
private:

	enum {
		kMaxInt = 0x7FFFFFFF,
		kMinInt  = 0x80000000,
	};

	LPCDIDEVICEOBJECTINSTANCE element_;
	DIOBJECTDATAFORMAT data_format_;

	logclass();
};

const LPCDIDEVICEOBJECTINSTANCE Win32InputElement::GetDirectInputElement() const {
	return element_;
}

const LPDIOBJECTDATAFORMAT Win32InputElement::GetDataFormat() const {
	return (LPDIOBJECTDATAFORMAT)&data_format_;
}




class Win32InputDevice: public InputDevice {
public:
	Win32InputDevice(LPDIRECTINPUTDEVICE8 di_device, LPCDIDEVICEINSTANCE info, InputManager* manager);
	virtual ~Win32InputDevice();

	virtual void Activate();
	virtual void Release();

	virtual void PollEvents();

protected:
private:
	static BOOL CALLBACK EnumElementsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID ref);

	LPDIRECTINPUTDEVICE8 di_device_;
	bool reacquire_;

	int rel_axis_count_;
	int abs_axis_count_;
	int analogue_count_;
	int button_count_;

	// The DirectInput data format description of mInputData.
	DIDATAFORMAT data_format_;
	LPDIDEVICEOBJECTDATA device_object_data_;

	logclass();
};



class Win32InputManager: public InputManager, public Win32Observer {
	typedef InputManager Parent;
public:
	// Declared as friend in order to get access to screen width and height.
	friend class Win32InputElement;

	Win32InputManager(Win32DisplayManager* display_manager);
	virtual ~Win32InputManager();

	virtual void Refresh();

	Win32DisplayManager* GetDisplayManager() const;

	virtual const InputDevice* GetKeyboard() const;
	virtual InputDevice* GetKeyboard();

	// Returns the mouse device, if there is one... NULL otherwise.
	virtual const InputDevice* GetMouse() const;
	virtual InputDevice* GetMouse();

	virtual void SetCursorVisible(bool visible);

	virtual float GetCursorX();
	virtual float GetCursorY();
	virtual void SetMousePosition(int x, int y);

	bool IsInitialized();


protected:
	bool OnMessage(int msg, int wparam, long lparam);
	void SetKey(KeyCode w_param, long l_param, bool is_down);

private:
	// The DirectInput device enumeration callback.
	static BOOL CALLBACK EnumDeviceCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID ref);

	void AddObserver();
	void RemoveObserver();

	void SetMousePosition(int msg, int x, int y);

	Win32DisplayManager* display_manager_;

	// The DirectInput object.
	LPDIRECTINPUT8 direct_input_;

	bool enum_error_;
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
