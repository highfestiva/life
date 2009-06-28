
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "ClientOptionsManager.h"
#include <assert.h>
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Math.h"
#include "../RtVar.h"
#include "ClientOptions.h"



namespace Life
{
namespace Options
{



ClientOptionsManager::ClientOptionsManager(Cure::RuntimeVariableScope* pVariableScope, int pPriority):
	mVariableScope(pVariableScope)
{
	::memset(&mOptions, 0, sizeof(mOptions));
	SetDefault(pPriority);
}


	
bool ClientOptionsManager::UpdateInput(UiLepra::InputManager::KeyCode pKeyCode, bool pActive)
{
	const Lepra::String lInputElementName = ConvertToString(pKeyCode);
	return (SetValue(lInputElementName, pActive? 1.0f : 0.0f));
}

bool ClientOptionsManager::UpdateInput(UiLepra::InputElement* pElement)
{
	bool lValueSet;
	float lValue = (float)pElement->GetValue();
	const Lepra::String lInputElementName = ConvertToString(pElement);
	if (pElement->GetType() == UiLepra::InputElement::ANALOGUE)
	{
		// Clamp analogue to neutral when close enough.
		if (Lepra::Math::IsEpsEqual(lValue, 0.0f, 0.1f))
		{
			lValue = 0;
		}
		// Update both sides for analogue input.
		if (lValue >= 0)
		{
			lValueSet = SetValue(lInputElementName+_T("+"), lValue);
			SetValue(lInputElementName+_T("-"), 0);
		}
		else
		{
			lValueSet = SetValue(lInputElementName+_T("-"), -lValue);
			SetValue(lInputElementName+_T("+"), 0);
		}
	}
	else
	{
		lValueSet = SetValue(lInputElementName, lValue);
	}
	return (lValueSet);
}



const ClientOptions& ClientOptionsManager::GetOptions() const
{
	return (mOptions);
}



bool ClientOptionsManager::SetDefault(int pPriority)
{
	bool lOk = false;
	// TODO: check for installed devices, prioritize, etc.
	switch (pPriority)
	{
		case 0:
		{
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.PARAGRAPH, Key.ACUTE, Key.APOSTROPHE"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD, _T("Device0.Button1"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD3D, _T("Device0.AbsoluteAxis2-"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK3D, _T("Device0.AbsoluteAxis2+"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, _T("Device0.Button0"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_LEFT, _T("Device0.AbsoluteAxis3-"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_RIGHT, _T("Device0.AbsoluteAxis3+"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Device0.AbsoluteAxis0-"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Device0.AbsoluteAxis0+"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Device0.Button5"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRK, _T(""));
			lOk = true;
		}
		break;
		case 1:
		{
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.SCROLL_LOCK"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD, _T("Key.UP"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, _T("Key.DOWN"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_LEFT, _T("Key.LEFT"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_RIGHT, _T("Key.RIGHT"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Key.NUMPAD_4"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Key.NUMPAD_1"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Key.NUMPAD_0"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRK, _T(""));
			lOk = true;
		}
		break;
		case 2:
		{
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.F12"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD, _T("Key.W"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, _T("Key.S"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_LEFT, _T("Key.A"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_RIGHT, _T("Key.D"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Key.E"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Key.Q"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Key.E"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRK, _T(""));
			lOk = true;
		}
		break;
		case 3:
		{
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.PAUSE"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD, _T("Key.NUMPAD_8"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, _T("Key.NUMPAD_5"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_LEFT, _T("Key.NUMPAD_4"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_RIGHT, _T("Key.NUMPAD_6"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Key.NUMPAD_9"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Key.NUMPAD_7"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Key.NUMPAD_9"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRK, _T("Key.NUMPAD_7"));
			lOk = true;
		}
		break;
	}
	return (lOk);
}

const Lepra::String ClientOptionsManager::ConvertToString(UiLepra::InputManager::KeyCode pKeyCode) const
{
	return (_T("Key.")+UiLepra::InputManager::GetKeyName(pKeyCode));
}

