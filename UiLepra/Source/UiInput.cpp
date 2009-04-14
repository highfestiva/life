
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/UiInput.h"
#include <math.h>



namespace UiLepra
{



InputElement::InputElement(Type pType, InputDevice* pParentDevice):
	mPrevValue(0),
	mValue(0),
	mType(pType),
	mInterpretation(NO_INTERPRETATION),
	mParentDevice(pParentDevice),
	mFunctor(0)
{
}

InputElement::~InputElement()
{
	SetFunctor(0);
}

InputElement::Type InputElement::GetType() const
{
	return mType;
}

InputElement::Interpretation InputElement::GetInterpretation() const
{
	return mInterpretation;
}

void InputElement::SetInterpretation(Interpretation pInterpretation)
{
	mInterpretation = pInterpretation;
}

InputDevice* InputElement::GetParentDevice() const
{
	return mParentDevice;
}

bool InputElement::GetBooleanValue(Lepra::float64 pThreshold) const
{
	return (mValue >= pThreshold);
}

Lepra::float64 InputElement::GetValue() const
{
	return mValue;
}

Lepra::float64 InputElement::GetDeltaValue() const
{
	return (mValue - mPrevValue);
}

const Lepra::String& InputElement::GetIdentifier() const
{
	return mIdentifier;
}

void InputElement::SetIdentifier(const Lepra::String& pIdentifier)
{
	mIdentifier = pIdentifier;
}

void InputElement::SetFunctor(InputFunctor* pFunctor)
{
	if (mFunctor != 0)
	{
		delete mFunctor;
	}

	mFunctor = pFunctor;
}

const InputFunctor* InputElement::GetFunctor()
{
	return mFunctor;
}

void InputElement::SetValue(Lepra::float64 pNewValue)
{
	static const Lepra::float64 lEpsilon = 1e-15;

	if (fabs(pNewValue - mValue) > lEpsilon)
	{
		mPrevValue = mValue;
		mValue = pNewValue;

		// Notify our observer.
		if (mFunctor != 0)
		{
			mFunctor->Call(this);
		}
	}
}



InputDevice::InputDevice(InputManager* pManager):
	mManager(pManager),
	mNumDigitalElements(-1),
	mNumAnalogueElements(-1),
	mActive(false)
{
}

InputDevice::~InputDevice()
{
}

InputManager* InputDevice::GetManager() const
{
	return (mManager);
}

bool InputDevice::IsActive()
{
	return mActive;
}

void InputDevice::SetActive(bool pActive)
{
	mActive = pActive;
}

void InputDevice::SetElementValue(InputElement* pElement, Lepra::float64 pValue)
{
	pElement->SetValue(pValue);
}

const InputDevice::ElementArray& InputDevice::GetElements() const
{
	return mElementArray;
}

InputElement* InputDevice::GetElement(unsigned pElementIndex) const
{
	return mElementArray[pElementIndex];
}

unsigned InputDevice::GetNumElements() const
{
	return ((unsigned)mElementArray.size());
}

unsigned InputDevice::GetNumDigitalElements()
{
	if (mNumDigitalElements == -1)
	{
		CountElements();
	}
	return mNumDigitalElements;
}

unsigned InputDevice::GetNumAnalogueElements()
{
	if (mNumAnalogueElements == -1)
	{
		CountElements();
	}
	return mNumAnalogueElements;
}

void InputDevice::SetIdentifier(const Lepra::String& pIdentifier)
{
	mIdentifier = pIdentifier;
}

const Lepra::String& InputDevice::GetIdentifier() const
{
	return mIdentifier;
}

const InputElement* InputDevice::GetElement(const Lepra::String& pIdentifier) const
{
	ElementArray::const_iterator x;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement->GetIdentifier() == pIdentifier)
		{
			return lElement;
		}
	}

	return 0;
}


InputElement* InputDevice::GetElement(const Lepra::String& pIdentifier)
{
	ElementArray::iterator x;

	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement->GetIdentifier() == pIdentifier)
		{
			return lElement;
		}
	}

	return 0;
}

unsigned InputDevice::GetElementIndex(InputElement* pElement) const
{
	ElementArray::const_iterator x;

	int i = 0;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement == pElement)
		{
			return i;
		}
		i++;
	}

	return ((unsigned)-1);
}

InputElement* InputDevice::GetButton(unsigned pButtonIndex) const
{
	ElementArray::const_iterator x;

	unsigned lCurrentButton = 0;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement->GetType() == InputElement::DIGITAL)
		{
			if (lCurrentButton == pButtonIndex)
			{
				return lElement;
			}
			lCurrentButton++;
		}
	}

	return 0;
}

InputElement* InputDevice::GetAxis(unsigned pAxisIndex) const
{
	ElementArray::const_iterator x;

	unsigned lCurrentAxis = 0;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement->GetType() == InputElement::ANALOGUE)
		{
			if (lCurrentAxis == pAxisIndex)
			{
				return lElement;
			}
			lCurrentAxis++;
		}
	}

	return 0;
}



void InputDevice::CountElements()
{
	mNumDigitalElements  = 0;
	mNumAnalogueElements = 0;

	ElementArray::iterator x;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;

		if (lElement->GetType() == InputElement::DIGITAL)
		{
			mNumDigitalElements++;
		}
		else if(lElement->GetType() == InputElement::ANALOGUE)
		{
			mNumAnalogueElements++;
		}
	}
}

void InputDevice::SetFunctor(const Lepra::String& pElementIdentifier,
							 InputFunctor* pFunctor)
{
	ElementArray::iterator x;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement->GetIdentifier() == pElementIdentifier)
		{
			lElement->SetFunctor(pFunctor);
		}
	}
}

