
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

// Some (optional to use) simple and standard log subsystems.



#pragma once

#include <vector>
#include "LogLevel.h"
#include "String.h"



namespace Lepra
{



class Log;



class LogType
{
public:
	enum Subsystem
	{
		SUB_LOWEST_TYPE = 0,
		SUB_ROOT = SUB_LOWEST_TYPE,	// Try to avoid using base log directly.
		SUB_GENERAL,			// General data structures and algoritms.
		SUB_GENERAL_RESOURCES,		// File handling, etc.
		SUB_NETWORK,
		SUB_NETWORK_CLIENT,
		SUB_NETWORK_SERVER,
		SUB_CONSOLE,
		SUB_PHYSICS,
		SUB_UI,
		SUB_UI_INPUT,
		SUB_UI_SOUND,
		SUB_UI_GFX,
		SUB_UI_GFX_2D,
		SUB_UI_GFX_3D,
		SUB_GAME,
		SUB_GAME_CONTEXT,
		SUB_GAME_CONTEXT_CPP,
		SUB_TEST,
		SUB_TYPE_COUNT
	};

	static void Init();
	static void Close();

	static Log* GetLog(Subsystem pSubsystem);
	static Log* GetLog(const str& pName);
	static const std::vector<Log*> GetLogs();
	static void SetLogLevel(LogLevel pLevel);

private:
	static bool mInitialized;
	static Log* mLogArray[SUB_TYPE_COUNT];
};



}
