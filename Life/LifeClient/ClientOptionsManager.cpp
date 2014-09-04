
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "ClientOptionsManager.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/Math.h"
#include "ClientOptions.h"
#include "RtVar.h"



namespace Life
{
namespace Options
{



ClientOptionsManager::ClientOptionsManager(Cure::RuntimeVariableScope* pVariableScope, int pPriority):
	OptionsManager(pVariableScope, pPriority)
{
	mMouseSensitivity = 4.0f;
	mShowScore = 0;
	::memset(&mSteeringControl, 0, sizeof(mSteeringControl));
	::memset(&mCamControl, 0, sizeof(mCamControl));
	::memset(&mFireControl, 0, sizeof(mFireControl));
	SetDefault(pPriority);
	DoRefreshConfiguration();
}



const bool ClientOptionsManager::GetShowScore() const
{
	return mShowScore > 0.5f;
}

const Steering& ClientOptionsManager::GetSteeringControl() const
{
	return mSteeringControl;
}

const CamControl& ClientOptionsManager::GetCamControl() const
{
	return mCamControl;
}

const FireControl& ClientOptionsManager::GetFireControl() const
{
	return mFireControl;
}



void ClientOptionsManager::DoRefreshConfiguration()
{
	const KeyValue lEntries[] =
	{
		KeyValue(_T(RTVAR_CTRL_UI_CONTOGGLE), &mConsoleToggle),
		KeyValue(_T(RTVAR_CTRL_UI_SHOWSCORE), &mShowScore),
		KeyValue(_T(RTVAR_CTRL_STEER_FWD), &mSteeringControl.mControl[Steering::CONTROL_FORWARD]),
		KeyValue(_T(RTVAR_CTRL_STEER_BACK), &mSteeringControl.mControl[Steering::CONTROL_BACKWARD]),
		KeyValue(_T(RTVAR_CTRL_STEER_FWD3D), &mSteeringControl.mControl[Steering::CONTROL_FORWARD3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_BACK3D), &mSteeringControl.mControl[Steering::CONTROL_BACKWARD3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_BRKBACK), &mSteeringControl.mControl[Steering::CONTROL_BRAKEANDBACK]),
		KeyValue(_T(RTVAR_CTRL_STEER_LEFT), &mSteeringControl.mControl[Steering::CONTROL_LEFT]),
		KeyValue(_T(RTVAR_CTRL_STEER_RIGHT), &mSteeringControl.mControl[Steering::CONTROL_RIGHT]),
		KeyValue(_T(RTVAR_CTRL_STEER_LEFT3D), &mSteeringControl.mControl[Steering::CONTROL_LEFT3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_RIGHT3D), &mSteeringControl.mControl[Steering::CONTROL_RIGHT3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_UP), &mSteeringControl.mControl[Steering::CONTROL_UP]),
		KeyValue(_T(RTVAR_CTRL_STEER_DOWN), &mSteeringControl.mControl[Steering::CONTROL_DOWN]),
		KeyValue(_T(RTVAR_CTRL_STEER_UP3D), &mSteeringControl.mControl[Steering::CONTROL_UP3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_DOWN3D), &mSteeringControl.mControl[Steering::CONTROL_DOWN3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_HANDBRK), &mSteeringControl.mControl[Steering::CONTROL_HANDBRAKE]),
		KeyValue(_T(RTVAR_CTRL_STEER_BRK), &mSteeringControl.mControl[Steering::CONTROL_BREAK]),
		KeyValue(_T(RTVAR_CTRL_STEER_YAW), &mSteeringControl.mControl[Steering::CONTROL_YAW_ANGLE]),
		KeyValue(_T(RTVAR_CTRL_STEER_PITCH), &mSteeringControl.mControl[Steering::CONTROL_PITCH_ANGLE]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMLEFT), &mCamControl.mControl[CamControl::CAMDIR_LEFT]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMRIGHT), &mCamControl.mControl[CamControl::CAMDIR_RIGHT]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMFORWARD), &mCamControl.mControl[CamControl::CAMDIR_FORWARD]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMBACKWARD), &mCamControl.mControl[CamControl::CAMDIR_BACKWARD]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMUP), &mCamControl.mControl[CamControl::CAMDIR_UP]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMDOWN), &mCamControl.mControl[CamControl::CAMDIR_DOWN]),
		KeyValue(_T(RTVAR_CTRL_FIRE0), &mFireControl.mControl[FireControl::FIRE0]),
		KeyValue(_T(RTVAR_CTRL_FIRE1), &mFireControl.mControl[FireControl::FIRE1]),
		KeyValue(_T(RTVAR_CTRL_FIRE2), &mFireControl.mControl[FireControl::FIRE2]),
	};
	SetValuePointers(lEntries, LEPRA_ARRAY_COUNT(lEntries));

	v_tryget(mMouseSensitivity, =(float), mVariableScope, RTVAR_CTRL_MOUSESENSITIVITY, 4.0f);
}

