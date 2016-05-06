
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/cure.h"
#include "../include/runtimevariable.h"



namespace cure {



RuntimeVariableScope* g_cure_scope = 0;



void Init() {
	if (!g_cure_scope) {
		g_cure_scope = new RuntimeVariableScope(0);
	}
	SetDefault(g_cure_scope);
}

void Shutdown() {
	delete (g_cure_scope);
	g_cure_scope = 0;
}

void SetDefault(RuntimeVariableScope* settings) {
	v_set(settings, kRtvarConsoleColumnspacing, 2);
	v_set(settings, kRtvarConsoleCharacterwidth, 80);
	v_set(settings, kRtvarConsoleCharacterdelimitors, " \t\v\r\n.,!?:;/\\'\"()&%#@{[]+-*|<>^_$`");
	v_set(settings, kRtvarConsoleKeyCompletion, (int)'\t');
	v_set(settings, kRtvarConsoleKeyEnter, (int)'\r');
	v_set(settings, kRtvarConsoleKeySilent, (int)'\v');
	v_set(settings, kRtvarConsoleKeyBackspace, (int)'\b');
	v_set(settings, kRtvarConsoleKeyDelete, ConsolePrompt::kConKeyDelete);
	v_set(settings, kRtvarConsoleKeyCtrlleft, ConsolePrompt::kConKeyCtrlLeft);
	v_set(settings, kRtvarConsoleKeyCtrlright, ConsolePrompt::kConKeyCtrlRight);
	v_set(settings, kRtvarConsoleKeyHome, ConsolePrompt::kConKeyHome);
	v_set(settings, kRtvarConsoleKeyEnd, ConsolePrompt::kConKeyEnd);
	v_set(settings, kRtvarConsoleKeyUp, ConsolePrompt::kConKeyUp);
	v_set(settings, kRtvarConsoleKeyDown, ConsolePrompt::kConKeyDown);
	v_set(settings, kRtvarConsoleKeyLeft, ConsolePrompt::kConKeyLeft);
	v_set(settings, kRtvarConsoleKeyRight, ConsolePrompt::kConKeyRight);
	v_set(settings, kRtvarConsoleKeyEsc, ConsolePrompt::kConKeyEscape);
	v_set(settings, kRtvarConsoleKeyPageup, ConsolePrompt::kConKeyPageUp);
	v_set(settings, kRtvarConsoleKeyPagedown, ConsolePrompt::kConKeyPageDown);

	v_set(settings, kRtvarDebugEnable, false);

	v_set(settings, kRtvarNetphysPossendintervallimit, 0.5);
	v_set(settings, kRtvarNetphysResyncondiffgt, 0.2);
	v_set(settings, kRtvarNetphysSlowframecount, 2);
	v_set(settings, kRtvarNetphysEnablesmoothing, true);
	v_set(settings, kRtvarNetphysExtrapolationfactor, 0.0);

	v_set(settings, kRtvarNetworkServeraddress, "localhost:16650");
	v_set(settings, kRtvarNetworkConnectRetrycount, 1);
	v_set(settings, kRtvarNetworkConnectLocalportrange, "1025-65535");
	v_set(settings, kRtvarNetworkConnectTimeout, 3.0);
	v_set(settings, kRtvarNetworkLoginTimeout, 3.0);
	v_set(settings, kRtvarNetworkKeepaliveSendinterval, 5.0);
	v_set(settings, kRtvarNetworkKeepalivePinginterval, 7.0);
	v_set(settings, kRtvarNetworkKeepalivePingretrycount, 4);
	v_set(settings, kRtvarNetworkKeepaliveKillinterval, 20.0);

	v_set(settings, kRtvarPhysicsFps, kCureStandardFrameRate);
	v_set(settings, kRtvarPhysicsRtr, 1.0);
	v_set(settings, kRtvarPhysicsMicrosteps, 3);
	v_internal(settings, kRtvarPhysicsFastalgo, true);
	v_set(settings, kRtvarPhysicsHalt, false);
	v_set(settings, kRtvarPhysicsParallel, true);
	v_set(settings, kRtvarPhysicsIsfixedfps, false);
	v_set(settings, kRtvarPhysicsNoclip, false);

	v_set(settings, kRtvarPerformanceTextInterval, 1.0);
	v_set(settings, kRtvarPerformanceTextEnable, false);
}

RuntimeVariableScope* GetSettings() {
	return (g_cure_scope);
}



}
