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
	(e)->SetFunctor(new TInputFunctor<_class>(this, (_func)));

	When the state of e1 and e2 changes, the functions OnJump() and OnShoot()
	will be called.

	To unbind an element, just make the call "element->SetFunctor(0)" on that 
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
		DIGITAL,
		ANALOGUE,
	};

	enum Interpretation
	{
		NO_INTERPRETATION = 0,

		// Analogue interpretations.
		X_AXIS,       // X-axis on mouse or joystick.
		Y_AXIS,       // Y-axis on mouse or joystick.
		Z_AXIS,       // ??
		MOUSE_WHEEL,
		ACCELERATION, // Gas pedal, thrust or something.

		// Digital interpretations.
		BUTTON1,	// BUTTON1 + offset gives the button index.
	};

	InputElement(Type pType, InputDevice* pParentDevice);
	virtual ~InputElement();

	Type GetType() const;

	// Gets and sets the preferred interpretation of this element.
	Interpretation GetInterpretation() const;
	void SetInterpretation(Interpretation pInterpretation);

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

	// Sets the input listener functor. The InputElement will be responsible
	// of deleting it.
	void SetFunctor(InputFunctor* pFunctor);
	const InputFunctor* GetFunctor();

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
	InputDevice* mParentDevice;

	Lepra::String mIdentifier;

	// The one and only "action listener", or "input listener".
	InputFunctor* mFunctor;
};

