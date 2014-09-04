
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/Log.h"
#include "../Include/Logger.h"



namespace Lepra
{



LogDecorator::LogDecorator(Logger* pLogger, const std::type_info& pTypeId):
	mLogger(pLogger)
#ifdef LEPRA_MSVC
	// Skip "class " in beginning of name.
	, mClassName(strutil::Encode(astr(pTypeId.name()+6)))
{
#elif defined(LEPRA_POSIX)
{
	// Parse up to class name.
	// The format looks something like "1w5Lepra14TestBajjaMajjaE", thus (((BCD length)(data))...).
	// If in global namespace, the namespace (len)(data) pair is left out.
	const char* s = pTypeId.name();
	const size_t lLength = ::strlen(s);
	int lWordCount = 0;
	int lTargetWordCount = 1;
	size_t x = 0;
	size_t lStartOfWord = 0;
	size_t lStepLength = 1;
	for (; lWordCount < lTargetWordCount && x < lLength && lStepLength > 0; ++lWordCount, x += lStepLength)
	{
		for (; x < lLength && ::isalpha(s[x]); ++x)
			;
		char* lEndPtr;
		long lWordLength = astrutil::StrToL(&s[x], &lEndPtr, 10);
		lStepLength = lEndPtr-&s[x];
		lStartOfWord = x+lStepLength;
		lStepLength = (lStepLength < 1)? 1 : lStepLength;
		lStepLength += lWordLength;
		if (lWordCount == 0 && x+lStepLength < lLength && ::isdigit(s[x+lStepLength]))
		{
			++lTargetWordCount;
		}
	}
	if (x <= lLength)
	{
		astr lCrop(&s[lStartOfWord], x-lStartOfWord);
		mClassName = strutil::Encode(lCrop);
	}
	else
	{
		mClassName = strutil::Encode(s);
	}
#else // !MSVC
#error typeid parsing not implemented.
#endif // MSVC/!MSVC
}

void LogDecorator::Print(LogLevel pLogLevel, const str& pText) const
{
	mLogger->Print(mClassName, pText, pLogLevel);
}

void LogDecorator::RawPrint(LogLevel pLogLevel, const str& pText) const
{
	mLogger->RawPrint(pText, pLogLevel);
}

#define StrV()	\
	va_list	lArguments;	\
	va_start(lArguments, pText);	\
	str lText = strutil::VFormat(pText, lArguments);	\
	va_end(lArguments)

#define StrVLog(level)	\
	StrV();	\
	mLogger->Print(mClassName, lText, level)

#ifndef NO_LOG_DEBUG_INFO
void LogDecorator::Tracef(const tchar* pText, ...) const
{
	StrVLog(LEVEL_TRACE);
}

void LogDecorator::Trace(const str& pText) const
{
	Print(LEVEL_TRACE, pText);
}

void LogDecorator::Debugf(const tchar* pText, ...) const
{
	StrVLog(LEVEL_DEBUG);
}

void LogDecorator::Debug(const str& pText) const
{
	Print(LEVEL_DEBUG, pText);
}
#endif // !NO_LOG_DEBUG_INFO

void LogDecorator::Performancef(const tchar* pText, ...) const
{
	StrVLog(LEVEL_PERFORMANCE);
}

void LogDecorator::Performance(const str& pText) const
{
	Print(LEVEL_PERFORMANCE, pText);
}

void LogDecorator::Infof(const tchar* pText, ...) const
{
	StrVLog(LEVEL_INFO);
}

void LogDecorator::Info(const str& pText) const
{
	Print(LEVEL_INFO, pText);
}

void LogDecorator::Headlinef(const tchar* pText, ...) const
{
	StrVLog(LEVEL_HEADLINE);
}

void LogDecorator::Headline(const str& pText) const
{
	Print(LEVEL_HEADLINE, pText);
}

void LogDecorator::Warningf(const tchar* pText, ...) const
{
	StrVLog(LEVEL_WARNING);
}

void LogDecorator::Warning(const str& pText) const
{
	Print(LEVEL_WARNING, pText);
}

void LogDecorator::Errorf(const tchar* pText, ...) const
{
	StrVLog(LEVEL_ERROR);
}

void LogDecorator::Error(const str& pText) const
{
	Print(LEVEL_ERROR, pText);
}

void LogDecorator::Fatalf(const tchar* pText, ...) const
{
	StrVLog(LEVEL_FATAL);
}

void LogDecorator::Fatal(const str& pText) const
{
	Print(LEVEL_FATAL, pText);
}

const str& LogDecorator::GetClassName() const
{
	return (mClassName);
}



}
