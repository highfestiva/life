
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/LepraAssert.h"
#include <iomanip>
#include <iostream>
#include <strstream>
#include "../Include/Logger.h"
#include "../Include/LogListener.h"
#include "../Include/SpinLock.h"



namespace Lepra
{



Logger::Logger(const str& pName, Logger* pParent, LogLevel pLevel):
	mName(pName),
	mParent(pParent),
	mLoggerListLock(new SpinLock),
	mLevel(pLevel)
{
}

Logger::~Logger()
{
	delete mLoggerListLock;
}

void Logger::SetupBasicListeners(LogListener* pConsole, LogListener* pDebug,
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

void Logger::AddListener(LogListener* pLogger, LogLevel pLevel)
{
	deb_assert(pLogger);
#ifdef NO_LEVEL_DEBUG_INFO	// Final (public release) version gets no debug messages.
	if (pLevel >= LEVEL_INFO)
#endif // !NO_LEVEL_DEBUG_INFO
	{
		deb_assert(pLevel >= LEVEL_LOWEST_TYPE && pLevel < LEVEL_TYPE_COUNT);
		if (pLevel >= LEVEL_LOWEST_TYPE && pLevel < LEVEL_TYPE_COUNT)
		{
			ScopeSpinLock lScopeLock(mLoggerListLock);
			mLoggerList[pLevel].push_back(pLogger);
			pLogger->AddLog(this);
		}
	}
}

void Logger::RemoveListener(LogListener* pLogger)
{
	deb_assert(pLogger);
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

LogListener* Logger::GetListener(const str& pName) const
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

const str& Logger::GetName() const
{
	return (mName);
}

LogLevel Logger::GetLevelThreashold()const
{
	return (mLevel);
}

void Logger::SetLevelThreashold(LogLevel pLevel)
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

void Logger::Print(const str& pAccount, const str& pMessage, LogLevel pLevel)
{
	if (pLevel >= mLevel && pLevel < LEVEL_TYPE_COUNT)
	{
		DoPrint(this, pAccount, pMessage, pLevel);
	}
}

void Logger::RawPrint(const str& pMessage, LogLevel pLevel)
{
	if (pLevel >= mLevel && pLevel < LEVEL_TYPE_COUNT)
	{
		DoRawPrint(pMessage, pLevel);
	}
}

void Logger::DoPrint(const Logger* pOriginator, const str& pAccount, const str& pMessage, LogLevel pLevel)
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

void Logger::DoRawPrint(const str& pMessage, LogLevel pLevel)
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



}
