
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

// Use LogDecorator and pass typeid(class) as constructor parameter.



#pragma once



namespace lepra {



enum LogLevel {
	kLevelLowestType = 0,
	kLevelTrace = kLevelLowestType,	// Use for tracing where execution goes.
	kLevelDebug,				// Used for developer information messages. Will not be logged in the final version.
	kLevelPerformance,			// Use when printing performance data.
	kLevelInfo,				// Used for normal logging.
	kLevelHeadline,				// Used for important events.
	kLevelWarning,				// Used for non critical errors.
	kLevelError,				// Used for critical errors.
	kLevelFatal,				// Even more critial, when it's time to panic or something... :)
	kLevelTypeCount
};



}
