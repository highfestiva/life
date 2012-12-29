
// Author: Jonas Bystršm
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../UiLepra.h"
#ifdef LEPRA_IOS
#include "UiMacOpenGLDisplay.h"
#include "../UiInput.h"



namespace UiLepra
{



class IosInputDevice;



class IosInputElement: public InputElement
{
	typedef InputElement Parent;
public:
	IosInputElement(InputElement::Type pType, Interpretation pInterpretation, int pTypeIndex,
		IosInputDevice* pParentDevice);
	virtual ~IosInputElement();

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

	int32 mMin;
	int32 mMax;

	LOG_CLASS_DECLARE();
};



class IosInputDevice: public InputDevice
{
public:
	IosInputDevice(InputManager* pManager);
	virtual ~IosInputDevice();

	virtual void Activate();
	virtual void Release();

	virtual void PollEvents();

	void AddElement(InputElement* pElement);

protected:
private:
	int mRelAxisCount;
	int mAbsAxisCount;
	int mAnalogueCount;
	int mButtonCount;

	LOG_CLASS_DECLARE();
};



class IosInputManager: public InputManager, public MacObserver
{
	typedef InputManager Parent;
public:
	// Declared as friend in order to get access to screen width and height.
	friend class IosInputElement;

	IosInputManager(MacOpenGLDisplay* pDisplayManager);
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

	virtual void SetCursorVisible(bool pVisible);

	virtual float GetCursorX();
	virtual float GetCursorY();
	void SetMousePosition(int x, int y);

private:
	virtual void OnEvent(LEPRA_APPLE_EVENT* e);

	void AddObserver();
	void RemoveObserver();

	MacOpenGLDisplay* mDisplayManager;

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

	LOG_CLASS_DECLARE();
};



}

#endif // !iOS
