
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../UiInput.h"
#include "../UiLepra.h"



namespace UiLepra
{



class X11DisplayManager;
class X11InputDevice;



#if 0



class X11InputElement: public InputElement
{
public:
	X11InputElement(InputElement::Type pType, Interpretation pInterpretation, int pTypeIndex,
		X11InputDevice* pParentDevice, LPCDIDEVICEOBJECTINSTANCE pElement, unsigned pFieldOffset);
	virtual ~X11InputElement();

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

	LOG_CLASS_DECLARE();
};

const LPCDIDEVICEOBJECTINSTANCE X11InputElement::GetDirectInputElement() const
{
	return mElement;
}

const LPDIOBJECTDATAFORMAT X11InputElement::GetDataFormat() const
{
	return (LPDIOBJECTDATAFORMAT)&mDataFormat;
}




class X11InputDevice: public InputDevice
{
public:
	X11InputDevice(LPDIRECTINPUTDEVICE8 pDIDevice, LPCDIDEVICEINSTANCE pInfo, InputManager* pManager);
	virtual ~X11InputDevice();

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

	LOG_CLASS_DECLARE();
};



class X11InputManager: public InputManager, public X11Observer
{
	typedef InputManager Parent;
public:
	// Declared as friend in order to get access to screen width and height.
	friend class X11InputElement;

	X11InputManager(X11DisplayManager* pDisplayManager);
	virtual ~X11InputManager();

	virtual void Refresh();

	X11DisplayManager* GetDisplayManager() const;

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

	X11DisplayManager* mDisplayManager;

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
};



#endif // 0



}
