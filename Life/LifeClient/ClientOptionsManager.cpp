
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
	mEscape = 0;
	mShowScore = 0;
	::memset(&mSteeringControl, 0, sizeof(mSteeringControl));
	::memset(&mCamControl, 0, sizeof(mCamControl));
	::memset(&mFireControl, 0, sizeof(mFireControl));
	SetDefault(pPriority);
	DoRefreshConfiguration();
}



const bool ClientOptionsManager::IsEscape() const
{
	return mEscape > 0.5f;
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
		KeyValue(RTVAR_CTRL_UI_CONTOGGLE, &mConsoleToggle),
		KeyValue(RTVAR_CTRL_UI_ESCAPE, &mEscape),
		KeyValue(RTVAR_CTRL_UI_SHOWSCORE, &mShowScore),
		KeyValue(RTVAR_CTRL_STEER_FWD, &mSteeringControl.mControl[Steering::CONTROL_FORWARD]),
		KeyValue(RTVAR_CTRL_STEER_BACK, &mSteeringControl.mControl[Steering::CONTROL_BACKWARD]),
		KeyValue(RTVAR_CTRL_STEER_FWD3D, &mSteeringControl.mControl[Steering::CONTROL_FORWARD3D]),
		KeyValue(RTVAR_CTRL_STEER_BACK3D, &mSteeringControl.mControl[Steering::CONTROL_BACKWARD3D]),
		KeyValue(RTVAR_CTRL_STEER_BRKBACK, &mSteeringControl.mControl[Steering::CONTROL_BRAKEANDBACK]),
		KeyValue(RTVAR_CTRL_STEER_LEFT, &mSteeringControl.mControl[Steering::CONTROL_LEFT]),
		KeyValue(RTVAR_CTRL_STEER_RIGHT, &mSteeringControl.mControl[Steering::CONTROL_RIGHT]),
		KeyValue(RTVAR_CTRL_STEER_LEFT3D, &mSteeringControl.mControl[Steering::CONTROL_LEFT3D]),
		KeyValue(RTVAR_CTRL_STEER_RIGHT3D, &mSteeringControl.mControl[Steering::CONTROL_RIGHT3D]),
		KeyValue(RTVAR_CTRL_STEER_UP, &mSteeringControl.mControl[Steering::CONTROL_UP]),
		KeyValue(RTVAR_CTRL_STEER_DOWN, &mSteeringControl.mControl[Steering::CONTROL_DOWN]),
		KeyValue(RTVAR_CTRL_STEER_UP3D, &mSteeringControl.mControl[Steering::CONTROL_UP3D]),
		KeyValue(RTVAR_CTRL_STEER_DOWN3D, &mSteeringControl.mControl[Steering::CONTROL_DOWN3D]),
		KeyValue(RTVAR_CTRL_STEER_HANDBRK, &mSteeringControl.mControl[Steering::CONTROL_HANDBRAKE]),
		KeyValue(RTVAR_CTRL_STEER_BRK, &mSteeringControl.mControl[Steering::CONTROL_BREAK]),
		KeyValue(RTVAR_CTRL_STEER_YAW, &mSteeringControl.mControl[Steering::CONTROL_YAW_ANGLE]),
		KeyValue(RTVAR_CTRL_STEER_PITCH, &mSteeringControl.mControl[Steering::CONTROL_PITCH_ANGLE]),
		KeyValue(RTVAR_CTRL_UI_CAMLEFT, &mCamControl.mControl[CamControl::CAMDIR_LEFT]),
		KeyValue(RTVAR_CTRL_UI_CAMRIGHT, &mCamControl.mControl[CamControl::CAMDIR_RIGHT]),
		KeyValue(RTVAR_CTRL_UI_CAMFORWARD, &mCamControl.mControl[CamControl::CAMDIR_FORWARD]),
		KeyValue(RTVAR_CTRL_UI_CAMBACKWARD, &mCamControl.mControl[CamControl::CAMDIR_BACKWARD]),
		KeyValue(RTVAR_CTRL_UI_CAMUP, &mCamControl.mControl[CamControl::CAMDIR_UP]),
		KeyValue(RTVAR_CTRL_UI_CAMDOWN, &mCamControl.mControl[CamControl::CAMDIR_DOWN]),
		KeyValue(RTVAR_CTRL_FIRE0, &mFireControl.mControl[FireControl::FIRE0]),
		KeyValue(RTVAR_CTRL_FIRE1, &mFireControl.mControl[FireControl::FIRE1]),
		KeyValue(RTVAR_CTRL_FIRE2, &mFireControl.mControl[FireControl::FIRE2]),
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
			lValueSet |= SetValue(lInputElementName+"+", 0, false);
			SetValue(lInputElementName+"-", 0, false);
		}
		if (lValue > 0)
		{
			lValueSet |= SetValue(lInputElementName+"+", lValue, lIsRelative);
			SetValue(lInputElementName+"-", 0, false);
		}
		else if (lValue < 0)
		{
			lValueSet |= SetValue(lInputElementName+"-", -lValue, lIsRelative);
			SetValue(lInputElementName+"+", 0, false);
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
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMUP, "Key.PGUP");
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMDOWN, "Key.INSERT");
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMLEFT, "Key.DEL");
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMRIGHT, "Key.PGDOWN");
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMFORWARD, "Key.HOME");
	v_override(mVariableScope, RTVAR_CTRL_UI_CAMBACKWARD, "Key.END");

	switch (pPriority)
	{
		case 0:
		{
			v_override(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, "Key.PARAGRAPH, Key.ACUTE, Key.APOSTROPHE");
			v_override(mVariableScope, RTVAR_CTRL_UI_ESCAPE, "Key.ESC");
			v_override(mVariableScope, RTVAR_CTRL_UI_SHOWSCORE, "Key.TAB");
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD, "Key.UP");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, "Key.DOWN");
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT, "Key.LEFT");
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT, "Key.RIGHT");
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP, "Key.NUMPAD_4");
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN, "Key.NUMPAD_1");
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP3D, "Key.NUMPAD_4");
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, "Key.NUMPAD_1");
			v_override(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, "Key.NUMPAD_0");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRK, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_YAW, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_PITCH, "");
			v_override(mVariableScope, RTVAR_CTRL_FIRE0, "Key.END");
			v_override(mVariableScope, RTVAR_CTRL_FIRE1, "");
			v_override(mVariableScope, RTVAR_CTRL_FIRE2, "");
			lOk = true;
		}
		break;
		case 1:
		{
			v_override(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, "Key.SCROLL_LOCK");
			v_override(mVariableScope, RTVAR_CTRL_UI_ESCAPE, "Key.ESC");
			v_override(mVariableScope, RTVAR_CTRL_UI_SHOWSCORE, "Key.TAB");
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD, "Device0.Button1");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD3D, "Device0.AbsoluteAxis2-");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK3D, "Device0.AbsoluteAxis2+");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, "Device0.Button0");
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT, "Device0.AbsoluteAxis3-");
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT, "Device0.AbsoluteAxis3+");
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, "Device0.AbsoluteAxis1-");
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, "Device0.AbsoluteAxis1+");
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP, "Device0.AbsoluteAxis0-");
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN, "Device0.AbsoluteAxis0+");
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP3D, "Device0.AbsoluteAxis0-");
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, "Device0.AbsoluteAxis0+");
			v_override(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, "Device0.Button5");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRK, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_YAW, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_PITCH, "");
			v_override(mVariableScope, RTVAR_CTRL_FIRE0, "Device0.Button2");
			v_override(mVariableScope, RTVAR_CTRL_FIRE1, "");
			v_override(mVariableScope, RTVAR_CTRL_FIRE2, "");
			lOk = true;
		}
		break;
		case 2:
		{
			v_override(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, "Key.F12");
			v_override(mVariableScope, RTVAR_CTRL_UI_ESCAPE, "Key.ESC");
			v_override(mVariableScope, RTVAR_CTRL_UI_SHOWSCORE, "Key.TAB");
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD, "Key.W");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, "Key.S");
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT, "Key.A");
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT, "Key.D");
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP, "Key.E");
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN, "Key.Q");
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP3D, "Key.E");
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, "Key.Q");
			v_override(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, "Key.E");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRK, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_YAW, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_PITCH, "");
			v_override(mVariableScope, RTVAR_CTRL_FIRE0, "Key.LCTRL");
			v_override(mVariableScope, RTVAR_CTRL_FIRE1, "");
			v_override(mVariableScope, RTVAR_CTRL_FIRE2, "");
			lOk = true;
		}
		break;
		case 3:
		{
			v_override(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, "Key.PAUSE");
			v_override(mVariableScope, RTVAR_CTRL_UI_ESCAPE, "Key.ESC");
			v_override(mVariableScope, RTVAR_CTRL_UI_SHOWSCORE, "Key.TAB");
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD, "Key.NUMPAD_8");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_FWD3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BACK3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, "Key.NUMPAD_5");
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT, "Key.NUMPAD_4");
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT, "Key.NUMPAD_6");
			v_override(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP, "Key.NUMPAD_9");
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN, "Key.NUMPAD_7");
			v_override(mVariableScope, RTVAR_CTRL_STEER_UP3D, "Key.NUMPAD_9");
			v_override(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, "Key.NUMPAD_7");
			v_override(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, "Key.NUMPAD_9");
			v_override(mVariableScope, RTVAR_CTRL_STEER_BRK, "Key.NUMPAD_7");
			v_override(mVariableScope, RTVAR_CTRL_STEER_YAW, "");
			v_override(mVariableScope, RTVAR_CTRL_STEER_PITCH, "");
			v_override(mVariableScope, RTVAR_CTRL_FIRE0, "Key.NUMPAD_PLUS");
			v_override(mVariableScope, RTVAR_CTRL_FIRE1, "");
			v_override(mVariableScope, RTVAR_CTRL_FIRE2, "");
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
