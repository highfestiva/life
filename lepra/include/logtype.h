
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Some (optional to use) simple and standard log subsystems.



#pragma once

#include <vector>
#include "loglevel.h"
#include "string.h"



namespace lepra {



class Logger;



class LogType {
public:
	enum Subsystem {
		kLowestType = 0,
		kRoot = kLowestType,	// Try to avoid using base log directly.
		kGeneral,			// General data structures and algoritms.
		kGeneralResources,		// File handling, etc.
		kNetwork,
		kNetworkClient,
		kNetworkServer,
		kConsole,
		kPhysics,
		kUi,
		kUiInput,
		kUiSound,
		kUiGfx,
		kUiGfx2D,
		kUiGfx3D,
		kGame,
		kGameContext,
		kGameContextCpp,
		kTest,
		kTypeCount
	};

	static void Init();
	static void Close();

	static Logger* GetLogger(Subsystem subsystem);
	static Logger* GetLogger(const str& name);
	static const std::vector<Logger*> GetLoggers();
	static void SetLogLevel(LogLevel level);

private:
	static bool initialized_;
	static Logger* logger_array_[kTypeCount];
};



}
