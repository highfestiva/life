
// Author: Jonas Bystrm
// Copyright (c) Pixel Doctrine



#include "../../Include/Mac/UiMacInput.h"
#ifndef LEPRA_IOS
#include "../../../Lepra/Include/CyclicArray.h"
#include "../../../Lepra/Include/Log.h"
#include "../../Include/Mac/UiMacCore.h"
#include "../../Include/Mac/UiMacDisplayManager.h"



namespace UiLepra
{



InputManager* InputManager::CreateInputManager(DisplayManager* pDisplayManager)
{
	return (new MacInputManager((MacDisplayManager*)pDisplayManager));
}



MacInputElement::MacInputElement(Type pType, Interpretation pInterpretation, int pTypeIndex,
	MacInputDevice* pParentDevice, pRecElement pElement):
	InputElement(pType, pInterpretation, pTypeIndex, pParentDevice),
	mElement(pElement)
{
	pParentDevice->CountElements();
	str lName;
	if (pType == ANALOGUE)
	{
		lName = _T("Axis") + strutil::IntToString(pParentDevice->GetNumAnalogueElements(), 10);
	}
	else
	{
		lName = str(_T("Button")) + strutil::IntToString(pParentDevice->GetNumDigitalElements(), 10);
	}
	SetIdentifier(lName);
	//mLog.Headlinef(_T("%s got Mac input element %s of type %i w/ index %i."), pParentDevice->GetIdentifier().c_str(), lName, GetType(), pTypeIndex);
}

MacInputElement::~MacInputElement()
{
}

pRecElement MacInputElement::GetNativeElement() const
{
	return (mElement);
}

loginstance(UI_INPUT, MacInputElement);



/*
	class MacInputDevice
*/

MacInputDevice::MacInputDevice(pRecDevice pNativeDevice, InputManager* pManager):
	InputDevice(pManager),
	mNativeDevice(pNativeDevice),
	mRelAxisCount(0),
	mAbsAxisCount(0),
	mAnalogueCount(0),
	mButtonCount(0)
{
	SetIdentifier(strutil::Encode((const char*)pNativeDevice->product));
	EnumElements();
}

MacInputDevice::~MacInputDevice()
{
	if (IsActive() == true)
	{
		Release();
	}
	//mNativeDevice->Release();
}

void MacInputDevice::Activate()
{
	if (IsActive() == false)
	{
		//mNativeDevice->Acquire();
		SetActive(true);
	}
}

void MacInputDevice::Release()
{
	//mNativeDevice->Unacquire();
	SetActive(false);
}

void MacInputDevice::PollEvents()
{
	if (IsActive() == true)
	{
		typedef std::vector<MacInputElement*> MacElementArray;
		MacElementArray lRelativeAxes;
		ElementArray::iterator x;
		for (x = mElementArray.begin(); x != mElementArray.end(); ++x)
		{
			MacInputElement* lElement = (MacInputElement*)*x;
			if (lElement->GetInterpretation() == InputElement::RELATIVE_AXIS)
			{
				lRelativeAxes.push_back(lElement);
				continue;
			}
			pRecElement lNativeElement = lElement->GetNativeElement();
			const int32 lValue = HIDGetElementValue(mNativeDevice, lNativeElement);
			lElement->SetValue(lValue);
		}

		if (!lRelativeAxes.empty())
		{
			IOHIDEventStruct lHidEvent;
			for (int z = 0; z < 20 && HIDGetEvent(mNativeDevice, &lHidEvent); ++z)
			{
				MacElementArray::iterator y = lRelativeAxes.begin();
				for (; y != lRelativeAxes.end(); ++y)
				{
					MacInputElement* lElement = *y;
					if (lElement->GetNativeElement()->cookie == lHidEvent.elementCookie)
					{
						const int lValue = lHidEvent.value;
						lElement->SetValue(lValue);
						break;
					}
				}
			}
		}
	}
}

void MacInputDevice::EnumElements()
{
	pRecElement lCurrentElement = HIDGetFirstDeviceElement(mNativeDevice, kHIDElementTypeInput);
	while (lCurrentElement)
	{
		MacInputElement* lElement = 0;
		switch (lCurrentElement->type)
		{
			case kIOHIDElementTypeInput_Button:
			{
				lElement = new MacInputElement(InputElement::DIGITAL, InputElement::BUTTON, mButtonCount, this, lCurrentElement);
				++mButtonCount;
			}
			break;
			case kIOHIDElementTypeInput_Axis:
			{
				InputElement::Interpretation lInterpretation = InputElement::ABSOLUTE_AXIS;
				if (lCurrentElement->relative)
				{
					lInterpretation = InputElement::RELATIVE_AXIS;
				}
				lElement = new MacInputElement(InputElement::ANALOGUE, lInterpretation, mAnalogueCount, this, lCurrentElement);
				if (lCurrentElement->relative)
				{
					HIDQueueElement(mNativeDevice, lCurrentElement);
				}
				++mAnalogueCount;
			}
			break;
			case kIOHIDElementTypeInput_Misc:
			{
				char name[256];
				HIDGetUsageName(lCurrentElement->usagePage, lCurrentElement->usage, name);
				if (lCurrentElement->usagePage == kHIDPage_GenericDesktop)
				{
					switch(lCurrentElement->usage)
					{
						case kHIDUsage_GD_DPadUp:
						case kHIDUsage_GD_DPadDown:
						case kHIDUsage_GD_DPadRight:
						case kHIDUsage_GD_DPadLeft:
						{
							lElement = new MacInputElement(InputElement::DIGITAL, InputElement::BUTTON, mButtonCount, this, lCurrentElement);
							++mButtonCount;
						}
						break;
						case kHIDUsage_GD_X:
						case kHIDUsage_GD_Y:
						case kHIDUsage_GD_Z:
						case kHIDUsage_GD_Rx:
						case kHIDUsage_GD_Ry:
						case kHIDUsage_GD_Rz:
						case kHIDUsage_GD_Slider:
						case kHIDUsage_GD_Wheel:
						{
							InputElement::Interpretation lInterpretation = InputElement::ABSOLUTE_AXIS;
							if (lCurrentElement->relative)
							{
								lInterpretation = InputElement::RELATIVE_AXIS;
							}
							lElement = new MacInputElement(InputElement::ANALOGUE, lInterpretation, mAnalogueCount, this, lCurrentElement);
							if (lCurrentElement->relative)
							{
								HIDQueueElement(mNativeDevice, lCurrentElement);
							}
							++mAnalogueCount;
						}
						break;
						default:
						{
							/*printf("Misc element:\n");
							printf("-------------\n");
							printf("name: %s\n", lCurrentElement->name);
							printf("usagePage: %li\nusage: %li\n", lCurrentElement->usagePage, lCurrentElement->usage);
							printf("usagename: %s\n", name);*/
						}
						break;
					}
				}
			}
			break;
			/*case kIOHIDElementTypeInput_ScanCodes:
			{
				printf("warning: kIOHIDElementTypeInput_ScanCodes support not implemented yet!\n");
				printf("min: %li\nmax: %li\nsize: %li\n", lCurrentElement->min, lCurrentElement->max, lCurrentElement->size);
			}
			break;
			default:
			{
				printf("unknown element type: %d\n", lCurrentElement->type);
			}*/
			break;
		}
		if (lElement)
		{
			mElementArray.push_back(lElement);
		}
		lCurrentElement = HIDGetNextDeviceElement(lCurrentElement, kHIDElementTypeInput);
	}
}



loginstance(UI_INPUT, MacInputDevice);



MacInputManager::MacInputManager(MacDisplayManager* pDisplayManager):
	mDisplayManager(pDisplayManager),
	mEnumError(false),
	mInitialized(false),
	mKeyModifiers(0),
	mScreenWidth(0),
	mScreenHeight(0),
	mCursorX(0),
	mCursorY(0),
	mKeyboard(0),
	mMouse(0)
{
	HIDBuildDeviceList(0, 0);
	if (HIDHaveDeviceList())
	{
		EnumDevices();
	}

	Refresh();

	AddObserver();

	mInitialized = true;
}

MacInputManager::~MacInputManager()
{
	if (mInitialized == true)
	{
		HIDReleaseDeviceList();
	}

	RemoveObserver();

	mDisplayManager = 0;
}

bool MacInputManager::IsInitialized()
{
	return (mInitialized);
}

void MacInputManager::Refresh()
{
	mScreenWidth  = mDisplayManager->GetWidth();
	mScreenHeight = mDisplayManager->GetHeight();
	/*if (mDisplayManager != 0 && mDisplayManager->GetHWND() != 0)
	{
		RECT lRect;
		::GetClientRect(mDisplayManager->GetHWND(), &lRect);
		
		mScreenWidth  = lRect.right - lRect.left;
		mScreenHeight = lRect.bottom - lRect.top;
	}
	else
	{
		// Get the entire screen area.
		mScreenWidth  = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		mScreenHeight = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		if (mScreenWidth == 0 || mScreenHeight == 0)
		{
			// Virtual screen not supported, use the primary display.
			mScreenWidth  = ::GetSystemMetrics(SM_CXSCREEN);
			mScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
		}
	}*/
}

MacInputManager::KeyCode MacInputManager::ConvertMacKeyCodeToKeyCode(unsigned pMacKeyCode)
{
	switch (pMacKeyCode)
	{
		case kVK_ANSI_A:		return IN_KBD_A;
		case kVK_ANSI_B:		return IN_KBD_B;
		case kVK_ANSI_C:		return IN_KBD_C;
		case kVK_ANSI_D:		return IN_KBD_D;
		case kVK_ANSI_E:		return IN_KBD_E;
		case kVK_ANSI_F:		return IN_KBD_F;
		case kVK_ANSI_G:		return IN_KBD_G;
		case kVK_ANSI_H:		return IN_KBD_H;
		case kVK_ANSI_I:		return IN_KBD_I;
		case kVK_ANSI_J:		return IN_KBD_J;
		case kVK_ANSI_K:		return IN_KBD_K;
		case kVK_ANSI_L:		return IN_KBD_L;
		case kVK_ANSI_M:		return IN_KBD_M;
		case kVK_ANSI_N:		return IN_KBD_N;
		case kVK_ANSI_O:		return IN_KBD_O;
		case kVK_ANSI_P:		return IN_KBD_P;
		case kVK_ANSI_Q:		return IN_KBD_Q;
		case kVK_ANSI_R:		return IN_KBD_R;
		case kVK_ANSI_S:		return IN_KBD_S;
		case kVK_ANSI_T:		return IN_KBD_T;
		case kVK_ANSI_U:		return IN_KBD_U;
		case kVK_ANSI_V:		return IN_KBD_V;
		case kVK_ANSI_W:		return IN_KBD_W;
		case kVK_ANSI_X:		return IN_KBD_X;
		case kVK_ANSI_Y:		return IN_KBD_Y;
		case kVK_ANSI_Z:		return IN_KBD_Z;
		case kVK_ANSI_1:		return IN_KBD_1;
		case kVK_ANSI_2:		return IN_KBD_2;
		case kVK_ANSI_3:		return IN_KBD_3;
		case kVK_ANSI_4:		return IN_KBD_4;
		case kVK_ANSI_5:		return IN_KBD_5;
		case kVK_ANSI_6:		return IN_KBD_6;
		case kVK_ANSI_7:		return IN_KBD_7;
		case kVK_ANSI_8:		return IN_KBD_8;
		case kVK_ANSI_9:		return IN_KBD_9;
		case kVK_ANSI_0:		return IN_KBD_0;
		case kVK_ANSI_Equal:		return IN_KBD_PLUS;
		case kVK_ANSI_Minus:		return IN_KBD_ACUTE;
		case kVK_ANSI_RightBracket:	return IN_KBD_AA;
		case kVK_ANSI_LeftBracket:	return IN_KBD_DIAERESIS;
		case kVK_ANSI_Quote:		return IN_KBD_OE;
		case kVK_ANSI_Semicolon:	return IN_KBD_AE;
		case kVK_ANSI_Backslash:	return IN_KBD_APOSTROPHE;
		case kVK_ANSI_Comma:		return IN_KBD_COMMA;
		case kVK_ANSI_Slash:		return IN_KBD_MINUS;
		case kVK_ANSI_Period:		return IN_KBD_DOT;
		case kVK_ANSI_Grave:		return IN_KBD_PARAGRAPH;
		case kVK_ANSI_KeypadDecimal:	return IN_KBD_NUMPAD_DOT;
		case kVK_ANSI_KeypadMultiply:	return IN_KBD_NUMPAD_MUL;
		case kVK_ANSI_KeypadPlus:	return IN_KBD_NUMPAD_PLUS;
		case kVK_ANSI_KeypadClear:	return IN_KBD_NUM_LOCK;
		case kVK_ANSI_KeypadDivide:	return IN_KBD_NUMPAD_DIV;
		case kVK_ANSI_KeypadEnter:	return IN_KBD_ENTER;
		case kVK_ANSI_KeypadMinus:	return IN_KBD_NUMPAD_MINUS;
		case kVK_ANSI_KeypadEquals:	return IN_KBD_NUM_LOCK;
		case kVK_ANSI_Keypad0:		return IN_KBD_NUMPAD_0;
		case kVK_ANSI_Keypad1:		return IN_KBD_NUMPAD_1;
		case kVK_ANSI_Keypad2:		return IN_KBD_NUMPAD_2;
		case kVK_ANSI_Keypad3:		return IN_KBD_NUMPAD_3;
		case kVK_ANSI_Keypad4:		return IN_KBD_NUMPAD_4;
		case kVK_ANSI_Keypad5:		return IN_KBD_NUMPAD_5;
		case kVK_ANSI_Keypad6:		return IN_KBD_NUMPAD_6;
		case kVK_ANSI_Keypad7:		return IN_KBD_NUMPAD_7;
		case kVK_ANSI_Keypad8:		return IN_KBD_NUMPAD_8;
		case kVK_ANSI_Keypad9:		return IN_KBD_NUMPAD_9;
		case kVK_Return:		return IN_KBD_ENTER;
		case kVK_Tab:			return IN_KBD_TAB;
		case kVK_Space:			return IN_KBD_SPACE;
		case kVK_Delete:		return IN_KBD_DEL;
		case kVK_Escape:		return IN_KBD_ESC;
		case kVK_Command:		return IN_KBD_LALT;
		case kVK_Shift:			return IN_KBD_LSHIFT;
		case kVK_CapsLock:		return IN_KBD_CAPS_LOCK;
		case kVK_Option:		return IN_KBD_LOS;
		case kVK_Control:		return IN_KBD_LCTRL;
		case kVK_RightShift:		return IN_KBD_RSHIFT;
		case kVK_RightOption:		return IN_KBD_ROS;
		case kVK_RightControl:		return IN_KBD_RCTRL;
		case kVK_Function:		return IN_KBD_CONTEXT_MENU;
		case kVK_F17:			return IN_KBD_F12;
		case kVK_VolumeUp:		return IN_KBD_QUICK_INCR_VOLUME;
		case kVK_VolumeDown:		return IN_KBD_QUICK_DECR_VOLUME;
		case kVK_Mute:			return IN_KBD_QUICK_SOUND_MUTE;
		case kVK_F18:			return IN_KBD_F12;
		case kVK_F19:			return IN_KBD_F12;
		case kVK_F20:			return IN_KBD_F12;
		case kVK_F5:			return IN_KBD_F5;
		case kVK_F6:			return IN_KBD_F6;
		case kVK_F7:			return IN_KBD_F7;
		case kVK_F3:			return IN_KBD_F3;
		case kVK_F8:			return IN_KBD_F8;
		case kVK_F9:			return IN_KBD_F9;
		case kVK_F11:			return IN_KBD_F11;
		case kVK_F13:			return IN_KBD_F12;
		case kVK_F16:			return IN_KBD_F12;
		case kVK_F14:			return IN_KBD_F12;
		case kVK_F10:			return IN_KBD_F10;
		case kVK_F12:			return IN_KBD_F12;
		case kVK_F15:			return IN_KBD_F12;
		case kVK_Help:			return IN_KBD_F1;
		case kVK_Home:			return IN_KBD_HOME;
		case kVK_PageUp:		return IN_KBD_PGUP;
		case kVK_ForwardDelete:		return IN_KBD_DEL;
		case kVK_F4:			return IN_KBD_F4;
		case kVK_End:			return IN_KBD_END;
		case kVK_F2:			return IN_KBD_F2;
		case kVK_PageDown:		return IN_KBD_PGDOWN;
		case kVK_F1:			return IN_KBD_F1;
		case kVK_LeftArrow:		return IN_KBD_LEFT;
		case kVK_RightArrow:		return IN_KBD_RIGHT;
		case kVK_DownArrow:		return IN_KBD_DOWN;
		case kVK_UpArrow:		return IN_KBD_UP;
		case kVK_ISO_Section:		return IN_KBD_PAUSE;
		case kVK_JIS_Yen:		return IN_KBD_PRINT_SCREEN;
		case kVK_JIS_Underscore:	return IN_KBD_PRINT_SCREEN;
		case kVK_JIS_KeypadComma:	return IN_KBD_NUMPAD_DOT;
		case kVK_JIS_Eisu:		return IN_KBD_NUM_LOCK;
		case kVK_JIS_Kana:		return IN_KBD_NUM_LOCK;
	}
	return IN_KBD_CONTEXT_MENU;
}

unichar MacInputManager::ConvertChar(unichar pChar)
{
	bool lIsChar;
	ConvertCharToKeyCode(pChar, false, lIsChar);
	if (!lIsChar)
	{
		return 0;
	}
	switch (pChar)
	{
		case 3:		return '\r';	// Numpad-ENTER.
		case 0x7F:	return '\b';
	}
	return pChar;
}

MacInputManager::KeyCode MacInputManager::ConvertCharToKeyCode(unichar pChar, bool pIsNumpad, bool& pIsChar)
{
	pIsChar = true;
#define NC()	pIsChar = false
	switch (pChar)
	{
		case NSUpArrowFunctionKey:	NC();	return IN_KBD_UP;
		case NSDownArrowFunctionKey:	NC();	return IN_KBD_DOWN;
		case NSLeftArrowFunctionKey:	NC();	return IN_KBD_LEFT;
		case NSRightArrowFunctionKey:	NC();	return IN_KBD_RIGHT;
		case NSF1FunctionKey:		NC();	return IN_KBD_F1;
		case NSF2FunctionKey:		NC();	return IN_KBD_F2;
		case NSF3FunctionKey:		NC();	return IN_KBD_F3;
		case NSF4FunctionKey:		NC();	return IN_KBD_F4;
		case NSF5FunctionKey:		NC();	return IN_KBD_F5;
		case NSF6FunctionKey:		NC();	return IN_KBD_F6;
		case NSF7FunctionKey:		NC();	return IN_KBD_F7;
		case NSF8FunctionKey:		NC();	return IN_KBD_F8;
		case NSF9FunctionKey:		NC();	return IN_KBD_F9;
		case NSF10FunctionKey:		NC();	return IN_KBD_F10;
		case NSF11FunctionKey:		NC();	return IN_KBD_F11;
		case NSF12FunctionKey:		NC();	return IN_KBD_F12;
		case 0xF746:	// Fall thru.
		case NSInsertFunctionKey:	NC();	return IN_KBD_INSERT;
		case NSDeleteFunctionKey:	NC();	return IN_KBD_DEL;
		case NSHomeFunctionKey:		NC();	return IN_KBD_HOME;
		case NSEndFunctionKey:		NC();	return IN_KBD_END;
		case NSPageUpFunctionKey:	NC();	return IN_KBD_PGUP;
		case NSPageDownFunctionKey:	NC();	return IN_KBD_PGDOWN;
		case NSSysReqFunctionKey:	// Fall thru.
		case NSPrintScreenFunctionKey:	NC();	return IN_KBD_PRINT_SCREEN;
		case NSScrollLockFunctionKey:	NC();	return IN_KBD_SCROLL_LOCK;
		case NSBreakFunctionKey:	// Fall thru.
		case NSPauseFunctionKey:	NC();	return IN_KBD_PAUSE;
		case NSMenuFunctionKey:		NC();	return IN_KBD_CONTEXT_MENU;
	}
	if (pChar >= 'a' && pChar <= 'z')
	{
		pChar -= 'a'-'A';
	}
	else if (pIsNumpad)
	{
		if (pChar >= '0' && pChar <= '9')
		{
			return (KeyCode)(IN_KBD_NUMPAD_0 + pChar - '0');
		}
		switch (pChar)
		{
			case '+':	return IN_KBD_NUMPAD_PLUS;
			case '-':	return IN_KBD_NUMPAD_MINUS;
			case '*':	return IN_KBD_NUMPAD_MUL;
			case '/':	return IN_KBD_NUMPAD_DIV;
			case '.':	// Fall thru.
			case ',':	return IN_KBD_NUMPAD_DOT;
		}
	}
	return (KeyCode)pChar;
}

MacDisplayManager* MacInputManager::GetDisplayManager() const
{
	return (mDisplayManager);
}

void MacInputManager::OnEvent(NSEvent* pEvent)
{
	bool lIsKeyDown = false;
	const NSEventType lType = [pEvent type];
	switch (lType)
	{
		case NSKeyDown:
		{
			lIsKeyDown = true;
		}	// TRICKY: fall thru to handle keyDown notification BEFORE OnChar is dispatched!
		case NSKeyUp:
		{
			const unsigned lMacKeyCode = [pEvent keyCode];	// Hardware independent key code.
			bool lConsumed = false;
			const KeyCode lKey = ConvertMacKeyCodeToKeyCode(lMacKeyCode);
			SetKey(lKey, lIsKeyDown);
			//mLog.Infof(_T("Got key %s: %x (%u, '%c')."), lIsKeyDown? _T("down") : _T("up"), lMacKeyCode, lMacKeyCode, lMacKeyCode);
			if (lIsKeyDown)
			{
				lConsumed |= NotifyOnKeyDown(lKey);
			}
			else
			{
				NotifyOnKeyUp(lKey);
			}

			// Handle OnChar dispatching.
			if (lIsKeyDown && !lConsumed)
			{
				const NSString* lCharacters = [pEvent characters];
				const unsigned lCharacterCount = [lCharacters length];
				for (unsigned x = 0; x < lCharacterCount; ++x)
				{
					unichar lChar = [lCharacters characterAtIndex:x];
					//mLog.Infof(_T("Got char: %x (%u, '%c')"), (unsigned)lChar, (unsigned)lChar, lChar);
					lChar = ConvertChar(lChar);
					if (lChar)
					{
						NotifyOnChar(lChar);
					}
				}
			}
		}
		break;
		case NSFlagsChanged:
		{
			const NSUInteger lKeyModifiers = [pEvent modifierFlags];
			//mLog.Infof(_T("Got key modifiers: %x (%u)"), lKeyModifiers, lKeyModifiers);
			const NSUInteger lDeltaModifiers = mKeyModifiers ^ lKeyModifiers;
			const int lTranslationTable[][2] =
			{
				{ UiLepra::InputManager::IN_KBD_CAPS_LOCK,	NSAlphaShiftKeyMask, },
				{ UiLepra::InputManager::IN_KBD_LSHIFT,		NSShiftKeyMask, },
				{ UiLepra::InputManager::IN_KBD_LCTRL,		NSControlKeyMask, },
				{ UiLepra::InputManager::IN_KBD_LALT,		NSAlternateKeyMask, },
				{ UiLepra::InputManager::IN_KBD_LOS,		NSCommandKeyMask, },
			};
			for (unsigned x = 0; x < LEPRA_ARRAY_COUNT(lTranslationTable); ++x)
			{
				if (!(lDeltaModifiers & lTranslationTable[x][1]))
				{
					continue;
				}
				const bool lIsKeyDown = (lKeyModifiers & lTranslationTable[x][1]);
				SetKey((KeyCode)lTranslationTable[x][0], lIsKeyDown);
				if (lIsKeyDown)
				{
					NotifyOnKeyDown((KeyCode)lTranslationTable[x][0]);
				}
				else
				{
					NotifyOnKeyUp((KeyCode)lTranslationTable[x][0]);
				}
			}
			mKeyModifiers = lKeyModifiers;
		}
		break;
		case NSMouseMoved:
		case NSLeftMouseDragged:
		case NSRightMouseDragged:
		{
			NSPoint lPoint = [pEvent locationInWindow];
			SetMousePosition(lPoint.x, lPoint.y);
		}
		break;
	}
}

void MacInputManager::SetCursorVisible(bool pVisible)
{
	if (pVisible)
	{
		CGDisplayShowCursor(kCGDirectMainDisplay);
		CGAssociateMouseAndMouseCursorPosition(true);
	}
	else
	{
		CGDisplayHideCursor(kCGDirectMainDisplay);
		CGAssociateMouseAndMouseCursorPosition(false);
	}
}

float MacInputManager::GetCursorX()
{
	return mCursorX;
}

float MacInputManager::GetCursorY()
{
	return mCursorY;
}

void MacInputManager::SetMousePosition(int x, int y)
{
	Parent::SetMousePosition(x, mScreenHeight-y);
	mCursorX = 2.0 * x / mScreenWidth  - 1.0;
	mCursorY = 2.0 * (mScreenHeight-y) / mScreenHeight - 1.0;
}

const InputDevice* MacInputManager::GetKeyboard() const
{
	return mKeyboard;
}

InputDevice* MacInputManager::GetKeyboard()
{
	return mKeyboard;
}

const InputDevice* MacInputManager::GetMouse() const
{
	return mMouse;
}

InputDevice* MacInputManager::GetMouse()
{
	return mMouse;
}

void MacInputManager::EnumDevices()
{
	pRecDevice lHIDDevice = HIDGetFirstDevice();
	while (lHIDDevice)
	{
		MacInputDevice* lDevice = new MacInputDevice(lHIDDevice, this);
		mDeviceList.push_back(lDevice);
		
		InputDevice::Interpretation lInterpretation = InputDevice::TYPE_OTHER;
		if (lHIDDevice->usagePage == kHIDPage_GenericDesktop &&
			(lHIDDevice->usage == kHIDUsage_GD_Pointer ||
			lHIDDevice->usage == kHIDUsage_GD_Mouse))
		{
			lInterpretation = InputDevice::TYPE_MOUSE;
		}
		else if (lHIDDevice->usagePage == kHIDPage_GenericDesktop &&
			(lHIDDevice->usage == kHIDUsage_GD_Keypad ||
			lHIDDevice->usage == kHIDUsage_GD_Keyboard))
		{
			lInterpretation = InputDevice::TYPE_KEYBOARD;
		}
		else if (lHIDDevice->usagePage == kHIDPage_GenericDesktop &&
			(lHIDDevice->usage == kHIDUsage_GD_Joystick ||
			lHIDDevice->usage == kHIDUsage_GD_MultiAxisController))
		{
			lInterpretation = InputDevice::TYPE_JOYSTICK;
		}
		else if (lHIDDevice->usagePage == kHIDPage_GenericDesktop &&
			lHIDDevice->usage == kHIDUsage_GD_GamePad)
		{
			lInterpretation = InputDevice::TYPE_GAMEPAD;
		}
		else
		{
			log_volatile(mLog.Debugf(_T("%s is not of known type: usage=%i, usagePage=%i."),
				lDevice->GetIdentifier().c_str(), lHIDDevice->usage, lHIDDevice->usagePage));
		}
		lDevice->SetInterpretation(lInterpretation, mTypeCount[lInterpretation]);
		++mTypeCount[lInterpretation];
		if (!mMouse && lInterpretation == InputDevice::TYPE_MOUSE)
		{
			mMouse = lDevice;
		}
		else if (!mKeyboard && lInterpretation == InputDevice::TYPE_KEYBOARD)
		{
			mKeyboard = lDevice;
		}
		
		lHIDDevice = HIDGetNextDevice(lHIDDevice);
	}
}
	
void MacInputManager::AddObserver()
{
	if (mDisplayManager)
	{
		// Listen to text input and standard mouse events.
		mDisplayManager->AddObserver(NSKeyDown, this);
		mDisplayManager->AddObserver(NSKeyUp, this);
		mDisplayManager->AddObserver(NSFlagsChanged, this);
		mDisplayManager->AddObserver(NSMouseMoved, this);
		mDisplayManager->AddObserver(NSLeftMouseDragged, this);
		mDisplayManager->AddObserver(NSRightMouseDragged, this);
	}
}

void MacInputManager::RemoveObserver()
{
	if (mDisplayManager)
	{
		mDisplayManager->RemoveObserver((MacInputManager*)this);
	}
}

loginstance(UI_INPUT, MacInputManager);



}

#endif // !iOS
