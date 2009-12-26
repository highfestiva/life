
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
public:
	Win32InputElement(InputElement::Type pType, Interpretation pInterpretation, int pTypeIndex,
		Win32InputDevice* pParentDevice, LPCDIDEVICEOBJECTINSTANCE pElement, unsigned pFieldOffset);
	virtual ~Win32InputElement();

	const LPCDIDEVICEOBJECTINSTANCE GetDirectInputElement() const;
	const LPDIOBJECTDATAFORMAT GetDataFormat() const;

	// Sets the uncalibrated value of this device.
	// (Only useful with analogue elements).
	void SetValue(int pValue);

	str GetCalibration() const;
	bool SetCalibration(const str& pData);

protected:
private:

	enum
	{
		MAX_INT = 0x7FFFFFFF,
		MIN_INT  = 0x80000000,
	};

	LPCDIDEVICEOBJECTINSTANCE mElement;
	DIOBJECTDATAFORMAT mDataFormat;

	int32 mMin;
	int32 mMax;

	LOG_CLASS_DECLARE();
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

	bool HaveRelativeAxes();

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

	LOG_CLASS_DECLARE();
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

	virtual void ShowCursor();
	virtual void HideCursor();

	virtual double GetCursorX();
	virtual double GetCursorY();

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
	double mCursorX;
	double mCursorY;

	// Default devices.
	InputDevice* mKeyboard;
	InputDevice* mMouse;
	int mTypeCount[InputDevice::TYPE_COUNT];
};



}
