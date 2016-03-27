
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/X11/UiX11Input.h"
#include "../../../Lepra/Include/Log.h"
#include "../../Include/X11/UiX11Core.h"
#include "../../Include/X11/UiX11DisplayManager.h"



namespace UiLepra
{



InputManager* InputManager::CreateInputManager(DisplayManager* pDisplayManager)
{
	return (new X11InputManager((X11DisplayManager*)pDisplayManager));
}



X11InputElement::X11InputElement(Type pType, Interpretation pInterpretation, int pTypeIndex,
	X11InputDevice* pParentDevice, void* pRawElement, unsigned pFieldOffset):
	InputElement(pType, pInterpretation, pTypeIndex, pParentDevice),
	mRawElement(pRawElement)
{
	/*SetIdentifier(mElement->tszName);

	mDataFormat.dwType  = mElement->dwType;
	mDataFormat.pguid   = 0;
	mDataFormat.dwOfs   = (DWORD)pFieldOffset;
	mDataFormat.dwFlags = 0;*/
}

X11InputElement::~X11InputElement()
{
}


loginstance(UI_INPUT, X11InputElement);



/*
	class X11InputDevice
*/

X11InputDevice::X11InputDevice(void* pRawDevice, InputManager* pManager):
	InputDevice(pManager),
	mRawDevice(pRawDevice),
	mRelAxisCount(0),
	mAbsAxisCount(0),
	mAnalogueCount(0),
	mButtonCount(0)
{
	/*SetIdentifier(pInfo->tszInstanceName);

	mDIDevice->EnumObjects(EnumElementsCallback, this, DIDFT_ALL);

	int lNumElements = (int)mElementArray.size();

	// Input data to use in buffered mode.
	mDeviceObjectData = new DIDEVICEOBJECTDATA[lNumElements];
	memset(mDeviceObjectData, 0, lNumElements * sizeof(DIDEVICEOBJECTDATA));

	// Create the DirectInput data format description.
	memset(&mDataFormat, 0, sizeof(mDataFormat));
	mDataFormat.dwSize     = sizeof(mDataFormat);
	mDataFormat.dwObjSize  = sizeof(DIOBJECTDATAFORMAT);
	mDataFormat.dwDataSize = lNumElements * sizeof(unsigned);
	mDataFormat.dwNumObjs  = lNumElements;
	mDataFormat.rgodf      = new DIOBJECTDATAFORMAT[lNumElements];

	mDataFormat.dwFlags = 0;
	if (mAbsAxisCount > 0)
	{
		mDataFormat.dwFlags |= DIDF_ABSAXIS;
	}
	if (mRelAxisCount > 0)
	{
		mDataFormat.dwFlags |= DIDF_RELAXIS;
	}

	memset(mDataFormat.rgodf, 0, mDataFormat.dwObjSize * lNumElements);

	// Fill the data format description with the correct data.
	ElementArray::iterator lIter;
	int i;
	for (i = 0, lIter = mElementArray.begin(); 
		lIter != mElementArray.end(); 
		++i, ++lIter)
	{
		mDeviceObjectData[i].dwOfs = i * sizeof(unsigned);

		X11InputElement* lElement = (X11InputElement*)(*lIter);

		memcpy(&mDataFormat.rgodf[i], lElement->GetDataFormat(), sizeof(DIOBJECTDATAFORMAT));
	}*/
}

X11InputDevice::~X11InputDevice()
{
	if (IsActive() == true)
	{
		//mDIDevice->Unacquire();
	}
	//mDIDevice->Release();
}

void X11InputDevice::Activate()
{
	if (IsActive() == false)
	{
		/*HRESULT lHR;
		DWORD lCooperativeFlags = 0;
		if (this == GetManager()->GetKeyboard())
		{
			lCooperativeFlags |= DISCL_NOWINKEY;
		}
		lHR = mDIDevice->SetCooperativeLevel(((X11InputManager*)GetManager())->GetDisplayManager()->GetHWND(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND|lCooperativeFlags);
		lHR = mDIDevice->SetDataFormat(&mDataFormat);

		DIPROPDWORD lProp;
		lProp.diph.dwSize = sizeof(DIPROPDWORD);
		lProp.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		lProp.diph.dwHow = DIPH_DEVICE;
		lProp.diph.dwObj = 0;
		lProp.dwData = 1024;
		mDIDevice->SetProperty(DIPROP_BUFFERSIZE, &lProp.diph);*/

		//mReacquire = true;
		SetActive(true);
	}
}

void X11InputDevice::Release()
{
	//mDIDevice->Unacquire();
	SetActive(false);
}

void X11InputDevice::PollEvents()
{
	if (IsActive() == true)
	{
		/*if (mReacquire)
		{
			if (mDIDevice->Acquire() != DI_OK)
			{
				// System won't let us in yet. Keep trying.
				return;
			}
			mReacquire = false;
			log_debug(GetIdentifier()+_T(": acquired input device."));
		}*/


		/*HRESULT lHR = mDIDevice->Poll();
		if (lHR == DIERR_INPUTLOST)
		{
			mReacquire = true;
			log_debug(GetIdentifier()+_T(": lost input device."));
			return;
		}
		else if (lHR != DI_OK && lHR != DI_NOEFFECT)
		{
			mReacquire = true;
			mLog.Warningf((GetIdentifier() + _T(": Failed reaquiring device. Error=0x%8.8X.")).c_str(), lHR);
			return;
		}

		bool lMore = true;
		while (lMore)
		{
			DWORD lInOut = (DWORD)mElementArray.size();
			lHR = mDIDevice->GetDeviceData(sizeof(mDeviceObjectData[0]), mDeviceObjectData, &lInOut, 0);
			lMore = (lHR == DI_OK);

			for (unsigned i = 0; lMore && i < lInOut; i++)
			{
				// The following is a hack. I don't know if it works as 
				// intended on non-Swedish keyboards. The issue is 
				// that when pressing the right Alt key (Alt Gr), we will 
				// receive one Ctrl-event, and then one "Right Alt"-event
				// at the same time (on Swedish keyboards at least).
				if (i + 1 < lInOut && 
					mDeviceObjectData[i].dwTimeStamp == 
					mDeviceObjectData[i + 1].dwTimeStamp &&
					mDeviceObjectData[i + 1].dwOfs == 400)	// Right Alt at offset 400.
				{
					i++;
				}

				int lElementIndex = mDeviceObjectData[i].dwOfs / sizeof(unsigned);
				X11InputElement* lElement = (X11InputElement*)mElementArray[lElementIndex];

				if (lElement->GetType() == InputElement::ANALOGUE)
				{
					int lValue = mDeviceObjectData[i].dwData;
					lElement->SetValue(lValue);
				}
				else
				{
					int lValue = mDeviceObjectData[i].dwData;
					SetElementValue(lElement, (lValue&0x80)? 1.0 : 0.0);
				}
			}

			if (lInOut == 0)
			{
				lMore = false;
			}
		}*/
	}
}



loginstance(UI_INPUT, X11InputDevice);



X11InputManager::X11InputManager(X11DisplayManager* pDisplayManager):
	mDisplayManager(pDisplayManager),
	//mDirectInput(0),
	mEnumError(false),
	mInitialized(false),
	mScreenWidth(0),
	mScreenHeight(0),
	mCursorX(0),
	mCursorY(0),
	mKeyboard(0),
	mMouse(0)
{
	/*POINT lPoint;
	::GetCursorPos(&lPoint);
	SetMousePosition(WM_NCMOUSEMOVE, lPoint.x, lPoint.y);*/

	::memset(&mTypeCount, 0, sizeof(mTypeCount));

	/*HRESULT lHR;
	
	// Create the DirectInput object.
	lHR = DirectInput8Create(X11Core::GetAppInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(LPVOID*)&mDirectInput, 0);

	if (lHR != DI_OK)
	{
		mDisplayManager->ShowMessageBox(_T("DirectX 8 not supported (dinput creation failure)!"), _T("DirectInput error!"));
		return;
	}

	// Enumerate all devices.
	lHR = mDirectInput->EnumDevices(DI8DEVCLASS_ALL, EnumDeviceCallback, this, DIEDFL_ALLDEVICES);

	// mEnumError will be set if an error has occured.
	if (lHR != DI_OK || mEnumError == true)
	{
		mDisplayManager->ShowMessageBox(_T("DirectInput failed enumerating your devices!"), _T("DirectInput error!"));
		return;
	}*/

	Refresh();

	AddObserver();

	mInitialized = true;
}

X11InputManager::~X11InputManager()
{
	if (mInitialized == true)
	{
		//mDirectInput->Release();
	}

	RemoveObserver();

	mDisplayManager = 0;
}

void X11InputManager::Refresh()
{
	if (mDisplayManager != 0 && mDisplayManager->GetWindow() != 0)
	{
		/*RECT lRect;
		::GetClientRect(mDisplayManager->GetHWND(), &lRect);
		
		mScreenWidth  = lRect.right - lRect.left;
		mScreenHeight = lRect.bottom - lRect.top;*/
	}
	else
	{
		/*// Get the entire screen area.
		mScreenWidth  = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		mScreenHeight = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		if (mScreenWidth == 0 || mScreenHeight == 0)
		{
			// Virtual screen not supported, use the primary display.
			mScreenWidth  = ::GetSystemMetrics(SM_CXSCREEN);
			mScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
		}*/
	}
}

X11DisplayManager* X11InputManager::GetDisplayManager() const
{
	return (mDisplayManager);
}

bool X11InputManager::OnMessage(const XEvent& pEvent)
{
	bool lConsumed = false;
	switch (pEvent.type)
	{
		case KeyPress:
		case KeyRelease:
		{
			XKeyEvent& lKeyEvent = (XKeyEvent&)pEvent;
			char lKey[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
			KeySym lSym;
			::XLookupString(&lKeyEvent, lKey, sizeof(lKey), &lSym, NULL);
			mLog.Infof(_T("Key event %i: keycode=%i, state=%i, lookup=%s, keysym=%i"),
					lKeyEvent.type, lKeyEvent.keycode, lKeyEvent.state, lKey, lSym);
			KeyCode lKeyCode = TranslateKey(lKeyEvent.state, lSym);
			if (pEvent.type == KeyPress)
			{
				SetKey(lKeyCode, true);
				lConsumed = NotifyOnKeyDown(lKeyCode);

				if (!lConsumed && lKey[0])
				{
					wstr lWideKey = wstrutil::Encode(lKey);
					lConsumed |= NotifyOnChar(lWideKey[0]);
				}
			}
			else
			{
				SetKey(lKeyCode, false);
				lConsumed = NotifyOnKeyUp(lKeyCode);
			}
		}
		break;
		case ButtonPress:
		case ButtonRelease:
		{
			// TODO: handle mouse button press.
		}
		break;
		case MotionNotify:
		{
			// TODO: handle mouse move.
		}
		break;
	}
	return (lConsumed);
}

X11InputManager::KeyCode X11InputManager::TranslateKey(int pState, KeySym pKeySym)
{
	switch (pKeySym)
	{
		case 65288:	return IN_KBD_BACKSPACE;
		case 65289:	return IN_KBD_TAB;
		case 65293:	return IN_KBD_ENTER;
		case 65421:	return IN_KBD_ENTER;	// Numpad.
		case 65437:	return IN_KBD_CENTER;
		case 65505:	return IN_KBD_LSHIFT;
		case 65507:	return IN_KBD_LCTRL;
		case 65513:	return IN_KBD_LALT;
		case 65299:	return IN_KBD_PAUSE;
		case 65509:	return IN_KBD_CAPS_LOCK;

		case 65307:	return IN_KBD_ESC;

		case 65365:	return IN_KBD_PGUP;
		case 65366:	return IN_KBD_PGDOWN;
		case 65367:	return IN_KBD_END;
		case 65360:	return IN_KBD_HOME;
		case 65361:	return IN_KBD_LEFT;
		case 65362:	return IN_KBD_UP;
		case 65363:	return IN_KBD_RIGHT;
		case 65364:	return IN_KBD_DOWN;
		case 65379:	return IN_KBD_INSERT;
		case 65535:	return IN_KBD_DEL;

		case 65516:	return IN_KBD_ROS;
		case 65383:	return IN_KBD_CONTEXT_MENU;

		case 65456:	return IN_KBD_NUMPAD_0;
		case 65457:	return IN_KBD_NUMPAD_1;
		case 65458:	return IN_KBD_NUMPAD_2;
		case 65459:	return IN_KBD_NUMPAD_3;
		case 65460:	return IN_KBD_NUMPAD_4;
		case 65461:	return IN_KBD_NUMPAD_5;
		case 65462:	return IN_KBD_NUMPAD_6;
		case 65463:	return IN_KBD_NUMPAD_7;
		case 65464:	return IN_KBD_NUMPAD_8;
		case 65465:	return IN_KBD_NUMPAD_9;

		case 65455:	return IN_KBD_NUMPAD_DIV;
		case 65450:	return IN_KBD_NUMPAD_MUL;
		case 65453:	return IN_KBD_NUMPAD_MINUS;
		case 65451:	return IN_KBD_NUMPAD_PLUS;
		case 65452:	return IN_KBD_NUMPAD_DOT;

		case 65470:	return IN_KBD_F1;
		case 65471:	return IN_KBD_F2;
		case 65472:	return IN_KBD_F3;
		case 65473:	return IN_KBD_F4;
		case 65474:	return IN_KBD_F5;
		case 65475:	return IN_KBD_F6;
		case 65476:	return IN_KBD_F7;
		case 65477:	return IN_KBD_F8;
		case 65478:	return IN_KBD_F9;
		case 65479:	return IN_KBD_F10;
		case 65480:	return IN_KBD_F11;
		case 65481:	return IN_KBD_F12;

		case 65407:	return IN_KBD_NUM_LOCK;
		case 65300:	return IN_KBD_SCROLL_LOCK;

		case 65111:	return IN_KBD_DIAERESIS;
		case '+':	return IN_KBD_PLUS;
		case ',':	return IN_KBD_COMMA;
		case '.':	return IN_KBD_DOT;
		case '-':	return IN_KBD_MINUS;
		case 65104:	return IN_KBD_APOSTROPHE;
		case 65105:	return IN_KBD_APOSTROPHE;

		case 246:	return IN_KBD_OE;
		case 167:	return IN_KBD_PARAGRAPH;
		case 229:	return IN_KBD_AA;
		case 228:	return IN_KBD_AE;

		case '<':	return IN_KBD_COMPARE;
		case '>':	return IN_KBD_COMPARE;

		case 65506:	return IN_KBD_RSHIFT;
		case 65508:	return IN_KBD_RCTRL;
		case 65027:	return IN_KBD_RALT;
	}
	if (pKeySym >= 'a' && pKeySym <= 'z')
	{
		return KeyCode(pKeySym-'a'+'A');
	}
	return (KeyCode)pKeySym;
}

void X11InputManager::SetCursorVisible(bool pVisible)
{
	//::ShowCursor(pVisible? TRUE : FALSE);
}

float X11InputManager::GetCursorX()
{
	return mCursorX;
}

float X11InputManager::GetCursorY()
{
	return mCursorY;
}

void X11InputManager::SetMousePosition(int x, int y)
{
	Parent::SetMousePosition(x, y);
	mCursorX = 2.0f * x / mScreenWidth  - 1.0f;
	mCursorY = 2.0f * y / mScreenHeight - 1.0f;
}

const InputDevice* X11InputManager::GetKeyboard() const
{
	return mKeyboard;
}

InputDevice* X11InputManager::GetKeyboard()
{
	return mKeyboard;
}

const InputDevice* X11InputManager::GetMouse() const
{
	return mMouse;
}

InputDevice* X11InputManager::GetMouse()
{
	return mMouse;
}

void X11InputManager::AddObserver()
{
	if (mDisplayManager)
	{
		// Listen to text input and standard mouse events.
		mDisplayManager->AddObserver(KeyPress, this);
		mDisplayManager->AddObserver(KeyRelease, this);
		mDisplayManager->AddObserver(ButtonPress, this);
		mDisplayManager->AddObserver(ButtonRelease, this);
		mDisplayManager->AddObserver(MotionNotify, this);
	}
}

void X11InputManager::RemoveObserver()
{
	if (mDisplayManager)
	{
		mDisplayManager->RemoveObserver(this);
	}
}

bool X11InputManager::IsInitialized()
{
	return mInitialized;
}



loginstance(UI_INPUT, X11InputManager);



}
