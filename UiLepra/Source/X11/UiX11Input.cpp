
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/X11/UiX11Input.h"
#include "../../../Lepra/Include/Log.h"
#include "../../Include/X11/UiX11Core.h"
#include "../../Include/X11/UiX11DisplayManager.h"
#include <X11/cursorfont.h>

#define MOUSE_BUTTON_COUNT	10
#define MOUSE_AXIS_COUNT	10


namespace UiLepra
{



InputManager* InputManager::CreateInputManager(DisplayManager* pDisplayManager)
{
	return (new X11InputManager((X11DisplayManager*)pDisplayManager));
}



X11InputElement::X11InputElement(Type pType, Interpretation pInterpretation, int pTypeIndex, X11InputDevice* pParentDevice):
	InputElement(pType, pInterpretation, pTypeIndex, pParentDevice)
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

X11InputDevice::X11InputDevice(InputManager* pManager):
	InputDevice(pManager)
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

void X11InputDevice::AddElement(X11InputElement* pElement)
{
	mElementArray.push_back(pElement);
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
}



loginstance(UI_INPUT, X11InputDevice);



X11InputManager::X11InputManager(X11DisplayManager* pDisplayManager):
	mDisplayManager(pDisplayManager),
	//mDirectInput(0),
	mInitialized(false),
	mScreenWidth(0),
	mScreenHeight(0),
	mCursorX(0),
	mCursorY(0),
	mIgnoreNextMouseMove(true),
	mGrabCursor(false),
	mMouseGrabX(0),
	mMouseGrabY(0),
	mMouseGrabDeltaX(0),
	mMouseGrabDeltaY(0),
	mKeyboard(0),
	mMouse(0)
{
	XIM lInputMethod = XOpenIM(mDisplayManager->GetDisplay(), NULL, NULL, NULL);
	mInputContext = XCreateIC(lInputMethod, XNInputStyle, XIMPreeditNothing|XIMStatusNothing, XNClientWindow, mDisplayManager->GetWindow(), NULL);;

	Window rw, cw;
	int _, x, y;
	unsigned lMask;
	XQueryPointer(mDisplayManager->GetDisplay(), mDisplayManager->GetWindow(), &rw, &cw, &_, &_, &x, &y, &lMask);
	SetMousePosition(x, y);
	mMouseGrabX = x;
	mMouseGrabY = y;

	::memset(&mTypeCount, 0, sizeof(mTypeCount));
	++mTypeCount[InputDevice::TYPE_KEYBOARD];
	++mTypeCount[InputDevice::TYPE_MOUSE];
	mKeyboard = new X11InputDevice(this);
	mKeyboard->SetInterpretation(InputDevice::TYPE_KEYBOARD, 0);
	X11InputDevice* lMouse = new X11InputDevice(this);
	mMouse = lMouse;
	mMouse->SetInterpretation(InputDevice::TYPE_MOUSE, 0);
	for (int x = 0; x < MOUSE_BUTTON_COUNT; ++x)
	{
		lMouse->AddElement(new X11InputElement(InputElement::DIGITAL, InputElement::BUTTON, x, lMouse));
	}
	for (int x = 0; x < MOUSE_AXIS_COUNT; ++x)
	{
		lMouse->AddElement(new X11InputElement(InputElement::ANALOGUE, InputElement::RELATIVE_AXIS, x, lMouse));
	}
	mDeviceList.push_back(mKeyboard);
	mDeviceList.push_back(mMouse);

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

void X11InputManager::PreProcessEvents()
{
	mMouse->GetAxis(0)->SetValue(0);
	mMouse->GetAxis(1)->SetValue(0);
	mMouse->GetAxis(2)->SetValue(0);

	// Warp pointer.
	if (mGrabCursor)
	{
		const int sx = mScreenWidth/2;
		const int sy = mScreenHeight/2;
		Window rw, cw;
		int _, x, y;
		unsigned lMask;
		XQueryPointer(mDisplayManager->GetDisplay(), mDisplayManager->GetWindow(), &rw, &cw, &_, &_, &x, &y, &lMask);
		if (abs(x-sx) >= 100 || abs(y-sy) >= 100)
		{
			//mLog.Infof(_T("mwarp: (%i; %i)"), sx-x, sy-y);
			mMouseGrabDeltaX += sx - x;
			mMouseGrabDeltaY += sy - y;
			XWarpPointer(mDisplayManager->GetDisplay(), None, mDisplayManager->GetWindow(),
					0, 0, 0, 0, sx, sy);
		}
	}
}

void X11InputManager::PollEvents()
{
	Parent::PollEvents();

	if (mMouseGrabDeltaX || mMouseGrabDeltaY)
	{
		mMouseGrabX += mMouseGrabDeltaX;
		mMouseGrabY += mMouseGrabDeltaY;
		mMouseGrabDeltaX = mMouseGrabDeltaY = 0;
	}
}

void X11InputManager::Refresh()
{
	if (mDisplayManager != 0)
	{
		mScreenWidth  = mDisplayManager->GetWidth();
		mScreenHeight = mDisplayManager->GetHeight();
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
			if (pEvent.type == KeyPress)
			{
				::Xutf8LookupString(mInputContext, &lKeyEvent, lKey, sizeof(lKey), &lSym, NULL);
			}
			else
			{
				::XLookupString(&lKeyEvent, lKey, sizeof(lKey), &lSym, NULL);
			}
			KeyCode lKeyCode = TranslateKey(lKeyEvent.state, lSym);
			//mLog.Infof(_T("Key event %i: keycode=%i, state=%i, lookup=%s, keysym=%i, keycode=%i"),
			//		lKeyEvent.type, lKeyEvent.keycode, lKeyEvent.state, lKey, lSym, lKeyCode);
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
				if (XEventsQueued(mDisplayManager->GetDisplay(), QueuedAfterReading))
				{
					XEvent e;
					XPeekEvent(mDisplayManager->GetDisplay(), &e);
					if (e.type == KeyPress &&
						e.xkey.time == lKeyEvent.time &&
						e.xkey.keycode == lKeyEvent.keycode)
					{
						lConsumed = true;
						break;
					}
				}
				SetKey(lKeyCode, false);
				lConsumed = NotifyOnKeyUp(lKeyCode);
			}
		}
		break;
		case ButtonPress:
		case ButtonRelease:
		{
			const XButtonEvent& lButton = (const XButtonEvent&)pEvent;
			int lButtonIndex = lButton.button-1;
			if (lButtonIndex == 1)
			{
				lButtonIndex = 2;
			}
			else if (lButtonIndex == 2)
			{
				lButtonIndex = 1;
			}
			mMouse->GetButton(lButtonIndex)->SetValue((pEvent.type == ButtonPress)? 1.0f : 0.0f);
			if (lButtonIndex == 3)
			{
				mMouse->GetAxis(2)->AddValue(+1);
			}
			if (lButtonIndex == 4)
			{
				mMouse->GetAxis(2)->AddValue(-1);
			}
		}
		break;
		case MotionNotify:
		{
			const XMotionEvent& lMotion = (const XMotionEvent&)pEvent;
			SetMousePosition(lMotion.x, lMotion.y);
			const int x = lMotion.x;
			const int y = lMotion.y;
			const int dx = x - mMouseGrabX;
			const int dy = y - mMouseGrabY;
			mMouseGrabX = x;
			mMouseGrabY = y;
			if (mIgnoreNextMouseMove)
			{
				mIgnoreNextMouseMove = false;
				break;
			}
			//mLog.Infof(_T("mmove: (%i; %i), cnt=%i"), dx, dy, mWarpCount);
			mMouse->GetAxis(0)->AddValue(dx);
			mMouse->GetAxis(1)->AddValue(dy);
		}
		break;
		case FocusIn:
		{
			if (!mIsCursorVisible)
			{
				mIsCursorVisible = true;
				SetCursorVisible(false);
			}
			mIgnoreNextMouseMove = true;
		}
		break;
		case FocusOut:
		{
			if (!mIsCursorVisible)
			{
				SetCursorVisible(true);
				mIsCursorVisible = false;
			}
			mIgnoreNextMouseMove = true;
		}
		break;
		case EnterNotify:
		{
			mIgnoreNextMouseMove = true;
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
	if (mIsCursorVisible != pVisible)
	{
		if (pVisible)
		{
			log_volatile(mLog.Debug(_T("Showing cursor.")));
			Cursor lCursor = XCreateFontCursor(mDisplayManager->GetDisplay(), XC_X_cursor);
			XDefineCursor(mDisplayManager->GetDisplay(), mDisplayManager->GetWindow(), lCursor);
			XFreeCursor(mDisplayManager->GetDisplay(), lCursor);

			XUngrabPointer(mDisplayManager->GetDisplay(), CurrentTime);
			mGrabCursor = false;
		}
		else
		{
			log_volatile(mLog.Debug(_T("Hiding cursor.")));
			XColor lBlack;
			lBlack.red = lBlack.green = lBlack.blue = 0;
			static char noData[] = { 0,0,0,0,0,0,0,0 };
			Pixmap lBitmapNoData = XCreateBitmapFromData(mDisplayManager->GetDisplay(), mDisplayManager->GetWindow(), noData, 8, 8);
			Cursor lInvisibleCursor = XCreatePixmapCursor(mDisplayManager->GetDisplay(), lBitmapNoData, lBitmapNoData, 
								&lBlack, &lBlack, 0, 0);
			XDefineCursor(mDisplayManager->GetDisplay(), mDisplayManager->GetWindow(), lInvisibleCursor);
			XFreeCursor(mDisplayManager->GetDisplay(), lInvisibleCursor);
			XFreePixmap(mDisplayManager->GetDisplay(), lBitmapNoData);

			XGrabPointer(mDisplayManager->GetDisplay(), mDisplayManager->GetWindow(), True, 0, GrabModeAsync, GrabModeAsync, mDisplayManager->GetWindow(), None, CurrentTime);
			mGrabCursor = true;
		}
		mIsCursorVisible = pVisible;
	}
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
		mDisplayManager->AddObserver(FocusIn, this);
		mDisplayManager->AddObserver(FocusOut, this);
		mDisplayManager->AddObserver(EnterNotify, this);
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
