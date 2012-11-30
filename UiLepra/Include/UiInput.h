
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <hash_set>
#include <list>
#include <vector>
#include "../../Lepra/Include/LepraTypes.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/HashTable.h"
#include "../Include/UiLepra.h"



namespace UiLepra
{



class DisplayManager;
class InputDevice;
class InputElement;
class InputManager;



class TextInputObserver
{
public:
	virtual bool OnChar(tchar pChar) = 0;
};

class KeyCodeInputObserver;

class MouseInputObserver
{
public:
	virtual bool OnDoubleClick() = 0;
};



class InputFunctor
{
public:
	virtual void Call(InputElement* pElement) = 0;
	virtual InputFunctor* CreateCopy() const = 0;
};

template<class _TClass> class TInputFunctor : public InputFunctor
{
public:
	TInputFunctor(_TClass* pObject, void (_TClass::*pFunc)(InputElement* pElement)):
		mObject(pObject),
		mFunc(pFunc)
	{
	}
	
	virtual void Call(InputElement* pElement)
	{
		(mObject->*mFunc)(pElement);
	}

	InputFunctor* CreateCopy() const
	{
		return new TInputFunctor(mObject, mFunc);
	}

private:
	_TClass* mObject;
	void (_TClass::*mFunc)(InputElement* pElement);
};



class InputElement
{
public:
	
	friend class InputDevice;

	enum Type
	{
		UNKNOWN = 0,
		ANALOGUE,
		DIGITAL,
	};

	enum Interpretation
	{
		ABSOLUTE_AXIS,
		RELATIVE_AXIS,
		BUTTON1,	// BUTTON1 + offset gives the button index.
	};

	InputElement(Type pType, Interpretation pInterpretation, int pTypeIndex, InputDevice* pParentDevice);
	virtual ~InputElement();

	Type GetType() const;

	Interpretation GetInterpretation() const;
	int GetTypeIndex() const;

	InputDevice* GetParentDevice() const;

	// If digital:
	//    Returns true if "pressed", false otherwise.
	// If analogue:
	//    Returns true if the analogue value is greater than pThreshold.
	bool GetBooleanValue(float pThreshold = 0.5) const;

	// If digital:
	//    Returns 1 if "pressed", 0 otherwise.
	// If analogue:
	//    Returns the analogue value normalized around -1 and 1.
	float GetValue() const;
	// Sets the uncalibrated value of this device.
	// (Only useful with analogue elements).
	virtual void SetValue(int pNewValue) = 0;

	// Returns the difference between the current value and the previous value.
	float GetDeltaValue() const;

	void SetIdentifier(const str& pIdentifier);
	const str& GetIdentifier() const;
	str GetFullName() const;
	str GetName() const;

	// Sets the input listener functor. The InputElement will be responsible
	// of deleting it.
	void AddFunctor(InputFunctor* pFunctor);
	void ClearFunctors();

	virtual str GetCalibration() const = 0;
	virtual bool SetCalibration(const str& pData) = 0;

protected:
	virtual void SetValue(float pNewValue);
private:

	float mPrevValue;
	float mValue;

	Type mType;
	Interpretation mInterpretation;
	int mTypeIndex;
	InputDevice* mParentDevice;

	str mIdentifier;

	typedef std::vector<InputFunctor*> FunctorArray;
	FunctorArray mFunctorArray;

	LOG_CLASS_DECLARE();
};

#define ADD_INPUT_CALLBACK(_e, _func, _class) \
	(_e)->AddFunctor(new UiLepra::TInputFunctor<_class>(this, &_class::_func));



class InputDevice
{
public:
	typedef std::vector<InputElement*> ElementArray;

	enum Interpretation
	{
		TYPE_MOUSE = 0,
		TYPE_KEYBOARD,
		TYPE_JOYSTICK,
		TYPE_GAMEPAD,
		TYPE_1STPERSON,
		TYPE_PEDALS,
		TYPE_WHEEL,
		TYPE_FLIGHT,
		TYPE_OTHER,
		TYPE_COUNT
	};

	typedef std::pair<str, str> CalibrationElement;
	typedef std::vector<CalibrationElement> CalibrationData;

	InputDevice(InputManager* pManager);
	virtual ~InputDevice();

