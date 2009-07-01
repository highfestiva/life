/*
	Class:  TextInputObserver,
		KeyCodeInputObserver,
		MouseInputObserver,
		InputFunctor,
		InputElement,
		InputDevice,
		InputManager,
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	These classes constitutes the input system. This system offers a variety 
	of ways of reading input from the keyboard and the mouse, but only one way 
	to read other devices.

	The InputDevice- and the InputElement-interfaces are preferrably used for
	in-game action like steering the player's character, and nothing else.
	There is no way to tell which button on the keyboard (for instance) an 
	InputElement is associated with before the key is actually pressed and
	you can find out which index it has, unless all keyboards in the world
	are indexed in the same way.

	Other ways of reading input from the mouse and the keyboard are:

	1. Read the current state of a button and the mouse cursor. This is the
	   easiest way of reading input when you don't care about how many times 
	   the user clicks/presses a button.
	2. Implement one of the following interfaces:

	   * TextInputObserver. This makes life easy to read text from the 
	     keyboard. You don't even have to care about the language settings
	     since this is handled by the underlying OS.

	   * KeyCodeInputObserver. Use this if it is important to read every
	     keypress including the repeated keypresses that occur automatically 
	     when a key is pressed for a longer period of time.

	   * MouseInputObserver. This interface is intentionally limited to only
	     one function - OnDoubleClick(). All other functionality is 
	     implemented much better using the InputDevice of the mouse, or by
	     reading the state directly through InputManager.
*/

#ifndef INPUT_H
#define INPUT_H

#include <hash_set>
#include <list>
#include <vector>
#include "../../Lepra/Include/LepraTypes.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/HashTable.h"

namespace UiLepra
{

class DisplayManager;
class InputDevice;
class InputElement;
class InputManager;

/*
	class TextInputObserver,
	      KeyCodeInputObserver,
	      MouseInputObserver
*/

class TextInputObserver
{
public:
	virtual bool OnChar(Lepra::tchar pChar) = 0;
};

class KeyCodeInputObserver; // Declared at the end of the file.

class MouseInputObserver
{
public:
	virtual bool OnDoubleClick() = 0;
};



/*
	class InputFunctor
	template class TInputFunctor

	These classes are used to manage function pointers to functions that 
	aren't static, where the functions themselves are declared as:

	void FunctionName(InputElement* pElement);

	This makes it easy to listen to input, whether you like to see it as
	regular input events, or "actions".

	To illustrate what I'm talking about, consider the following class:

	class Player
	{
	public:
		void OnJump(InputElement* pElement);
		void OnShoot(InputElement* pElement);
		...

		void BindInput();
	};

	Now, in BindInput(), given that we have two InputElements e1 and e2,
	we can write:

	BIND_INPUT(e1, OnJump, Player);    
	BIND_INPUT(e2, OnShoot, Player);

	...where BIND_INPUT(_e, _func, _class) is a macro defined as:
	(e)->AddFunctor(new TInputFunctor<_class>(this, (_func)));

	When the state of e1 and e2 changes, the functions OnJump() and OnShoot()
	will be called.

	To unbind elements, just make the call "element->ClearFunctorArray()" on that 
	element.

	In order to store keybindings in a file, you need to identify the exact
	element to bind. To do this, store the following information:

	1. The device's identifier. Can be found through element->
	   GetParentDevice()->GetIdentifier().
	2. The device's index (in case there are more than one device with the 
	   same identifier). This can be found through InputManager::
	   GetDeviceManager()->GetDeviceIndex(element->GetParentDevice()).
	3. The element's identifier or index. (element->GetIdentifier() or 
	   element->GetParentDevice()->GetElementIndex(element)).

	If you have deviceID, deviceIndex and elementID, you can then find the
	element e like this:
	e = InputManager::GetDeviceManager()->FindDevice(deviceID, deviceIndex)->
	GetElement(elementID).
*/
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






/*
	class InputElement

	This class represents any input element on an input device.
	It can be either "digital" (boolean - true or false) like a 
	button, or analogue like the axes of a joystick.

	The identification string should not be used to search for 
	(or identify) elements, since the actual string depends on 
	the language of the OS among other things. If you need to
	find a specific element on a device, you have to know its 
	type (digital or analogue) and its index.

	All analogue elements that give absolute coordinates (like
	the axes on a joystick) should be calibrated before use.
	The element will always auto-calibrate wile being used.
	So all you need to do to calibrate your joystick is to move
	it around to its extents, and then you are ready to go.
	
	To spare the user from doing this every time, you can get
	and set the calibration data as raw data (a byte-array).
*/
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
	bool GetBooleanValue(Lepra::float64 pThreshold = 0.5) const;

	// If digital:
	//    Returns 1 if "pressed", 0 otherwise.
	// If analogue:
	//    Returns the analogue value normalized around -1 and 1.
	Lepra::float64 GetValue() const;

	// Returns the difference between the current value and the previous value.
	Lepra::float64 GetDeltaValue() const;