bool ClientOptionsManager::UpdateInput(UiLepra::InputElement* pElement)
{
	bool lValueSet = false;
	float lValue = pElement->GetValue();
	const str lInputElementName = pElement->GetFullName();
	if (pElement->GetType() == UiLepra::InputElement::ANALOGUE)
	{
		const bool lIsRelative = (pElement->GetInterpretation() == UiLepra::InputElement::RELATIVE_AXIS);
		// Update both sides for analogue input.
		if (lIsRelative)
		{
			lValue *= mMouseSensitivity * 0.03f;	// Relative values are more sensitive.
			lValueSet |= SetValue(lInputElementName, lValue, true);
		}
		else if (Math::IsEpsEqual(lValue, 0.0f, 0.1f))
		{
			// Clamp absolute+analogue to neutral when close enough.
			lValueSet |= SetValue(lInputElementName+_T("+"), 0, false);
			SetValue(lInputElementName+_T("-"), 0, false);
		}
		if (lValue > 0)
		{
			lValueSet |= SetValue(lInputElementName+_T("+"), lValue, lIsRelative);
			SetValue(lInputElementName+_T("-"), 0, false);
		}
		else if (lValue < 0)
		{
			lValueSet |= SetValue(lInputElementName+_T("-"), -lValue, lIsRelative);
			SetValue(lInputElementName+_T("+"), 0, false);
		}
	}
	else
	{
		lValueSet |= SetValue(lInputElementName, lValue, false);
	}
	return lValueSet;
}



bool ClientOptionsManager::SetDefault(int pPriority)
{
	bool lOk = false;

	// TODO: check for installed devices, prioritize, etc.

	// Hmmm... Same cam controls for all split-screen clients. Swell.
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMUP, _T("Key.PGUP"));
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMDOWN, _T("Key.INSERT"));
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMLEFT, _T("Key.DEL"));
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMRIGHT, _T("Key.PGDOWN"));
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMFORWARD, _T("Key.HOME"));
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMBACKWARD, _T("Key.END"));

	switch (pPriority)
	{
		case 0:
		{
			v_override(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.PARAGRAPH, Key.ACUTE, Key.APOSTROPHE"));
			v_override(mVariableScope, RTVAR_CTRL_UI_SHOWSCORE, _T("Key.TAB"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD, _T("Key.UP"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, _T("Key.DOWN"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT, _T("Key.LEFT"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT, _T("Key.RIGHT"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Key.NUMPAD_4"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Key.NUMPAD_1"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP3D, _T("Key.NUMPAD_4"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, _T("Key.NUMPAD_1"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Key.NUMPAD_0"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRK, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_YAW, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_PITCH, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_FIRE0, _T("Key.END"));
			v_override(mVariableScope, RTVAR_CTRL_FIRE1, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_FIRE2, _T(""));
			lOk = true;
		}
		break;
		case 1:
		{
			v_override(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.SCROLL_LOCK"));
			v_override(mVariableScope, RTVAR_CTRL_UI_SHOWSCORE, _T("Key.TAB"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD, _T("Device0.Button1"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD3D, _T("Device0.AbsoluteAxis2-"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK3D, _T("Device0.AbsoluteAxis2+"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, _T("Device0.Button0"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT, _T("Device0.AbsoluteAxis3-"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT, _T("Device0.AbsoluteAxis3+"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, _T("Device0.AbsoluteAxis1-"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, _T("Device0.AbsoluteAxis1+"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Device0.AbsoluteAxis0-"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Device0.AbsoluteAxis0+"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP3D, _T("Device0.AbsoluteAxis0-"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, _T("Device0.AbsoluteAxis0+"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Device0.Button5"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRK, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_YAW, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_PITCH, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_FIRE0, _T("Device0.Button2"));
			v_override(mVariableScope, RTVAR_CTRL_FIRE1, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_FIRE2, _T(""));
			lOk = true;
		}
		break;
		case 2:
		{
			v_override(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.F12"));
			v_override(mVariableScope, RTVAR_CTRL_UI_SHOWSCORE, _T("Key.TAB"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD, _T("Key.W"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, _T("Key.S"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT, _T("Key.A"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT, _T("Key.D"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Key.E"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Key.Q"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP3D, _T("Key.E"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, _T("Key.Q"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Key.E"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRK, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_YAW, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_PITCH, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_FIRE0, _T("Key.LCTRL"));
			v_override(mVariableScope, RTVAR_CTRL_FIRE1, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_FIRE2, _T(""));
			lOk = true;
		}
		break;
		case 3:
		{
			v_override(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.PAUSE"));
			v_override(mVariableScope, RTVAR_CTRL_UI_SHOWSCORE, _T("Key.TAB"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD, _T("Key.NUMPAD_8"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, _T("Key.NUMPAD_5"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT, _T("Key.NUMPAD_4"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT, _T("Key.NUMPAD_6"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Key.NUMPAD_9"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Key.NUMPAD_7"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP3D, _T("Key.NUMPAD_9"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, _T("Key.NUMPAD_7"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Key.NUMPAD_9"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRK, _T("Key.NUMPAD_7"));
			v_override(mVariableScope, RTVAR_CTRL_STEER_YAW, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_STEER_PITCH, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_FIRE0, _T("Key.NUMPAD_PLUS"));
			v_override(mVariableScope, RTVAR_CTRL_FIRE1, _T(""));
			v_override(mVariableScope, RTVAR_CTRL_FIRE2, _T(""));
			lOk = true;
		}
		break;
	}
	return (lOk);
}



void ClientOptionsManager::operator=(const ClientOptionsManager&)
{
	deb_assert(false);
}



loginstance(UI_INPUT, ClientOptionsManager);



}
}