	Interpretation GetInterpretation() const;
	int GetTypeIndex() const;
	void SetInterpretation(Interpretation pInterpretation, int pTypeIndex);

	InputManager* GetManager() const;

	// Check and set if device is active.
	bool IsActive();

	// Since only one window (or application) should be in focus of
	// the input, the display manager is required.
	virtual void Activate() = 0;
	virtual void Release() = 0;

	virtual void PollEvents() = 0;

	// Returns the array of input elements.
	const ElementArray& GetElements() const;

	// Returns the first input element identified by pIdentifier.
	// Returns NULL if no element was found.
	const InputElement* GetElement(const str& pIdentifier) const;
	InputElement* GetElement(const str& pIdentifier);
	InputElement* GetElement(unsigned pElementIndex) const;
	unsigned GetElementIndex(InputElement* pElement) const;

	InputElement* GetButton(unsigned pButtonIndex) const;
	InputElement* GetAxis(unsigned pAxisIndex) const;

	unsigned GetNumElements() const;
	unsigned GetNumDigitalElements();
	unsigned GetNumAnalogueElements();

	void SetIdentifier(const str& pIdentifier);
	const str& GetIdentifier() const;
	const str& GetUniqueIdentifier() const;

	// Sets an observer on the entire device. (All elements).
	// The device takes care of deleting the functor.
	void AddFunctor(InputFunctor* pFunctor);
	void ClearFunctors();

	CalibrationData GetCalibration() const;
	bool SetCalibration(const CalibrationData& pData);

protected:

	void SetActive(bool pActive);

	void SetElementValue(InputElement* pElement, float pValue);

	ElementArray mElementArray;

private:

	void CountElements();

	InputManager* mManager;
	Interpretation mInterpretation;
	int mTypeIndex;
	int mNumDigitalElements;
	int mNumAnalogueElements;

	bool mActive;

	str mIdentifier;
	str mUniqueIdentifier;

	LOG_CLASS_DECLARE();
};



/*
	class InputManager

	This is the main class of the input system. It provides you
	with a list of all input devices (call GetDeviceList()), and
	some default mouse functionality.
*/

class InputManager
{
public:

	typedef std::list<InputDevice*> DeviceList;

	enum KeyCode
	{
		IN_KBD_BACKSPACE	= 8,
		IN_KBD_TAB		= 9,

		IN_KBD_CENTER		= 12,
		IN_KBD_ENTER		= 13,

		IN_KBD_LSHIFT		= 16,
		IN_KBD_LCTRL		= 17,
		IN_KBD_LALT		= 18,

		IN_KBD_PAUSE		= 19,
		IN_KBD_CAPS_LOCK	= 20,

		IN_KBD_ESC		= 27,

		IN_KBD_SPACE		= 32,
		IN_KBD_PGUP		= 33,
		IN_KBD_PGDOWN		= 34,
		IN_KBD_END		= 35,
		IN_KBD_HOME		= 36,
		IN_KBD_LEFT		= 37,
		IN_KBD_UP		= 38,
		IN_KBD_RIGHT		= 39,
		IN_KBD_DOWN		= 40,

		IN_KBD_PRINT_SCREEN	= 44,
		IN_KBD_INSERT		= 45,
		IN_KBD_DEL		= 46,

		IN_KBD_0		= 48,
		IN_KBD_1		= 49,
		IN_KBD_2		= 50,
		IN_KBD_3		= 51,
		IN_KBD_4		= 52,
		IN_KBD_5		= 53,
		IN_KBD_6		= 54,
		IN_KBD_7		= 55,
		IN_KBD_8		= 56,
		IN_KBD_9		= 57,