	void SetIdentifier(const Lepra::String& pIdentifier);
	const Lepra::String& GetIdentifier() const;
	Lepra::String GetFullName() const;

	// Sets the input listener functor. The InputElement will be responsible
	// of deleting it.
	void AddFunctor(InputFunctor* pFunctor);
	void ClearFunctorArray();

	// Returns the required size in bytes of the calibration data.
	// This value is always valid.
	virtual unsigned GetCalibrationDataSize() = 0;

	// Gets and sets raw calibration data.
	virtual void GetCalibrationData(Lepra::uint8* pData) = 0;
	virtual void SetCalibrationData(const Lepra::uint8* pData) = 0;

protected:
	void SetValue(Lepra::float64 pNewValue);
private:

	Lepra::float64 mPrevValue;
	Lepra::float64 mValue;

	Type mType;
	Interpretation mInterpretation;
	int mTypeIndex;
	InputDevice* mParentDevice;

	Lepra::String mIdentifier;

	typedef std::vector<InputFunctor*> FunctorArray;
	FunctorArray mFunctorArray;
};

#define BIND_INPUT(_e, _func, _class) \
	(_e)->AddFunctor(new UiLepra::TInputFunctor<_class>(this, &_class::_func));







/*
	class InputDevice

	This class represents an input device, which can be anything
	from a regular mouse or keyboard, to a joystick or even a
	VR-helmet.
*/

class InputDevice
{
public:
	typedef std::vector<InputElement*> ElementArray;

	enum Interpretation
	{
		TYPE_MOUSE = 0,
		TYPE_KEYBOARD,
		TYPE_OTHER,
	};

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
	const InputElement* GetElement(const Lepra::String& pIdentifier) const;
	InputElement* GetElement(const Lepra::String& pIdentifier);
	InputElement* GetElement(unsigned pElementIndex) const;
	unsigned GetElementIndex(InputElement* pElement) const;

	InputElement* GetButton(unsigned pButtonIndex) const;
	InputElement* GetAxis(unsigned pAxisIndex) const;

	unsigned GetNumElements() const;
	unsigned GetNumDigitalElements();
	unsigned GetNumAnalogueElements();

	void SetIdentifier(const Lepra::String& pIdentifier);
	const Lepra::String& GetIdentifier() const;

	// Sets an observer on the entire device. (All elements).
	// The device takes care of deleting the functor.
	void AddFunctor(InputFunctor* pFunctor);

	unsigned GetCalibrationDataSize();
	void GetCalibrationData(Lepra::uint8* pData);
	void SetCalibrationData(Lepra::uint8* pData);

protected:

	void SetActive(bool pActive);

	void SetElementValue(InputElement* pElement, Lepra::float64 pValue);

	ElementArray mElementArray;

private:

	void CountElements();

	InputManager* mManager;
	Interpretation mInterpretation;
	int mTypeIndex;
	int mNumDigitalElements;
	int mNumAnalogueElements;

	bool mActive;

	Lepra::String mIdentifier;
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
		IN_KBD_LWIN	= 91,
		IN_KBD_RWIN	= 92,

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
	InputDevice* FindDevice(const Lepra::String& pDeviceIdentifier, int pN = 0);

	// Returns the number of devices with the same identifier.
	int GetDeviceCount(const Lepra::String& pDeviceIdentifier) const;

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
	
	// Activase/release all devices.
	void ActivateAll();
	void ReleaseAll();

	/*
		Default mouse management.
	*/

	// Show/hide system default cursor.
	virtual void ShowCursor() = 0;
	virtual void HideCursor() = 0;

	// Range: [-1, 1] (Left and right, up and down)
	virtual Lepra::float64 GetCursorX() = 0;
	virtual Lepra::float64 GetCursorY() = 0;

	bool ReadKey(KeyCode pKeyCode);

	static Lepra::String GetKeyName(KeyCode pKeyCode);

protected:

	DeviceList mDeviceList;

	InputManager();

	// Will notify all text input observers...
	// Is called from the subclass.
	bool NotifyOnChar(Lepra::tchar pChar);
	bool NotifyOnKeyDown(KeyCode pKeyCode);
	bool NotifyOnKeyUp(KeyCode pKeyCode);
	bool NotifyMouseDoubleClick();

	void SetKey(KeyCode pKeyCode, bool pValue);

private:

	typedef std::hash_set<TextInputObserver*, std::hash<void*> > TextObserverList;
	typedef std::hash_set<KeyCodeInputObserver*, std::hash<void*> > KeyCodeObserverList;
	typedef std::hash_set<MouseInputObserver*, std::hash<void*> > MouseObserverList;
	TextObserverList mTextObserverList;
	KeyCodeObserverList mKeyCodeObserverList;
	MouseObserverList mMouseObserverList;

	bool mKeyDown[256];
};



class KeyCodeInputObserver
{
public:
	virtual bool OnKeyDown(InputManager::KeyCode pKeyCode) = 0;
	virtual bool OnKeyUp(InputManager::KeyCode pKeyCode) = 0;
};


} // End namespace.

#endif
