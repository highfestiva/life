
// Author: Jonas Bystršm
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../UiLepra.h"
#ifndef LEPRA_IOS
#include "../UiInput.h"
#import <AppKit/AppKit.h>
#include <HID_Utilities.h>
#include <HID_Queue_Utilities.h>
#import "UiMacDisplayManager.h"



namespace UiLepra
{



class MacInputDevice;



class MacInputElement: public InputElement
{
	typedef InputElement Parent;
public:
	MacInputElement(InputElement::Type pType, Interpretation pInterpretation, int pTypeIndex,
		MacInputDevice* pParentDevice, pRecElement pElement);
	virtual ~MacInputElement();

	pRecElement GetNativeElement() const;

	// Sets the uncalibrated value of this device.
	// (Only useful with analogue elements).
	void SetValue(int pValue);

	str GetCalibration() const;
	bool SetCalibration(const str& pData);

private:
	enum
	{
		MAX_INT = 0x7FFFFFFF,
		MIN_INT  = 0x80000000,
	};

	pRecElement mElement;

	int32 mMin;
	int32 mMax;

	LOG_CLASS_DECLARE();
};



class MacInputDevice: public InputDevice
{
public:
	MacInputDevice(pRecDevice pNativeDevice, InputManager* pManager);
	virtual ~MacInputDevice();

	virtual void Activate();
	virtual void Release();

	virtual void PollEvents();

	bool HaveRelativeAxes();

protected:
private:
	void EnumElements();

	pRecDevice mNativeDevice;

	int mRelAxisCount;
	int mAbsAxisCount;
	int mAnalogueCount;
	int mButtonCount;

	LOG_CLASS_DECLARE();
};



class MacInputManager: public InputManager, public MacObserver
{
	typedef InputManager Parent;
public:
	// Declared as friend in order to get access to screen width and height.
	friend class MacInputElement;

	MacInputManager(MacDisplayManager* pDisplayManager);
	virtual ~MacInputManager();

	bool IsInitialized();
	virtual void Refresh();

	MacDisplayManager* GetDisplayManager() const;

	virtual const InputDevice* GetKeyboard() const;
	virtual InputDevice* GetKeyboard();

	// Returns the mouse device, if there is one... NULL otherwise.
	virtual const InputDevice* GetMouse() const;
	virtual InputDevice* GetMouse();

	virtual void ShowCursor();
	virtual void HideCursor();

	virtual float GetCursorX();
	virtual float GetCursorY();
	void SetMousePosition(int x, int y);

	static KeyCode ConvertMacKeyCodeToKeyCode(unsigned pMacKeyCode);
	static unichar ConvertChar(unichar pChar);
	static KeyCode ConvertCharToKeyCode(unichar pChar, bool pIsNumpad, bool& pIsChar);

protected:
	virtual void OnEvent(NSEvent* pEvent);
	//void SetKey(KeyCode pWParam, long pLParam, bool pIsDown);

private:
	// The DirectInput device enumeration callback.
	//static BOOL CALLBACK EnumDeviceCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	void EnumDevices();

	void AddObserver();
	void RemoveObserver();

	MacDisplayManager* mDisplayManager;

	bool mEnumError;
	bool mInitialized;

	NSUInteger mKeyModifiers;

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

	LOG_CLASS_DECLARE();
};



}

#endif // !iOS