		IN_KBD_A		= 65,
		IN_KBD_B		= 66,
		IN_KBD_C		= 67,
		IN_KBD_D		= 68,
		IN_KBD_E		= 69,
		IN_KBD_F		= 70,
		IN_KBD_G		= 71,
		IN_KBD_H		= 72,
		IN_KBD_I		= 73,
		IN_KBD_J		= 74,
		IN_KBD_K		= 75,
		IN_KBD_L		= 76,
		IN_KBD_M		= 77,
		IN_KBD_N		= 78,
		IN_KBD_O		= 79,
		IN_KBD_P		= 80,
		IN_KBD_Q		= 81,
		IN_KBD_R		= 82,
		IN_KBD_S		= 83,
		IN_KBD_T		= 84,
		IN_KBD_U		= 85,
		IN_KBD_V		= 86,
		IN_KBD_W		= 87,
		IN_KBD_X		= 88,
		IN_KBD_Y		= 89,
		IN_KBD_Z		= 90,
		IN_KBD_LOS		= 91,
		IN_KBD_ROS		= 92,

		IN_KBD_CONTEXT_MENU	= 93,	// The "windows menu" button on Microsoft-compatible keyboards.

		IN_KBD_NUMPAD_0		= 96,
		IN_KBD_NUMPAD_1		= 97,
		IN_KBD_NUMPAD_2		= 98,
		IN_KBD_NUMPAD_3		= 99,
		IN_KBD_NUMPAD_4		= 100,
		IN_KBD_NUMPAD_5		= 101,
		IN_KBD_NUMPAD_6		= 102,
		IN_KBD_NUMPAD_7		= 103,
		IN_KBD_NUMPAD_8		= 104,
		IN_KBD_NUMPAD_9		= 105,
		IN_KBD_NUMPAD_MUL	= 106,
		IN_KBD_NUMPAD_PLUS	= 107,
		IN_KBD_NUMPAD_MINUS	= 109,
		IN_KBD_NUMPAD_DOT	= 110,
		IN_KBD_NUMPAD_DIV	= 111,
		IN_KBD_F1		= 112,
		IN_KBD_F2		= 113,
		IN_KBD_F3		= 114,
		IN_KBD_F4		= 115,
		IN_KBD_F5		= 116,
		IN_KBD_F6		= 117,
		IN_KBD_F7		= 118,
		IN_KBD_F8		= 119,
		IN_KBD_F9		= 120,
		IN_KBD_F10		= 121,
		IN_KBD_F11		= 122,
		IN_KBD_F12		= 123,
		IN_KBD_NUM_LOCK		= 144,
		IN_KBD_SCROLL_LOCK	= 145,

		IN_KBD_QUICK_BACK	= 166,
		IN_KBD_QUICK_FORWARD	= 167,
		IN_KBD_QUICK_REFRESH	= 168,
		IN_KBD_QUICK_STOP	= 169,
		IN_KBD_QUICK_SEARCH	= 170,
		IN_KBD_QUICK_FAVORITES	= 171,
		IN_KBD_QUICK_WEB_HOME	= 172,
		IN_KBD_QUICK_SOUND_MUTE	= 173,
		IN_KBD_QUICK_DECR_VOLUME= 174,
		IN_KBD_QUICK_INCR_VOLUME= 175,
		IN_KBD_QUICK_NAV_RIGHT	= 176,
		IN_KBD_QUICK_NAV_LEFT	= 177,
		IN_KBD_QUICK_NAV_STOP	= 178,
		IN_KBD_QUICK_NAV_PLAYPAUSE= 179,
		IN_KBD_QUICK_MAIL	= 180,
		IN_KBD_QUICK_MEDIA	= 181,
		IN_KBD_QUICK_MY_COMPUTER= 182,
		IN_KBD_QUICK_CALCULATOR	= 183,

		IN_KBD_DIAERESIS	= 186,
		IN_KBD_PLUS		= 187,
		IN_KBD_COMMA		= 188,
		IN_KBD_MINUS		= 189,
		IN_KBD_DOT		= 190,
		IN_KBD_APOSTROPHE	= 191,

		IN_KBD_OE		= 192,

		IN_KBD_ACUTE		= 219,
		IN_KBD_PARAGRAPH	= 220,
		IN_KBD_AA		= 221,
		IN_KBD_AE		= 222,

		IN_KBD_COMPARE		= 226,

		IN_KBD_RSHIFT		= 227,	// Not defined by Win32.
		IN_KBD_RCTRL		= 228,	// Not defined by Win32.
		IN_KBD_RALT		= 229,	// Not defined by Win32.
	};

	static InputManager* CreateInputManager(DisplayManager* pDisplayManager);

