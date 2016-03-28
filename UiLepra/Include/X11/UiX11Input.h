
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiInput.h"
#include "../X11/UiX11DisplayManager.h"
#include "../UiLepra.h"



namespace UiLepra
{



class X11DisplayManager;
class X11InputDevice;



class X11InputElement: public InputElement
{
public:
	X11InputElement(InputElement::Type pType, Interpretation pInterpretation, int pTypeIndex,
		X11InputDevice* pParentDevice, void* pRawElement);
	virtual ~X11InputElement();

	const void* GetRawElement() const;

protected:
private:
	void* mRawElement;

	logclass();
};



class X11InputDevice: public InputDevice
{
public:
	X11InputDevice(void* pRawDevice, InputManager* pManager);
	virtual ~X11InputDevice();

	void AddElement(X11InputElement* pElement);

	virtual void Activate();
	virtual void Release();

	virtual void PollEvents();

protected:
private:
	void* mRawDevice;

	logclass();
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
	bool OnMessage(const XEvent& pEvent);
	static KeyCode TranslateKey(int pState, KeySym pKeySym);

private:
	void AddObserver();
	void RemoveObserver();

	X11DisplayManager* mDisplayManager;

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
