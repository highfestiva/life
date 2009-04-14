
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include <assert.h>
#include "../Include/Log.h"
#include "../Include/LogListener.h"



namespace Lepra
{



Log::Log(const String& pName, Log* pParent, LogLevel pLevel):
	mName(pName),
	mParent(pParent),
	mLevel(pLevel)
{
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
			//ScopeLock lScopeLock(&mLock);
			mLoggerList[pLevel].push_back(pLogger);
			pLogger->AddLog(this);
		}
	}
}

void Log::RemoveListener(LogListener* pLogger)
{
	assert(pLogger);
	for (int x = LEVEL_LOWEST_TYPE; x < LEVEL_TYPE_COUNT; ++x)
	{
		std::list<LogListener*>::iterator y = mLoggerList[x].begin();
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

LogListener* Log::GetListener(const Lepra::String& pName) const
{
	LogListener* lListener = 0;
	for (int x = LEVEL_LOWEST_TYPE; !lListener && x < LEVEL_TYPE_COUNT; ++x)
	{
		std::list<LogListener*>::const_iterator y = mLoggerList[x].begin();
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

String Log::GetName() const
{
	return (mName);
}

Log::LogLevel Log::GetLevelThreashold()const
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

void Log::Print(const String& pAccount, const String& pMessage, LogLevel pLevel)
{
	DoPrint(this, pAccount, pMessage, pLevel);
}

void Log::RawPrint(const String& pMessage, LogLevel pLevel)
{
	DoRawPrint(pMessage, pLevel);
}

void Log::DoPrint(const Log* pOriginator, const String& pAccount, const String& pMessage, LogLevel pLevel)
{
	if (pLevel >= mLevel && pLevel < LEVEL_TYPE_COUNT)
	{
		if (mParent)
		{
			mParent->DoPrint(this, pAccount, pMessage, pLevel);
		}

		std::list<LogListener*>::iterator x = mLoggerList[pLevel].begin();
		for (; x != mLoggerList[pLevel].end(); ++x)
		{
			(*x)->OnLog(pOriginator, pAccount, pMessage, pLevel);
		}
	}
}

void Log::DoRawPrint(const String& pMessage, LogLevel pLevel)
{
	if (pLevel >= mLevel && pLevel < LEVEL_TYPE_COUNT)
	{
		if (mParent)
		{
			mParent->DoRawPrint(pMessage, pLevel);
		}

		std::list<LogListener*>::iterator x = mLoggerList[pLevel].begin();
		for (; x != mLoggerList[pLevel].end(); ++x)
		{
			(*x)->WriteLog(pMessage, pLevel);
		}
	}
}



LogDecorator::LogDecorator(Log* pLog, const type_info& pTypeId):
	mLog(pLog),
#ifdef LEPRA_MSVC
	// Skip "class " in beginning of name.
	mClassName(AnsiStringUtility::ToCurrentCode(Lepra::AnsiString(pTypeId.name()+6)))
#elif // !MSVC
#error typeid parsing not implemented.
#endif // MSVC/!MSVC
{
}

void LogDecorator::Print(Log::LogLevel pLogLevel, const String& pText) const
{
	mLog->Print(mClassName, pText, pLogLevel);
}

void LogDecorator::RawPrint(Log::LogLevel pLogLevel, const String& pText) const
{
	mLog->RawPrint(pText, pLogLevel);
}

#define StrV()	\
	va_list	lArguments;	\
	va_start(lArguments, pText);	\
	String lText = StringUtility::VFormat(pText, lArguments)

#define StrVLog(level)	\
	StrV();	\
	mLog->Print(mClassName, lText, level)

#ifndef NO_LOG_DEBUG_INFO
void LogDecorator::Tracef(const tchar* pText, ...) const
{
	StrVLog(Log::LEVEL_TRACE);
}

void LogDecorator::Trace(const String& pText) const
{
	Print(Log::LEVEL_TRACE, pText);
}

void LogDecorator::Debugf(const tchar* pText, ...) const
{
	StrVLog(Log::LEVEL_DEBUG);
}

void LogDecorator::Debug(const String& pText) const
{
	Print(Log::LEVEL_DEBUG, pText);
}
#endif // !NO_LOG_DEBUG_INFO

void LogDecorator::Performancef(const tchar* pText, ...) const
{
	StrVLog(Log::LEVEL_PERFORMANCE);
}

void LogDecorator::Performance(const String& pText) const
{
	Print(Log::LEVEL_PERFORMANCE, pText);
}

void LogDecorator::Infof(const tchar* pText, ...) const
{
	StrVLog(Log::LEVEL_INFO);
}

void LogDecorator::Info(const String& pText) const
{
	Print(Log::LEVEL_INFO, pText);
}

void LogDecorator::Headlinef(const tchar* pText, ...) const
{
	StrVLog(Log::LEVEL_HEADLINE);
}

void LogDecorator::Headline(const String& pText) const
{
	Print(Log::LEVEL_HEADLINE, pText);
}

void LogDecorator::Warningf(const tchar* pText, ...) const
{
	StrVLog(Log::LEVEL_WARNING);
}

void LogDecorator::Warning(const String& pText) const
{
	Print(Log::LEVEL_WARNING, pText);
}

void LogDecorator::Errorf(const tchar* pText, ...) const
{
	StrVLog(Log::LEVEL_ERROR);
}

void LogDecorator::Error(const String& pText) const
{
	Print(Log::LEVEL_ERROR, pText);
}

void LogDecorator::Fatalf(const tchar* pText, ...) const
{
	StrVLog(Log::LEVEL_FATAL);
}

void LogDecorator::Fatal(const String& pText) const
{
	Print(Log::LEVEL_FATAL, pText);
}

const String& LogDecorator::GetClassName() const
{
	return (mClassName);
}



}