	// Returns a singleton input manager (created if needed).
	// This function is implemented elsewhere (not in Input.cpp),
	// depending on the platform dependent implementation.
	// Returns a null pointer if something went wrong in the creation.
	//static InputManager* GetInputManager();

	// A call to this function must be issued when the application
	// exits, IFF GetInputManager() was previously called.
	//static void ReleaseInputManager();

	virtual ~InputManager();

	virtual void Refresh() = 0;

	// Returns a list of all available input devices.
	const DeviceList& GetDeviceList() const;
	
	// Returns the keyboard device, if there is one... NULL otherwise.
	virtual const InputDevice* GetKeyboard() const = 0;
	virtual InputDevice* GetKeyboard() = 0;

	// Returns the mouse device, if there is one... NULL otherwise.
	virtual const InputDevice* GetMouse() const = 0;
	virtual InputDevice* GetMouse() = 0;

	// Searches for device number pN, with identifier pDeviceIdentifier.
	InputDevice* FindDevice(const str& pDeviceIdentifier, int pN = 0);

	// Returns the number of devices with the same identifier.
	int QueryIdentifierCount(const str& pDeviceIdentifier) const;

	// Returns which index the device has in the list of devices with the
	// same identifier. Thus, if there are two gamepads, and we pass one
	// of them as the parameter, the function will return either 0 or 1,
	// depending on if this is the first or second gamepad. This index can
	// be used with FindDevice() later on.
	unsigned GetDeviceIndex(InputDevice* pDevice) const;

	void AddTextInputObserver(TextInputObserver* pListener);
	void RemoveTextInputObserver(TextInputObserver* pListener);

	void AddKeyCodeInputObserver(KeyCodeInputObserver* pListener);
	void RemoveKeyCodeInputObserver(KeyCodeInputObserver* pListener);

	void AddMouseInputObserver(MouseInputObserver* pListener);
	void RemoveMouseInputObserver(MouseInputObserver* pListener);

	// Call this every frame, or at least at a regular basis.
	void PollEvents();

	// Sets one observer on all input devices, and all elements.
	// The input manager takes care of deleting the functor.
	void AddFunctor(InputFunctor* pFunctor);
	void ClearFunctors();

	void AddInputDevice(InputDevice* pDevice);
	void RemoveInputDevice(InputDevice* pDevice);

	// Activase/release all devices.
	void ActivateAll();
	void ReleaseAll();

	virtual void ActivateKeyboard();
	virtual void ReleaseKeyboard();

	/*
		Default mouse management.
	*/

	// Show/hide system default cursor.
	virtual void ShowCursor() = 0;
	virtual void HideCursor() = 0;

	// Range: [-1, 1] (Left and right, up and down)
	virtual float GetCursorX() = 0;
	virtual float GetCursorY() = 0;
	void GetMousePosition(int& x, int& y) const;
	virtual void SetMousePosition(int x, int y);

	bool NotifyOnChar(tchar pChar);
	bool NotifyOnKeyDown(KeyCode pKeyCode);
	bool NotifyOnKeyUp(KeyCode pKeyCode);
	bool NotifyMouseDoubleClick();

	bool ReadKey(KeyCode pKeyCode);

	static str GetKeyName(KeyCode pKeyCode);

protected:

	DeviceList mDeviceList;

	InputManager();

	void SetKey(KeyCode pKeyCode, bool pValue);

private:

	typedef std::hash_set<TextInputObserver*, LEPRA_VOIDP_HASHER> TextObserverList;
	typedef std::hash_set<KeyCodeInputObserver*, LEPRA_VOIDP_HASHER> KeyCodeObserverList;
	typedef std::hash_set<MouseInputObserver*, LEPRA_VOIDP_HASHER> MouseObserverList;
	TextObserverList mTextObserverList;
	KeyCodeObserverList mKeyCodeObserverList;
	MouseObserverList mMouseObserverList;

	bool mKeyDown[256];
	int mMouseX;
	int mMouseY;

	LOG_CLASS_DECLARE();
};



class KeyCodeInputObserver
{
public:
	virtual bool OnKeyDown(InputManager::KeyCode pKeyCode) = 0;
	virtual bool OnKeyUp(InputManager::KeyCode pKeyCode) = 0;
};



}
