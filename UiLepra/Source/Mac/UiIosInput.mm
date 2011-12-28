
// Author: Jonas Bystršm
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/Mac/UiIosInput.h"
#ifdef LEPRA_IOS
//#include "../../../Lepra/Include/CyclicArray.h"
//#include "../../../Lepra/Include/Log.h"
//#include "../../Include/Mac/UiMacCore.h"
#include "../../Include/Mac/UiMacDisplayManager.h"



namespace UiLepra
{



InputManager* InputManager::CreateInputManager(DisplayManager* pDisplayManager)
{
	return (new IosInputManager((MacDisplayManager*)pDisplayManager));
}



IosInputElement::IosInputElement(Type pType, Interpretation pInterpretation, int pTypeIndex,
	IosInputDevice* pParentDevice):
	InputElement(pType, pInterpretation, pTypeIndex, pParentDevice),
	mMin(MAX_INT),
	mMax(MIN_INT)
{
}

IosInputElement::~IosInputElement()
{
}

void IosInputElement::SetValue(int pValue)
{
	// Calibrate...
	if (pValue < mMin)
	{
		mMin = pValue;
	}
	if (pValue > mMax)
	{
		mMax = pValue;
	}

	if (GetType() == DIGITAL)
	{
		Parent::SetValue((float)pValue);
	}
	else if (mMin < mMax)
	{
		// Scale to +-1.
		Parent::SetValue((pValue*2.0f-(mMax+mMin)) / (float)(mMax-mMin));
	}
}

str IosInputElement::GetCalibration() const
{
	str lData;
	lData += strutil::IntToString(mMin, 10);
	lData += _T(", ");
	lData += strutil::IntToString(mMax, 10);
	return (lData);
}

bool IosInputElement::SetCalibration(const str& pData)
{
	bool lOk = false;
	strutil::strvec lData = strutil::Split(pData, _T(", "));
	if (lData.size() >= 2)
	{
		lOk = true;
		lOk &= strutil::StringToInt(lData[0], mMin, 10);
		lOk &= strutil::StringToInt(lData[1], mMax, 10);
	}
	return (lOk);
}



LOG_CLASS_DEFINE(UI_INPUT, IosInputElement);



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

bool IosInputDevice::HaveRelativeAxes()
{
	return (mRelAxisCount > mAbsAxisCount);
}

void IosInputDevice::AddElement(InputElement* pElement)
{
	mElementArray.push_back(pElement);
}



LOG_CLASS_DEFINE(UI_INPUT, IosInputDevice);



IosInputManager::IosInputManager(MacDisplayManager* pDisplayManager):
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
	IosInputElement* lButton = new IosInputElement(InputElement::DIGITAL, InputElement::BUTTON1, 0, lTouch);
	IosInputElement* x = new IosInputElement(InputElement::ANALOGUE, InputElement::RELATIVE_AXIS, 0, lTouch);
	IosInputElement* y = new IosInputElement(InputElement::ANALOGUE, InputElement::RELATIVE_AXIS, 1, lTouch);
	lTouch->AddElement(lButton);
	lTouch->AddElement(x);
	lTouch->AddElement(y);
	lButton->SetValue(1);
	x->SetValue(0);
	y->SetValue(0);
	lButton->SetValue(0);
	x->SetValue(mScreenWidth);
	y->SetValue(mScreenHeight);
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

	DeviceList::iterator lDIter;
	for (lDIter = mDeviceList.begin(); lDIter != mDeviceList.end(); ++lDIter)
	{
		InputDevice* lDevice = *lDIter;
		delete lDevice;
	}

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

void IosInputManager::ShowCursor()
{
}

void IosInputManager::HideCursor()
{
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
	mCursorY = 2.0 * (mScreenHeight-y) / mScreenHeight - 1.0;
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
		// Listen to text input and standard mouse events.
		//mDisplayManager->AddObserver(NSKeyDown, this);
	}
}

void IosInputManager::RemoveObserver()
{
	if (mDisplayManager)
	{
		mDisplayManager->RemoveObserver(this);
	}
}

LOG_CLASS_DEFINE(UI_INPUT, IosInputManager);



}

#endif // iOS
