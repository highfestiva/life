
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Some (optional to use) simple and standard log subsystems.



#pragma once

#include <vector>
#include "LogLevel.h"
#include "String.h"



namespace Lepra
{



class Logger;



class LogType
{
public:
	enum Subsystem
	{
		LOWEST_TYPE = 0,
		ROOT = LOWEST_TYPE,	// Try to avoid using base log directly.
		GENERAL,			// General data structures and algoritms.
		GENERAL_RESOURCES,		// File handling, etc.
		NETWORK,
		NETWORK_CLIENT,
		NETWORK_SERVER,
		CONSOLE,
		PHYSICS,
		UI,
		UI_INPUT,
		UI_SOUND,
		UI_GFX,
		UI_GFX_2D,
		UI_GFX_3D,
		GAME,
		GAME_CONTEXT,
		GAME_CONTEXT_CPP,
		TEST,
		TYPE_COUNT
	};

	static void Init();
	static void Close();

	static Logger* GetLogger(Subsystem pSubsystem);
	static Logger* GetLogger(const str& pName);
	static const std::vector<Logger*> GetLoggers();
	static void SetLogLevel(LogLevel pLevel);

private:
	static bool mInitialized;
	static Logger* mLoggerArray[TYPE_COUNT];
};



}
