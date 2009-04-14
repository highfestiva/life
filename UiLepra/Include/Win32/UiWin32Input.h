/*
	Class:  Win32InputElement,
			Win32InputDevice,
			Win32InputManager
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#pragma once

#define	DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include "../Win32/UiWin32DisplayManager.h"
#include "../UiInput.h"
#include "../UiLepra.h"

namespace UiLepra
{

class Win32InputDevice;




class Win32InputElement: public UiLepra::InputElement
{
public:
	Win32InputElement(UiLepra::InputElement::Type pType,
					  Win32InputDevice* pParentDevice,
					  LPCDIDEVICEOBJECTINSTANCE pElement,
					  unsigned pFieldOffset);
	virtual ~Win32InputElement();

	const LPCDIDEVICEOBJECTINSTANCE GetDirectInputElement() const;
	const LPDIOBJECTDATAFORMAT GetDataFormat() const;

	// Sets the uncalibrated value of this device.
	// (Only useful with analogue elements).
	void SetValue(int pValue);

	unsigned GetCalibrationDataSize();
	void GetCalibrationData(Lepra::uint8* pData);
	void SetCalibrationData(const Lepra::uint8* pData);

protected:
private:

	enum
	{
		MAX_INT = 0x7FFFFFFF,
		MIN_INT  = 0x80000000,
	};

	LPCDIDEVICEOBJECTINSTANCE mElement;
	DIOBJECTDATAFORMAT mDataFormat;

	int mMin;
	int mMax;
};

const LPCDIDEVICEOBJECTINSTANCE Win32InputElement::GetDirectInputElement() const
{
	return mElement;
}

const LPDIOBJECTDATAFORMAT Win32InputElement::GetDataFormat() const
{
	return (LPDIOBJECTDATAFORMAT)&mDataFormat;
}




class Win32InputDevice: public UiLepra::InputDevice
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

	// Only used in element enumeration.
	int mButtonCount;
	int mRelAxisCount;
	int mAbsAxisCount;

	// The DirectInput data format description of mInputData.
	DIDATAFORMAT mDataFormat;
	LPDIDEVICEOBJECTDATA mDeviceObjectData;

	LOG_CLASS_DECLARE();
};



class Win32InputManager: public InputManager, public Win32Observer
{
public:
	// Declared as friend in order to get access to screen width and height.
	friend class Win32InputElement;

	Win32InputManager(Win32DisplayManager* pDisplayManager);
	virtual ~Win32InputManager();

	virtual void Refresh();

	Win32DisplayManager* GetDisplayManager() const;

	virtual const UiLepra::InputDevice* GetKeyboard() const;
	virtual UiLepra::InputDevice* GetKeyboard();

	// Returns the mouse device, if there is one... NULL otherwise.
	virtual const UiLepra::InputDevice* GetMouse() const;
	virtual UiLepra::InputDevice* GetMouse();

	virtual void PollEvents();

	virtual void ShowCursor();
	virtual void HideCursor();
	virtual bool GetMouseButtonState(unsigned pButtonIndex);
	virtual double GetMouseDeltaX();
	virtual double GetMouseDeltaY();
	virtual double GetCursorDeltaX();
	virtual double GetCursorDeltaY();
	virtual double GetMouseDeltaUnit();
	virtual double GetMouseDeltaWheel();

	virtual double GetCursorX();
	virtual double GetCursorY();
	virtual void SetCursorX(double x);
	virtual void SetCursorY(double y);

	bool IsInitialized();


protected:
	bool OnMessage(int pMsg, int pwParam, long plParam);
	void SetKey(KeyCode pWParam, long pLParam, bool pIsDown);

private:
	// The DirectInput device enumeration callback.
	static BOOL CALLBACK EnumDeviceCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	void AddObserver();
	void RemoveObserver();

	Win32DisplayManager* mDisplayManager;

	// The DirectInput object.
	LPDIRECTINPUT8 mDirectInput;

	bool mEnumError;
	bool mInitialized;

	// The entire display area (not just the user's window).
	int mScreenWidth;
	int mScreenHeight;

	// Mouse related stuff.
	double mMouseDX;
	double mMouseDY;
	double mMouseDWheel;
	double mCursorDX;
	double mCursorDY;
	double mCursorX;
	double mCursorY;
	double mPrevCursorX;
	double mPrevCursorY;
	bool mMouseButton[3];

	// Default devices.
	UiLepra::InputDevice* mKeyboard;
	UiLepra::InputDevice* mMouse;
};

bool Win32InputManager::IsInitialized()
{
	return mInitialized;
}

}
