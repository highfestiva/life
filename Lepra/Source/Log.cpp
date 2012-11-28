
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include <iomanip>
#include <iostream>
#include <strstream>
#include "../Include/Log.h"
#include "../Include/LogListener.h"
#include "../Include/SpinLock.h"



namespace Lepra
{



Log::Log(const str& pName, Log* pParent, LogLevel pLevel):
	mName(pName),
	mParent(pParent),
	mLoggerListLock(new SpinLock),
	mLevel(pLevel)
{
}

Log::~Log()
{
	delete mLoggerListLock;
}

void Log::SetupBasicListeners(LogListener* pConsole, LogListener* pDebug,
	LogListener* pFile, LogListener* pPerformance, LogListener* pMem)
{
	if (pConsole)
	{
		// Console logs everything user wants.
		for (int x = LEVEL_LOWEST_TYPE; x < LEVEL_TYPE_COUNT; ++x)
		{
			AddListener(pConsole, (LogLevel)x);
		}
	}

	if (pDebug)
	{
#ifndef NO_LOG_DEBUG_INFO	// Only available in debug and release candidates.
		for (int x = LEVEL_LOWEST_TYPE; x < LEVEL_TYPE_COUNT; ++x)
		{
			AddListener(pDebug, (LogLevel)x);
		}
#endif // !NO_LOG_DEBUG_INFO
	}

	if (pFile)
	{
		// File logs everything but performance.
		AddListener(pFile, LEVEL_TRACE);
		AddListener(pFile, LEVEL_DEBUG);
		AddListener(pFile, LEVEL_INFO);
		AddListener(pFile, LEVEL_HEADLINE);
		AddListener(pFile, LEVEL_WARNING);
		AddListener(pFile, LEVEL_ERROR);
		AddListener(pFile, LEVEL_FATAL);
	}

	if (pPerformance)
	{
		AddListener(pPerformance, LEVEL_PERFORMANCE);
		AddListener(pPerformance, LEVEL_HEADLINE);
	}

	if (pMem)
	{
		for (int x = LEVEL_LOWEST_TYPE; x < LEVEL_TYPE_COUNT; ++x)
		{
			AddListener(pMem, (LogLevel)x);
		}
	}
}

void Log::AddListener(LogListener* pLogger, LogLevel pLevel)
{
	assert(pLogger);
#ifdef NO_LEVEL_DEBUG_INFO	// Final (public release) version gets no debug messages.
	if (pLevel >= LEVEL_INFO)
#endif // !NO_LEVEL_DEBUG_INFO
	{
		assert (pLevel >= LEVEL_LOWEST_TYPE && pLevel < LEVEL_TYPE_COUNT);
		if (pLevel >= LEVEL_LOWEST_TYPE && pLevel < LEVEL_TYPE_COUNT)
		{
			ScopeSpinLock lScopeLock(mLoggerListLock);
			mLoggerList[pLevel].push_back(pLogger);
			pLogger->AddLog(this);
		}
	}
}

void Log::RemoveListener(LogListener* pLogger)
{
	assert(pLogger);
	ScopeSpinLock lScopeLock(mLoggerListLock);
	for (int x = LEVEL_LOWEST_TYPE; x < LEVEL_TYPE_COUNT; ++x)
	{
		std::vector<LogListener*>::iterator y = mLoggerList[x].begin();
		for (; y != mLoggerList[x].end(); ++y)
		{
			if (*y == pLogger)
			{
				mLoggerList[x].erase(y);
				break;
			}
		}
	}
	pLogger->RemoveLog(this);
}

LogListener* Log::GetListener(const str& pName) const
{
	LogListener* lListener = 0;
	ScopeSpinLock lScopeLock(mLoggerListLock);
	for (int x = LEVEL_LOWEST_TYPE; !lListener && x < LEVEL_TYPE_COUNT; ++x)
	{
		std::vector<LogListener*>::const_iterator y = mLoggerList[x].begin();
		for (; !lListener && y != mLoggerList[x].end(); ++y)
		{
			if ((*y)->GetName() == pName)
			{
				lListener = *y;
			}
		}
	}
	return (lListener);
}

const str& Log::GetName() const
{
	return (mName);
}

LogLevel Log::GetLevelThreashold()const
{
	return (mLevel);
}

void Log::SetLevelThreashold(LogLevel pLevel)
{
	if (pLevel < LEVEL_LOWEST_TYPE)
	{
		pLevel = LEVEL_LOWEST_TYPE;
	}
	else if (pLevel >= LEVEL_TYPE_COUNT)
	{
		pLevel = (LogLevel)(LEVEL_TYPE_COUNT-1);
	}
	mLevel = pLevel;
}

void Log::Print(const str& pAccount, const str& pMessage, LogLevel pLevel)
{
	if (pLevel >= mLevel && pLevel < LEVEL_TYPE_COUNT)
	{
		DoPrint(this, pAccount, pMessage, pLevel);
	}
}

void Log::RawPrint(const str& pMessage, LogLevel pLevel)
{
	if (pLevel >= mLevel && pLevel < LEVEL_TYPE_COUNT)
	{
		DoRawPrint(pMessage, pLevel);
	}
}

void Log::DoPrint(const Log* pOriginator, const str& pAccount, const str& pMessage, LogLevel pLevel)
{
	if (mParent)
	{
		mParent->DoPrint(this, pAccount, pMessage, pLevel);
	}

	ScopeSpinLock lScopeLock(mLoggerListLock);
	std::vector<LogListener*>::iterator x = mLoggerList[pLevel].begin();
	for (; x != mLoggerList[pLevel].end(); ++x)
	{
		(*x)->OnLog(pOriginator, pAccount, pMessage, pLevel);
	}
}

void Log::DoRawPrint(const str& pMessage, LogLevel pLevel)
{
	if (mParent)
	{
		mParent->DoRawPrint(pMessage, pLevel);
	}

	ScopeSpinLock lScopeLock(mLoggerListLock);
	std::vector<LogListener*>::iterator x = mLoggerList[pLevel].begin();
	for (; x != mLoggerList[pLevel].end(); ++x)
	{
		(*x)->WriteLog(pMessage, pLevel);
	}
}



LogDecorator::LogDecorator(Log* pLog, const std::type_info& pTypeId):
	mLog(pLog)
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
	mLog->Print(mClassName, pText, pLogLevel);
}

void LogDecorator::RawPrint(LogLevel pLogLevel, const str& pText) const
{
	mLog->RawPrint(pText, pLogLevel);
}

#define StrV()	\
	va_list	lArguments;	\
	va_start(lArguments, pText);	\
	str lText = strutil::VFormat(pText, lArguments);	\
	va_end(lArguments)

#define StrVLog(level)	\
	StrV();	\
	mLog->Print(mClassName, lText, level)

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
