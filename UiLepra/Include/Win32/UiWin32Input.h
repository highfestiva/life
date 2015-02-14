
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiInput.h"
#define	DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include "../Win32/UiWin32DisplayManager.h"
#include "../UiLepra.h"



namespace UiLepra
{



class Win32InputDevice;



class Win32InputElement: public InputElement
{
	typedef InputElement Parent;
public:
	Win32InputElement(InputElement::Type pType, Interpretation pInterpretation, int pTypeIndex,
		Win32InputDevice* pParentDevice, LPCDIDEVICEOBJECTINSTANCE pElement, unsigned pFieldOffset);
	virtual ~Win32InputElement();

	const LPCDIDEVICEOBJECTINSTANCE GetDirectInputElement() const;
	const LPDIOBJECTDATAFORMAT GetDataFormat() const;

protected:
private:

	enum
	{
		MAX_INT = 0x7FFFFFFF,
		MIN_INT  = 0x80000000,
	};

	LPCDIDEVICEOBJECTINSTANCE mElement;
	DIOBJECTDATAFORMAT mDataFormat;

	logclass();
};

const LPCDIDEVICEOBJECTINSTANCE Win32InputElement::GetDirectInputElement() const
{
	return mElement;
}

const LPDIOBJECTDATAFORMAT Win32InputElement::GetDataFormat() const
{
	return (LPDIOBJECTDATAFORMAT)&mDataFormat;
}




class Win32InputDevice: public InputDevice
{
public:
	Win32InputDevice(LPDIRECTINPUTDEVICE8 pDIDevice, LPCDIDEVICEINSTANCE pInfo, InputManager* pManager);
	virtual ~Win32InputDevice();

	virtual void Activate();
	virtual void Release();

	virtual void PollEvents();

protected:
private:
	static BOOL CALLBACK EnumElementsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

	LPDIRECTINPUTDEVICE8 mDIDevice;
	bool mReacquire;

	int mRelAxisCount;
	int mAbsAxisCount;
	int mAnalogueCount;
	int mButtonCount;

	// The DirectInput data format description of mInputData.
	DIDATAFORMAT mDataFormat;
	LPDIDEVICEOBJECTDATA mDeviceObjectData;

	logclass();
};



class Win32InputManager: public InputManager, public Win32Observer
{
	typedef InputManager Parent;
public:
	// Declared as friend in order to get access to screen width and height.
	friend class Win32InputElement;

	Win32InputManager(Win32DisplayManager* pDisplayManager);
	virtual ~Win32InputManager();

	virtual void Refresh();

	Win32DisplayManager* GetDisplayManager() const;

	virtual const InputDevice* GetKeyboard() const;
	virtual InputDevice* GetKeyboard();

	// Returns the mouse device, if there is one... NULL otherwise.
	virtual const InputDevice* GetMouse() const;
	virtual InputDevice* GetMouse();

	virtual void SetCursorVisible(bool pVisible);

	virtual float GetCursorX();
	virtual float GetCursorY();
	virtual void SetMousePosition(int x, int y);

	bool IsInitialized();


protected:
	bool OnMessage(int pMsg, int pwParam, long plParam);
	void SetKey(KeyCode pWParam, long pLParam, bool pIsDown);

private:
	// The DirectInput device enumeration callback.
	static BOOL CALLBACK EnumDeviceCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	void AddObserver();
	void RemoveObserver();

	void SetMousePosition(int pMsg, int x, int y);

	Win32DisplayManager* mDisplayManager;

	// The DirectInput object.
	LPDIRECTINPUT8 mDirectInput;

	bool mEnumError;
	bool mInitialized;

	// The entire display area (not just the user's window).
	int mScreenWidth;
	int mScreenHeight;

	// Mouse related stuff.
	float mCursorX;
	float mCursorY;

	// Default devices.
	InputDevice* mKeyboard;
	InputDevice* mMouse;
	int mTypeCount[InputDevice::TYPE_COUNT];
	logclass();
};



}