#define BIND_INPUT(_e, _func, _class) \
	(_e)->SetFunctor(new UiLepra::TInputFunctor<_class>(this, &_class::_func));







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

	InputDevice(InputManager* pManager);
	virtual ~InputDevice();

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

	// Sets the observer on a specific element.
	void SetFunctor(const Lepra::String& pElementIdentifier,
					InputFunctor* pFunctor);

	// Sets an observer on the entire device. (All elements).
	// The device takes care of deleting the functor.
	void SetFunctor(InputFunctor* pFunctor);

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
		KEY_BACKSPACE		= 8,
		KEY_TAB			= 9,

		KEY_ENTER		= 13,

		KEY_LSHIFT		= 16,
		KEY_LCTRL		= 17,
		KEY_LALT		= 18,

		KEY_PAUSE		= 19,
		KEY_CAPS_LOCK		= 20,

		KEY_ESC			= 27,

		KEY_SPACE		= 32,
		KEY_PGUP		= 33,
		KEY_PGDOWN		= 34,
		KEY_END			= 35,
		KEY_HOME		= 36,
		KEY_LEFT		= 37,
		KEY_UP			= 38,
		KEY_RIGHT		= 39,
		KEY_DOWN		= 40,

		KEY_PRINT_SCREEN	= 44,
		KEY_INSERT		= 45,
		KEY_DEL			= 46,

		KEY_0			= 48,
		KEY_1			= 49,
		KEY_2			= 50,
		KEY_3			= 51,
		KEY_4			= 52,
		KEY_5			= 53,
		KEY_6			= 54,
		KEY_7			= 55,
		KEY_8			= 56,
		KEY_9			= 57,

		KEY_A			= 65,
		KEY_B			= 66,
		KEY_C			= 67,
		KEY_D			= 68,
		KEY_E			= 69,
		KEY_F			= 70,
		KEY_G			= 71,
		KEY_H			= 72,
		KEY_I			= 73,
		KEY_J			= 74,
		KEY_K			= 75,
		KEY_L			= 76,
		KEY_M			= 77,
		KEY_N			= 78,
		KEY_O			= 79,
		KEY_P			= 80,
		KEY_Q			= 81,
		KEY_R			= 82,
		KEY_S			= 83,
		KEY_T			= 84,
		KEY_U			= 85,
		KEY_V			= 86,
		KEY_W			= 87,
		KEY_X			= 88,
		KEY_Y			= 89,
		KEY_Z			= 90,
		KEY_LWIN_START		= 91,
		KEY_RWIN_START		= 92,

		KEY_CONTEXT_MENU	= 93,	// The "windows menu" button on Microsoft-compatible keyboards.

		KEY_NUMPAD_0		= 96,
		KEY_NUMPAD_1		= 97,
		KEY_NUMPAD_2		= 98,
		KEY_NUMPAD_3		= 99,
		KEY_NUMPAD_4		= 100,
		KEY_NUMPAD_5		= 101,
		KEY_NUMPAD_6		= 102,
		KEY_NUMPAD_7		= 103,
		KEY_NUMPAD_8		= 104,
		KEY_NUMPAD_9		= 105,
		KEY_NUMPAD_MUL		= 106,
		KEY_NUMPAD_PLUS		= 107,
		KEY_NUMPAD_MINUS	= 109,
		KEY_NUMPAD_DOT		= 110,
		KEY_NUMPAD_DIV		= 111,
		KEY_F1			= 112,
		KEY_F2			= 113,
		KEY_F3			= 114,
		KEY_F4			= 115,
		KEY_F5			= 116,
		KEY_F6			= 117,
		KEY_F7			= 118,
		KEY_F8			= 119,
		KEY_F9			= 120,
		KEY_F10			= 121,
		KEY_F11			= 122,
		KEY_F12			= 123,
		KEY_NUM_LOCK		= 144,
		KEY_SCROLL_LOCK		= 145,

		KEY_QUICK_BACK		= 166,
		KEY_QUICK_FORWARD	= 167,
		KEY_QUICK_REFRESH	= 168,
		KEY_QUICK_STOP		= 169,
		KEY_QUICK_SEARCH	= 170,
		KEY_QUICK_FAVORITES	= 171,
		KEY_QUICK_WEB_HOME	= 172,
		KEY_QUICK_SOUND_MUTE	= 173,
		KEY_QUICK_DECR_VOLUME	= 174,
		KEY_QUICK_INCR_VOLUME	= 175,
		KEY_QUICK_NAV_RIGHT	= 176,
		KEY_QUICK_NAV_LEFT	= 177,
		KEY_QUICK_NAV_STOP	= 178,
		KEY_QUICK_NAV_PLAYPAUSE	= 179,
		KEY_QUICK_MAIL		= 180,
		KEY_QUICK_MEDIA		= 181,
		KEY_QUICK_MY_COMPUTER	= 182,
		KEY_QUICK_CALCULATOR	= 183,

		KEY_DIAERESIS		= 186,
		KEY_PLUS		= 187,
		KEY_COMMA		= 188,
		KEY_MINUS		= 189,
		KEY_DOT			= 190,
		KEY_APOSTROPHE		= 191,

		KEY_OE			= 192,

		KEY_ACUTE		= 219,
		KEY_PARAGRAPH		= 220,
		KEY_AA			= 221,
		KEY_AE			= 222,

		KEY_COMPARE		= 226,

		KEY_RSHIFT		= 227,	// Not defined by Win32.
		KEY_RCTRL		= 228,	// Not defined by Win32.
		KEY_RALT		= 229,	// Not defined by Win32.
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
	virtual void PollEvents();

	// Sets one observer on all input devices, and all elements.
	// The input manager takes care of deleting the functor.
	void SetFunctor(InputFunctor* pFunctor);
	
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
	virtual void SetCursorX(Lepra::float64 x) = 0;
	virtual void SetCursorY(Lepra::float64 y) = 0;

	// Returns the state of the mouse button with index pButtonIndex.
	// The first mouse button has index 0. For any given system, the only
	// thing that can be promised is that there is ONE button, so don't
	// rely on multiple button mice.
	virtual bool GetMouseButtonState(unsigned pButtonIndex) = 0;

	// Returns change in mouse movement since last PollEvents call. 
	virtual Lepra::float64 GetMouseDeltaX() = 0;
	virtual Lepra::float64 GetMouseDeltaY() = 0;

	// Returns the change in mouse cursor position since last PollEvents call.
	virtual Lepra::float64 GetCursorDeltaX() = 0;
	virtual Lepra::float64 GetCursorDeltaY() = 0;

	virtual Lepra::float64 GetMouseDeltaUnit() = 0;

	// Get change in wheel position since last PollEvents call. 
	// (Size of one click = 0.1f).
	virtual Lepra::float64 GetMouseDeltaWheel() = 0;

	bool ReadKey(KeyCode pKeyCode);

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
