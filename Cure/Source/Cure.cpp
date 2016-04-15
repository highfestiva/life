
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/Cure.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



RuntimeVariableScope* gCureScope = 0;



void Init()
{
	if (!gCureScope)
	{
		gCureScope = new RuntimeVariableScope(0);
	}
	SetDefault(gCureScope);
}

void Shutdown()
{
	delete (gCureScope);
	gCureScope = 0;
}

void SetDefault(RuntimeVariableScope* pSettings)
{
	v_set(pSettings, RTVAR_CONSOLE_COLUMNSPACING, 2);
	v_set(pSettings, RTVAR_CONSOLE_CHARACTERWIDTH, 80);
	v_set(pSettings, RTVAR_CONSOLE_CHARACTERDELIMITORS, " \t\v\r\n.,!?:;/\\'\"()&%#@{[]+-*|<>^_$`");
	v_set(pSettings, RTVAR_CONSOLE_KEY_COMPLETION, (int)'\t');
	v_set(pSettings, RTVAR_CONSOLE_KEY_ENTER, (int)'\r');
	v_set(pSettings, RTVAR_CONSOLE_KEY_SILENT, (int)'\v');
	v_set(pSettings, RTVAR_CONSOLE_KEY_BACKSPACE, (int)'\b');
	v_set(pSettings, RTVAR_CONSOLE_KEY_DELETE, ConsolePrompt::CON_KEY_DELETE);
	v_set(pSettings, RTVAR_CONSOLE_KEY_CTRLLEFT, ConsolePrompt::CON_KEY_CTRL_LEFT);
	v_set(pSettings, RTVAR_CONSOLE_KEY_CTRLRIGHT, ConsolePrompt::CON_KEY_CTRL_RIGHT);
	v_set(pSettings, RTVAR_CONSOLE_KEY_HOME, ConsolePrompt::CON_KEY_HOME);
	v_set(pSettings, RTVAR_CONSOLE_KEY_END, ConsolePrompt::CON_KEY_END);
	v_set(pSettings, RTVAR_CONSOLE_KEY_UP, ConsolePrompt::CON_KEY_UP);
	v_set(pSettings, RTVAR_CONSOLE_KEY_DOWN, ConsolePrompt::CON_KEY_DOWN);
	v_set(pSettings, RTVAR_CONSOLE_KEY_LEFT, ConsolePrompt::CON_KEY_LEFT);
	v_set(pSettings, RTVAR_CONSOLE_KEY_RIGHT, ConsolePrompt::CON_KEY_RIGHT);
	v_set(pSettings, RTVAR_CONSOLE_KEY_ESC, ConsolePrompt::CON_KEY_ESCAPE);
	v_set(pSettings, RTVAR_CONSOLE_KEY_PAGEUP, ConsolePrompt::CON_KEY_PAGE_UP);
	v_set(pSettings, RTVAR_CONSOLE_KEY_PAGEDOWN, ConsolePrompt::CON_KEY_PAGE_DOWN);

	v_set(pSettings, RTVAR_DEBUG_ENABLE, false);

	v_set(pSettings, RTVAR_NETPHYS_POSSENDINTERVALLIMIT, 0.5);
	v_set(pSettings, RTVAR_NETPHYS_RESYNCONDIFFGT, 0.2);
	v_set(pSettings, RTVAR_NETPHYS_SLOWFRAMECOUNT, 2);
	v_set(pSettings, RTVAR_NETPHYS_ENABLESMOOTHING, true);
	v_set(pSettings, RTVAR_NETPHYS_EXTRAPOLATIONFACTOR, 0.0);

	v_set(pSettings, RTVAR_NETWORK_SERVERADDRESS, "localhost:16650");
	v_set(pSettings, RTVAR_NETWORK_CONNECT_RETRYCOUNT, 1);
	v_set(pSettings, RTVAR_NETWORK_CONNECT_LOCALPORTRANGE, "1025-65535");
	v_set(pSettings, RTVAR_NETWORK_CONNECT_TIMEOUT, 3.0);
	v_set(pSettings, RTVAR_NETWORK_LOGIN_TIMEOUT, 3.0);
	v_set(pSettings, RTVAR_NETWORK_KEEPALIVE_SENDINTERVAL, 5.0);
	v_set(pSettings, RTVAR_NETWORK_KEEPALIVE_PINGINTERVAL, 7.0);
	v_set(pSettings, RTVAR_NETWORK_KEEPALIVE_PINGRETRYCOUNT, 4);
	v_set(pSettings, RTVAR_NETWORK_KEEPALIVE_KILLINTERVAL, 20.0);

	v_set(pSettings, RTVAR_PHYSICS_FPS, CURE_STANDARD_FRAME_RATE);
	v_set(pSettings, RTVAR_PHYSICS_RTR, 1.0);
	v_set(pSettings, RTVAR_PHYSICS_MICROSTEPS, 3);
	v_internal(pSettings, RTVAR_PHYSICS_FASTALGO, true);
	v_set(pSettings, RTVAR_PHYSICS_HALT, false);
	v_set(pSettings, RTVAR_PHYSICS_PARALLEL, true);
	v_set(pSettings, RTVAR_PHYSICS_ISFIXEDFPS, false);
	v_set(pSettings, RTVAR_PHYSICS_NOCLIP, false);

	v_set(pSettings, RTVAR_PERFORMANCE_TEXT_INTERVAL, 1.0);
	v_set(pSettings, RTVAR_PERFORMANCE_TEXT_ENABLE, false);
}

RuntimeVariableScope* GetSettings()
{
	return (gCureScope);
}



}