const Lepra::String ClientOptionsManager::ConvertToString(UiLepra::InputElement* pElement) const
{
	Lepra::String lName;
	switch (pElement->GetParentDevice()->GetInterpretation())
	{
		case UiLepra::InputDevice::TYPE_MOUSE:		lName = _T("Mouse");	break;
		case UiLepra::InputDevice::TYPE_KEYBOARD:	lName = _T("Keyboard");	break;
		default:					lName = _T("Device");	break;
	}
	lName += Lepra::StringUtility::IntToString(pElement->GetParentDevice()->GetTypeIndex(), 10)+_T(".");

	switch (pElement->GetInterpretation())
	{
		case UiLepra::InputElement::ABSOLUTE_AXIS:	lName += _T("AbsoluteAxis");	break;
		case UiLepra::InputElement::RELATIVE_AXIS:	lName += _T("RelativeAxis");	break;
		default:					lName += _T("Button");		break;
	}
	lName += Lepra::StringUtility::IntToString(pElement->GetTypeIndex(), 10);
	return (lName);
}

bool ClientOptionsManager::SetValue(const Lepra::String& pKey, float pValue)
{
	bool lIsSteeringValue;
	float* lValuePointer = GetValuePointer(pKey, lIsSteeringValue);
	if (lValuePointer)
	{
		if (!Lepra::Math::IsEpsEqual(*lValuePointer, pValue))
		{
			*lValuePointer = pValue;
			return (lIsSteeringValue);	// RAII.
		}
	}
	return (false);
}

float* ClientOptionsManager::GetValuePointer(const Lepra::String& pKey, bool& pIsSteeringValue)
{
	pIsSteeringValue = false;

	typedef std::pair<const Lepra::String, float*> KeyValue;
	const KeyValue lEntries[] =
	{
		KeyValue(_T(RTVAR_CTRL_UI_CONTOGGLE), &mOptions.mControl.mUi.mConsoleToggle),
		KeyValue(_T(RTVAR_CTRL_STEER_FWD), &mOptions.mControl.mVehicle.mForward),
		KeyValue(_T(RTVAR_CTRL_STEER_BACK), &mOptions.mControl.mVehicle.mBackward),
		KeyValue(_T(RTVAR_CTRL_STEER_FWD3D), &mOptions.mControl.mVehicle.mForward3d),
		KeyValue(_T(RTVAR_CTRL_STEER_BACK3D), &mOptions.mControl.mVehicle.mBackward3d),
		KeyValue(_T(RTVAR_CTRL_STEER_BRKBACK), &mOptions.mControl.mVehicle.mBreakAndBack),
		KeyValue(_T(RTVAR_CTRL_STEER_LEFT), &mOptions.mControl.mVehicle.mLeft),
		KeyValue(_T(RTVAR_CTRL_STEER_RIGHT), &mOptions.mControl.mVehicle.mRight),
		KeyValue(_T(RTVAR_CTRL_STEER_UP), &mOptions.mControl.mVehicle.mUp),
		KeyValue(_T(RTVAR_CTRL_STEER_DOWN), &mOptions.mControl.mVehicle.mDown),
		KeyValue(_T(RTVAR_CTRL_STEER_HANDBRK), &mOptions.mControl.mVehicle.mHandBreak),
		KeyValue(_T(RTVAR_CTRL_STEER_BRK), &mOptions.mControl.mVehicle.mBreak),
	};
	for (int x = 0; x < sizeof(lEntries)/sizeof(lEntries[0]); ++x)
	{
		const Lepra::String lKeys = mVariableScope->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY,
			lEntries[x].first);
		typedef Lepra::StringUtility::StringVector SV;
		SV lKeyArray = Lepra::StringUtility::Split(lKeys, _T(", \t"));
		for (SV::iterator y = lKeyArray.begin(); y != lKeyArray.end(); ++y)
		{
			if ((*y) == pKey)
			{
				pIsSteeringValue = (lEntries[x].first.find(_T("Steer")) != Lepra::String::npos);
				return (lEntries[x].second);
			}
		}
	}
	return (0);
}



void ClientOptionsManager::operator=(const ClientOptionsManager&)
{
	assert(false);
}



LOG_CLASS_DEFINE(UI_INPUT, ClientOptionsManager);



}
}
