
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Cure.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



RuntimeVariableScope* mCureScope;



void Init()
{
	mCureScope = new RuntimeVariableScope(0);
	SetDefault(mCureScope);
}

void Shutdown()
{
	delete (mCureScope);
	mCureScope = 0;
}

void SetDefault(RuntimeVariableScope* pSettings)
{
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_COLUMNSPACING, 2);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_CHARACTERWIDTH, 80);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_CHARACTERDELIMITORS, _T(" \t\v\r\n.,!?:;/\\'\"()&%#@{[]+-*|<>^_$`"));
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_COMPLETION, (int)'\t');
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_ENTER, (int)'\r');
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_BACKSPACE, (int)'\b');
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_DELETE, ConsolePrompt::CON_KEY_DELETE);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_CTRLLEFT, ConsolePrompt::CON_KEY_CTRL_LEFT);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_CTRLRIGHT, ConsolePrompt::CON_KEY_CTRL_RIGHT);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_HOME, ConsolePrompt::CON_KEY_HOME);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_END, ConsolePrompt::CON_KEY_END);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_UP, ConsolePrompt::CON_KEY_UP);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_DOWN, ConsolePrompt::CON_KEY_DOWN);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_LEFT, ConsolePrompt::CON_KEY_LEFT);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_RIGHT, ConsolePrompt::CON_KEY_RIGHT);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_ESC, ConsolePrompt::CON_KEY_ESCAPE);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_PAGEUP, ConsolePrompt::CON_KEY_PAGE_UP);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_PAGEDOWN, ConsolePrompt::CON_KEY_PAGE_DOWN);

	CURE_RTVAR_SET(pSettings, RTVAR_NETPHYS_RESYNCONDIFFGT, 100.0);
	CURE_RTVAR_SET(pSettings, RTVAR_NETPHYS_SLOWFRAMECOUNT, 2);

	CURE_RTVAR_SET(pSettings, RTVAR_NETWORK_CONNECT_RETRYCOUNT, 3);
	CURE_RTVAR_SET(pSettings, RTVAR_NETWORK_CONNECT_LOCALPORTRANGE, _T("1025-65535"));
	CURE_RTVAR_SET(pSettings, RTVAR_NETWORK_CONNECT_TIMEOUT, 3.0);
	CURE_RTVAR_SET(pSettings, RTVAR_NETWORK_LOGIN_TIMEOUT, 3.0);
	CURE_RTVAR_SET(pSettings, RTVAR_NETWORK_KEEPALIVE_SENDINTERVAL, 5.0);
	CURE_RTVAR_SET(pSettings, RTVAR_NETWORK_KEEPALIVE_PINGINTERVAL, 7.0);
	CURE_RTVAR_SET(pSettings, RTVAR_NETWORK_KEEPALIVE_PINGRETRYCOUNT, 4);
	CURE_RTVAR_SET(pSettings, RTVAR_NETWORK_KEEPALIVE_KILLINTERVAL, 20.0);

	CURE_RTVAR_SET(pSettings, RTVAR_PHYSICS_FPS, 60);
	CURE_RTVAR_SET(pSettings, RTVAR_PHYSICS_MICROSTEPS, 3);
	CURE_RTVAR_SET(pSettings, RTVAR_PHYSICS_PARALLEL, true);

	CURE_RTVAR_SET(pSettings, RTVAR_PERFORMANCE_TEXT_INTERVAL, 10.0);
	CURE_RTVAR_SET(pSettings, RTVAR_PERFORMANCE_TEXT_ENABLE, false);
}

RuntimeVariableScope* GetSettings()
{
	return (mCureScope);
}



}
