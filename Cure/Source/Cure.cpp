
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



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
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_CHARACTERDELIMITORS, _T(" \t\v\r\n.,!?:;/\\'\"()&%#@{[]+-*|<>^_$`´"));
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_COMPLETION, (int)'\t');
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_ENTER, (int)'\r');
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_BACKSPACE, (int)'\b');
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_DELETE, Lepra::ConsolePrompt::KEY_DELETE);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_CTRLLEFT, Lepra::ConsolePrompt::KEY_CTRL_LEFT);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_CTRLRIGHT, Lepra::ConsolePrompt::KEY_CTRL_RIGHT);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_HOME, Lepra::ConsolePrompt::KEY_HOME);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_END, Lepra::ConsolePrompt::KEY_END);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_UP, Lepra::ConsolePrompt::KEY_UP);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_DOWN, Lepra::ConsolePrompt::KEY_DOWN);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_LEFT, Lepra::ConsolePrompt::KEY_LEFT);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_RIGHT, Lepra::ConsolePrompt::KEY_RIGHT);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_ESC, Lepra::ConsolePrompt::KEY_ESCAPE);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_PAGEUP, Lepra::ConsolePrompt::KEY_PAGE_UP);
	CURE_RTVAR_SET(pSettings, RTVAR_CONSOLE_KEY_PAGEDOWN, Lepra::ConsolePrompt::KEY_PAGE_DOWN);

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

	CURE_RTVAR_SET(pSettings, RTVAR_PERFORMANCE_TEXT_INTERVAL, 10.0);
	CURE_RTVAR_SET(pSettings, RTVAR_PERFORMANCE_TEXT_ENABLE, false);

	CURE_RTVAR_SET(pSettings, RTVAR_TICK_INTERVAL, 0.009);
}

RuntimeVariableScope* GetSettings()
{
	return (mCureScope);
}



}