void InputDevice::SetFunctor(InputFunctor* pFunctor)
{
	ElementArray::iterator x;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		lElement->SetFunctor(pFunctor->CreateCopy());
	}

	delete pFunctor;
}

unsigned InputDevice::GetCalibrationDataSize()
{
	unsigned lDataSize = 0;

	ElementArray::iterator x;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		lDataSize += lElement->GetCalibrationDataSize();
	}

	return lDataSize;
}

void InputDevice::GetCalibrationData(Lepra::uint8* pData)
{
	unsigned lOffset = 0;
	ElementArray::iterator x;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		lElement->GetCalibrationData(&pData[lOffset]);
		lOffset += lElement->GetCalibrationDataSize();
	}
}

void InputDevice::SetCalibrationData(Lepra::uint8* pData)
{
	unsigned lOffset = 0;
	ElementArray::iterator x;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		lElement->SetCalibrationData(&pData[lOffset]);
		lOffset += lElement->GetCalibrationDataSize();
	}
}







InputManager::InputManager()
{
	for (int i = 0; i < 256; i++)
	{
		mKeyDown[i] = false;
	}
}

InputManager::~InputManager()
{
}

const InputManager::DeviceList& InputManager::GetDeviceList() const
{
	return (mDeviceList);
}

void InputManager::AddTextInputObserver(TextInputObserver* pListener)
{
	mTextObserverList.insert(pListener);
}

void InputManager::RemoveTextInputObserver(TextInputObserver* pListener)
{
	mTextObserverList.erase(pListener);
}

void InputManager::AddKeyCodeInputObserver(KeyCodeInputObserver* pListener)
{
	mKeyCodeObserverList.insert(pListener);
}

void InputManager::RemoveKeyCodeInputObserver(KeyCodeInputObserver* pListener)
{
	mKeyCodeObserverList.erase(pListener);
}

void InputManager::AddMouseInputObserver(MouseInputObserver* pListener)
{
	mMouseObserverList.insert(pListener);
}

void InputManager::RemoveMouseInputObserver(MouseInputObserver* pListener)
{
	mMouseObserverList.erase(pListener);
}

bool InputManager::ReadKey(KeyCode pKeyCode)
{
	return (mKeyDown[(int)pKeyCode]);
}

void InputManager::SetKey(KeyCode pKeyCode, bool pValue)
{
	mKeyDown[(int)pKeyCode] = pValue;
}

bool InputManager::NotifyOnChar(Lepra::tchar pChar)
{
	bool lConsumed = false;
	TextObserverList::iterator x = mTextObserverList.begin();
	for (; !lConsumed && x != mTextObserverList.end(); ++x)
	{
		lConsumed = (*x)->OnChar(pChar);
	}
	return (lConsumed);
}

bool InputManager::NotifyOnKeyDown(KeyCode pKeyCode)
{
	bool lConsumed = false;
	KeyCodeObserverList::iterator x = mKeyCodeObserverList.begin();
	for (; !lConsumed && x != mKeyCodeObserverList.end(); ++x)
	{
		lConsumed = (*x)->OnKeyDown(pKeyCode);
	}
	return (lConsumed);
}

bool InputManager::NotifyOnKeyUp(KeyCode pKeyCode)
{
	bool lConsumed = false;
	KeyCodeObserverList::iterator x = mKeyCodeObserverList.begin();
	for (; !lConsumed && x != mKeyCodeObserverList.end(); ++x)
	{
		lConsumed = (*x)->OnKeyUp(pKeyCode);
	}
	return (lConsumed);
}

bool InputManager::NotifyMouseDoubleClick()
{
	bool lConsumed = false;
	MouseObserverList::iterator x = mMouseObserverList.begin();
	for (; !lConsumed && x != mMouseObserverList.end(); ++x)
	{
		lConsumed = (*x)->OnDoubleClick();
	}
	return (lConsumed);
}

void InputManager::PollEvents()
{
	DeviceList::iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		lDevice->PollEvents();
	}
}

void InputManager::SetFunctor(InputFunctor* pFunctor)
{
	DeviceList::iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		lDevice->SetFunctor(pFunctor->CreateCopy());
	}

	delete pFunctor;
}

void InputManager::ActivateAll()
{
	DeviceList::iterator x;
	for (x = mDeviceList.begin(); x != mDeviceList.end(); ++x)
	{
		InputDevice* lDevice = *x;
		lDevice->Activate();
	}
}

void InputManager::ReleaseAll()
{
	DeviceList::iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		lDevice->Release();
	}
}

InputDevice* InputManager::FindDevice(const Lepra::String& pDeviceIdentifier, int pN)
{
	int lN = 0;

	DeviceList::iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		if (pDeviceIdentifier == lDevice->GetIdentifier())
		{
			if (lN == pN)
			{
				return lDevice;
			}
			else
			{
				lN++;
			}
		}
	}

	return 0;
}

int InputManager::GetDeviceCount(const Lepra::String& pDeviceIdentifier) const
{
	int lCount = 0;

	DeviceList::const_iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		if (pDeviceIdentifier == lDevice->GetIdentifier())
		{
			lCount++;
		}
	}

	return lCount;
}

unsigned InputManager::GetDeviceIndex(InputDevice* pDevice) const
{
	int lCount = 0;

	DeviceList::const_iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		if (pDevice->GetIdentifier() == lDevice->GetIdentifier())
		{
			if (pDevice == lDevice)
			{
				return lCount;
			}

			lCount++;
		}
	}

	return 0;
}



}
