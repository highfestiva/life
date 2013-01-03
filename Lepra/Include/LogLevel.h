
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

// Use LogDecorator and pass typeid(class) as constructor parameter.



#pragma once



namespace Lepra
{



enum LogLevel
{
	LEVEL_LOWEST_TYPE = 0,
	LEVEL_TRACE = LEVEL_LOWEST_TYPE,	// Use for tracing where execution goes.
	LEVEL_DEBUG,				// Used for developer information messages. Will not be logged in the final version.
	LEVEL_PERFORMANCE,			// Use when printing performance data.
	LEVEL_INFO,				// Used for normal logging.
	LEVEL_HEADLINE,				// Used for important events.
	LEVEL_WARNING,				// Used for non critical errors.
	LEVEL_ERROR,				// Used for critical errors.
	LEVEL_FATAL,				// Even more critial, when it's time to panic or something... :)
	LEVEL_TYPE_COUNT
};



}
