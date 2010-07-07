
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
	::memset(&mControl, 0, sizeof(mControl));
	SetDefault(pPriority);
	DoRefreshConfiguration();
}



const Vehicle& ClientOptionsManager::GetControl() const
{
	return (mControl);
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
		KeyValue(_T(RTVAR_CTRL_STEER_FWD), &mControl.mControl[Vehicle::CONTROL_FORWARD]),
		KeyValue(_T(RTVAR_CTRL_STEER_BACK), &mControl.mControl[Vehicle::CONTROL_BACKWARD]),
		KeyValue(_T(RTVAR_CTRL_STEER_FWD3D), &mControl.mControl[Vehicle::CONTROL_FORWARD3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_BACK3D), &mControl.mControl[Vehicle::CONTROL_BACKWARD3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_BRKBACK), &mControl.mControl[Vehicle::CONTROL_BREAKANDBACK]),
		KeyValue(_T(RTVAR_CTRL_STEER_LEFT), &mControl.mControl[Vehicle::CONTROL_LEFT]),
		KeyValue(_T(RTVAR_CTRL_STEER_RIGHT), &mControl.mControl[Vehicle::CONTROL_RIGHT]),
		KeyValue(_T(RTVAR_CTRL_STEER_LEFT3D), &mControl.mControl[Vehicle::CONTROL_LEFT3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_RIGHT3D), &mControl.mControl[Vehicle::CONTROL_RIGHT3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_UP), &mControl.mControl[Vehicle::CONTROL_UP]),
		KeyValue(_T(RTVAR_CTRL_STEER_DOWN), &mControl.mControl[Vehicle::CONTROL_DOWN]),
		KeyValue(_T(RTVAR_CTRL_STEER_UP3D), &mControl.mControl[Vehicle::CONTROL_UP3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_DOWN3D), &mControl.mControl[Vehicle::CONTROL_DOWN3D]),
		KeyValue(_T(RTVAR_CTRL_STEER_HANDBRK), &mControl.mControl[Vehicle::CONTROL_HANDBREAK]),
		KeyValue(_T(RTVAR_CTRL_STEER_BRK), &mControl.mControl[Vehicle::CONTROL_BREAK]),
	};
	SetValuePointers(lEntries, LEPRA_ARRAY_SIZE(lEntries));
}



void ClientOptionsManager::operator=(const ClientOptionsManager&)
{
	assert(false);
}



LOG_CLASS_DEFINE(UI_INPUT, ClientOptionsManager);



}
}
