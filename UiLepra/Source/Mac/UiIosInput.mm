
// Author: Jonas Bystrm
// Copyright (c) Pixel Doctrine



#include "../../Include/Mac/UiIosInput.h"
#ifdef LEPRA_IOS
//#include "../../../Lepra/Include/CyclicArray.h"
//#include "../../../Lepra/Include/Log.h"
//#include "../../Include/Mac/UiMacCore.h"
//#include "../../Include/Mac/UiMacOpenGLDisplayManager.h"



namespace UiLepra
{



InputManager* InputManager::CreateInputManager(DisplayManager* pDisplayManager)
{
	return (new IosInputManager((MacOpenGLDisplay*)pDisplayManager));
}



IosInputElement::IosInputElement(Type pType, Interpretation pInterpretation, int pTypeIndex,
	IosInputDevice* pParentDevice):
	InputElement(pType, pInterpretation, pTypeIndex, pParentDevice)
{
}

IosInputElement::~IosInputElement()
{
}

loginstance(UI_INPUT, IosInputElement);



/*
	class IosInputDevice
*/

IosInputDevice::IosInputDevice(InputManager* pManager):
	InputDevice(pManager),
	mRelAxisCount(0),
	mAbsAxisCount(0),
	mAnalogueCount(0),
	mButtonCount(0)
{
}

IosInputDevice::~IosInputDevice()
{
	if (IsActive() == true)
	{
		Release();
	}
	//mNativeDevice->Release();

	ElementArray::iterator x;
	for (x = mElementArray.begin(); x != mElementArray.end(); ++x)
	{
		InputElement* lElement = *x;
		delete lElement;
	}
	mElementArray.clear();
}

void IosInputDevice::Activate()
{
	if (IsActive() == false)
	{
		//mNativeDevice->Acquire();
		SetActive(true);
	}
}

void IosInputDevice::Release()
{
	//mNativeDevice->Unacquire();
	SetActive(false);
}

void IosInputDevice::PollEvents()
{
	if (IsActive() == true)
	{
		ElementArray::iterator x;
		for (x = mElementArray.begin(); x != mElementArray.end(); ++x)
		{
			IosInputElement* lElement = (IosInputElement*)*x;
			lElement->SetValue(lElement->GetValue());	// TODO: do some stuff?
		}
	}
}

void IosInputDevice::AddElement(InputElement* pElement)
{
	mElementArray.push_back(pElement);
}



loginstance(UI_INPUT, IosInputDevice);



IosInputManager::IosInputManager(MacOpenGLDisplay* pDisplayManager):
	mDisplayManager(pDisplayManager),
	mScreenWidth(0),
	mScreenHeight(0),
	mCursorX(0),
	mCursorY(0),
	mKeyboard(0),
	mMouse(0)
{
	Refresh();

	AddObserver();

	IosInputDevice* lTouch = new IosInputDevice(this);
	IosInputElement* x = new IosInputElement(InputElement::ANALOGUE, InputElement::RELATIVE_AXIS, 0, lTouch);
	IosInputElement* y = new IosInputElement(InputElement::ANALOGUE, InputElement::RELATIVE_AXIS, 1, lTouch);
	IosInputElement* lButton = new IosInputElement(InputElement::DIGITAL, InputElement::BUTTON, 0, lTouch);
	lTouch->SetIdentifier(_T("IosVirtualMouse"));
	x->SetIdentifier(_T("RelAxisX"));
	y->SetIdentifier(_T("RelAxisY"));
	lButton->SetIdentifier(_T("Button"));
	lTouch->AddElement(x);
	lTouch->AddElement(y);
	lTouch->AddElement(lButton);
	x->SetValue(0);
	y->SetValue(0);
	lButton->SetValue(1);
	x->SetValue(mScreenWidth);
	y->SetValue(mScreenHeight);
	lButton->SetValue(0);
	mDeviceList.push_back(lTouch);
	mMouse = lTouch;

	mInitialized = true;
}

IosInputManager::~IosInputManager()
{
	if (mInitialized == true)
	{
		//HIDReleaseDeviceList();
	}

	RemoveObserver();

	mDisplayManager = 0;
}

bool IosInputManager::IsInitialized()
{
	return (mInitialized);
}

void IosInputManager::Refresh()
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

MacDisplayManager* IosInputManager::GetDisplayManager() const
{
	return (mDisplayManager);
}

void IosInputManager::ActivateKeyboard()
{
	EAGLView* lView = mDisplayManager->GetGlView();
	[lView becomeFirstResponder];
}

void IosInputManager::ReleaseKeyboard()
{
	EAGLView* lView = mDisplayManager->GetGlView();
	[lView resignFirstResponder];
}

void IosInputManager::SetCursorVisible(bool pVisible)
{
	(void)pVisible;
}

float IosInputManager::GetCursorX()
{
	return mCursorX;
}

float IosInputManager::GetCursorY()
{
	return mCursorY;
}

void IosInputManager::SetMousePosition(int x, int y)
{
	mCursorX = 2.0 * x / mScreenWidth  - 1.0;
	mCursorY = 2.0 * y / mScreenHeight - 1.0;
}

const InputDevice* IosInputManager::GetKeyboard() const
{
	return mKeyboard;
}

InputDevice* IosInputManager::GetKeyboard()
{
	return mKeyboard;
}

const InputDevice* IosInputManager::GetMouse() const
{
	return mMouse;
}

InputDevice* IosInputManager::GetMouse()
{
	return mMouse;
}

void IosInputManager::OnEvent(LEPRA_APPLE_EVENT* e)
{
}

void IosInputManager::AddObserver()
{
	if (mDisplayManager)
	{
		EAGLView* lView = mDisplayManager->GetGlView();
		lView.inputManager = this;
	}
}

void IosInputManager::RemoveObserver()
{
	if (mDisplayManager)
	{
		mDisplayManager->RemoveObserver(this);
	}
}

loginstance(UI_INPUT, IosInputManager);



}

#endif // iOS
