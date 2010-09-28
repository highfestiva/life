
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "ClientOptionsManager.h"
#include <assert.h>
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
	::memset(&mSteeringControl, 0, sizeof(mSteeringControl));
	::memset(&mCamControl, 0, sizeof(mCamControl));
	SetDefault(pPriority);
	DoRefreshConfiguration();
}



const Steering& ClientOptionsManager::GetSteeringControl() const
{
	return (mSteeringControl);
}

const CamControl& ClientOptionsManager::GetCamControl() const
{
	return (mCamControl);
}



bool ClientOptionsManager::SetDefault(int pPriority)
{
	bool lOk = false;

	// TODO: check for installed devices, prioritize, etc.

	// Hmmm... Same cam controls for all split-screen clients. Swell.
	CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CAMUP, _T("Key.PGUP"));
	CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CAMDOWN, _T("Key.INSERT"));
	CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CAMLEFT, _T("Key.DEL"));
	CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CAMRIGHT, _T("Key.PGDOWN"));
	CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CAMFORWARD, _T("Key.HOME"));
	CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CAMBACKWARD, _T("Key.END"));

	switch (pPriority)
	{
		case 0:
		{
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.PARAGRAPH, Key.ACUTE, Key.APOSTROPHE"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD, _T("Key.UP"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, _T("Key.DOWN"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_LEFT, _T("Key.LEFT"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_RIGHT, _T("Key.RIGHT"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Key.NUMPAD_4"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Key.NUMPAD_1"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP3D, _T("Key.NUMPAD_4"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, _T("Key.NUMPAD_1"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Key.NUMPAD_0"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRK, _T(""));
			lOk = true;
		}
		break;
		case 1:
		{
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.SCROLL_LOCK"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD, _T("Device0.Button1"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_FWD3D, _T("Device0.AbsoluteAxis2-"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BACK3D, _T("Device0.AbsoluteAxis2+"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRKBACK, _T("Device0.Button0"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_LEFT, _T("Device0.AbsoluteAxis3-"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_RIGHT, _T("Device0.AbsoluteAxis3+"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, _T("Device0.AbsoluteAxis1-"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, _T("Device0.AbsoluteAxis1+"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Device0.AbsoluteAxis0-"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Device0.AbsoluteAxis0+"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP3D, _T("Device0.AbsoluteAxis0-"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, _T("Device0.AbsoluteAxis0+"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Device0.Button5"));
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
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Key.E"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Key.Q"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP3D, _T("Key.E"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, _T("Key.Q"));
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
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_LEFT3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_RIGHT3D, _T(""));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP, _T("Key.NUMPAD_9"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN, _T("Key.NUMPAD_7"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_UP3D, _T("Key.NUMPAD_9"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_DOWN3D, _T("Key.NUMPAD_7"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_HANDBRK, _T("Key.NUMPAD_9"));
			CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_STEER_BRK, _T("Key.NUMPAD_7"));
			lOk = true;
		}
		break;
	}
	return (lOk);
}

void ClientOptionsManager::DoRefreshConfiguration()
{
	typedef std::pair<const str, float*> KeyValue;
	const KeyValue lEntries[] =
	{
		KeyValue(_T(RTVAR_CTRL_UI_CONTOGGLE), &mConsoleToggle),
		KeyValue(_T(RTVAR_CTRL_STEER_FWD), &mSteeringControl.mControl[Steering::CONTROL_FORWARD]),
		KeyValue(_T(RTVAR_CTRL_STEER_BACK), &mSteeringControl.mControl[Steering::CONTROL_BACKWARD]),
		KeyValue(_T(RTVAR_CTRL_STEER_FWD3D), &mSteeringControl.mControl[Steering::CONTROL_FORWARD3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_BACK3D), &mSteeringControl.mControl[Steering::CONTROL_BACKWARD3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_BRKBACK), &mSteeringControl.mControl[Steering::CONTROL_BREAKANDBACK]),
		KeyValue(_T(RTVAR_CTRL_STEER_LEFT), &mSteeringControl.mControl[Steering::CONTROL_LEFT]),
		KeyValue(_T(RTVAR_CTRL_STEER_RIGHT), &mSteeringControl.mControl[Steering::CONTROL_RIGHT]),
		KeyValue(_T(RTVAR_CTRL_STEER_LEFT3D), &mSteeringControl.mControl[Steering::CONTROL_LEFT3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_RIGHT3D), &mSteeringControl.mControl[Steering::CONTROL_RIGHT3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_UP), &mSteeringControl.mControl[Steering::CONTROL_UP]),
		KeyValue(_T(RTVAR_CTRL_STEER_DOWN), &mSteeringControl.mControl[Steering::CONTROL_DOWN]),
		KeyValue(_T(RTVAR_CTRL_STEER_UP3D), &mSteeringControl.mControl[Steering::CONTROL_UP3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_DOWN3D), &mSteeringControl.mControl[Steering::CONTROL_DOWN3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_HANDBRK), &mSteeringControl.mControl[Steering::CONTROL_HANDBREAK]),
		KeyValue(_T(RTVAR_CTRL_STEER_BRK), &mSteeringControl.mControl[Steering::CONTROL_BREAK]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMLEFT), &mCamControl.mControl[CamControl::CAMDIR_LEFT]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMRIGHT), &mCamControl.mControl[CamControl::CAMDIR_RIGHT]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMFORWARD), &mCamControl.mControl[CamControl::CAMDIR_FORWARD]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMBACKWARD), &mCamControl.mControl[CamControl::CAMDIR_BACKWARD]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMUP), &mCamControl.mControl[CamControl::CAMDIR_UP]),
		KeyValue(_T(RTVAR_CTRL_UI_CAMDOWN), &mCamControl.mControl[CamControl::CAMDIR_DOWN]),
	};
	SetValuePointers(lEntries, LEPRA_ARRAY_COUNT(lEntries));
}



void ClientOptionsManager::operator=(const ClientOptionsManager&)
{
	assert(false);
}



LOG_CLASS_DEFINE(UI_INPUT, ClientOptionsManager);



}
}
