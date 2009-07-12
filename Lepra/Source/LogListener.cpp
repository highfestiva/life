
// Author: Jonas Byström
// Copyright (c) 2002-2008, Jonas Byström



#include <assert.h>
#include "../Include/LepraTarget.h"
#include "../Include/LogListener.h"
#include "../Include/Time.h"



namespace Lepra
{



LogListener::LogListener(Lepra::String pName, OutputFormat pFormat):
	mLog(0),
	mName(pName),
	mLevel(Log::LEVEL_LOWEST_TYPE),
	mFormat(pFormat),
	mLogCount(1)
{
}

LogListener::~LogListener()
{
	if (mLog)
	{
		mLog->RemoveListener(this);
		assert(mLog == 0);
	}
}

void LogListener::AddLog(Log* pLog)
{
	assert(mLog == 0 || mLog == pLog);
	if (mLog == 0 || mLog == pLog)
	{
		mLog = pLog;
	}
}

void LogListener::RemoveLog(Log* pLog)
{
	assert(mLog == pLog);
	if (mLog == pLog)
	{
		mLog = 0;
	}
}

void LogListener::OnLog(const Log* pOriginator, const String& pAccount, const String& pMessage, Log::LogLevel pLevel)
{
	if (mFormat & FORMAT_CLASS)
	{
		OnLog(pOriginator, pAccount+_T(": ")+pMessage+_T("\n"), pLevel);
	}
	else
	{
		OnLog(pOriginator, pMessage+_T("\n"), pLevel);
	}
}

void LogListener::OnLog(const Log* pOriginator, const String& pMessage, Log::LogLevel pLevel)
{
	if (pLevel < mLevel)
	{
		return;	// TRICKY: optimization by early return.
	}

	String lOutputString;

	if (mFormat & FORMAT_LOGCOUNT)
	{
		lOutputString = StringUtility::Format(_T("%.5i. "), mLogCount);
	}
	++mLogCount;

	if (mFormat & FORMAT_TYPE)
	{
		switch(pLevel)
		{
			case Log::LEVEL_TRACE:		lOutputString += _T("TRACE:       ");	break;
			case Log::LEVEL_DEBUG:		lOutputString += _T("DEBUG:       ");	break;
			case Log::LEVEL_PERFORMANCE:	lOutputString += _T("PERFORMANCE: ");	break;
			case Log::LEVEL_INFO:		lOutputString += _T("INFO:        ");	break;
			case Log::LEVEL_HEADLINE:	lOutputString += _T("HEADLINE:    ");	break;
			case Log::LEVEL_WARNING:	lOutputString += _T("WARNING:     ");	break;
			case Log::LEVEL_ERROR:		lOutputString += _T("ERROR:       ");	break;
			default:	// Fall through.
			case Log::LEVEL_FATAL:		lOutputString += _T("FATAL:       ");	break;
		}
	}

	if (mFormat & FORMAT_SUBSYSTEM)
	{
		lOutputString += pOriginator->GetName();
	}

	if (mFormat & FORMAT_TIME)
	{
		Time lTime;
		lOutputString += StringUtility::Format(_T("(%i-%.2i-%.2i, %.2i:%.2i:%.2i) "), lTime.GetYear(), lTime.GetMonth(), lTime.GetDay(), lTime.GetHour(), lTime.GetMinute(), lTime.GetSecond());
	}

	if (mFormat & FORMAT_THREAD)
	{
		Thread* lThread = Thread::GetCurrentThread();
		String lThreadName;
		if (lThread)
		{
			lOutputString += StringUtility::Format(_T("%10s: "), lThread->GetThreadName().c_str());
		}
		else
		{
			lOutputString += StringUtility::Format(_T("%10s: "), _T("<Unknown>"));
		}
	}

	lOutputString += pMessage;

	WriteLog(lOutputString, pLevel);
}

Log::LogLevel LogListener::GetLevelThreashold() const
{
	return (mLevel);
}

void LogListener::SetLevelThreashold(Log::LogLevel pLevel)
{
	mLevel = pLevel;
}

const Lepra::String& LogListener::GetName() const
{
	return (mName);
}



StdioConsoleLogListener::StdioConsoleLogListener(OutputFormat pFormat):
	LogListener(_T("console"), pFormat)
{
}

void StdioConsoleLogListener::WriteLog(const String& pFullMessage, Log::LogLevel pLevel)
{
#if defined(LEPRA_WINDOWS)
	HANDLE lStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);

	// Set console text color. Default color is light gray.
	WORD lAttributes;
	switch(pLevel)
	{
		case Log::LEVEL_TRACE:	// Fall through.
		case Log::LEVEL_DEBUG:	// Fall through.
		case Log::LEVEL_INFO:		lAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;				break;
		case Log::LEVEL_PERFORMANCE:	lAttributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN;						break;
		case Log::LEVEL_HEADLINE:	lAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;	break;
		case Log::LEVEL_WARNING:	lAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;			break;
		case Log::LEVEL_ERROR:		lAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED;						break;
		default:	// Fall through.
		case Log::LEVEL_FATAL:		lAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_INTENSITY | BACKGROUND_RED;	break;
	}
	::SetConsoleTextAttribute(lStdOut, lAttributes);
	DWORD lNumChars = (DWORD)pFullMessage.length();
	DWORD lCharsWritten = 0;
	::WriteConsole(lStdOut, pFullMessage.c_str(), lNumChars, &lCharsWritten, NULL);

	// Restore normal text color.
	lAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	::SetConsoleTextAttribute(lStdOut, lAttributes);
#else // !LEPRA_WINDOWS
	::printf("%s", AnsiStringUtility::ToOwnCode(pFullMessage).c_str());
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
}



InteractiveConsoleLogListener::InteractiveConsoleLogListener(OutputFormat pFormat):
	LogListener(_T("console"), pFormat)
{
}

void InteractiveConsoleLogListener::SetAutoPrompt(const String& pAutoPrompt)
{
	ScopeLock lLock(&mLock);
	mAutoPrompt = pAutoPrompt;
}

void InteractiveConsoleLogListener::StepPage(int)
{
	// Default is a dummy, used in graphical applications.
}


InteractiveStdioConsoleLogListener::InteractiveStdioConsoleLogListener()
{
}

void InteractiveStdioConsoleLogListener::WriteLog(const String& pFullMessage, Log::LogLevel pLevel)
{
	ScopeLock lLock(&mLock);

#ifdef LEPRA_WINDOWS
	CONSOLE_SCREEN_BUFFER_INFO lConsoleInfo;
	::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &lConsoleInfo);
#elif defined(LEPRA_POSIX)
	::printf("\033[s");	// Store position.
#else // !known
#error "Cursor stuff not implemented for platform."
#endif // Windows / POSIX / ?

	::printf("\r");
	mStdioLogListener.WriteLog(pFullMessage, pLevel);

	::printf("%s", AnsiStringUtility::ToOwnCode(mAutoPrompt).c_str());

#ifdef LEPRA_WINDOWS
	SHORT x = lConsoleInfo.dwCursorPosition.X;
	::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &lConsoleInfo);
	lConsoleInfo.dwCursorPosition.X = x;
	::SetConsoleCursorPosition(::GetStdHandle(STD_OUTPUT_HANDLE), lConsoleInfo.dwCursorPosition);
#elif defined(LEPRA_POSIX)
	::printf("\033[u");	// Pop position.
	for (size_t x = 0; x < pFullMessage.length(); ++x)
	{
		if (pFullMessage[x] == _T('\n'))
		{
			::printf("\033[B");	// Move down.
		}
	}
	::fflush(stdout);
#else // !known
#error "Cursor stuff not implemented for platform."
#endif // Windows / Posix / ?
}

void InteractiveStdioConsoleLogListener::OnLogRawMessage(const String& pText)
{
	::printf("%s", AnsiStringUtility::ToOwnCode(pText).c_str());
}



DebuggerLogListener::DebuggerLogListener(OutputFormat pFormat):
	LogListener(_T("debug"), pFormat)
{
}

void DebuggerLogListener::WriteLog(const String& pFullMessage, Log::LogLevel)
{
#if !defined(NO_LOG_DEBUG_INFO)
#if defined(LEPRA_WINDOWS)
	OutputDebugString((_T(">>>")+pFullMessage).c_str());
#else // !Windows
	// Usually "console" is equivalent to "debug console" on other systems.
#endif // Windows/!Windows
#else // Logging debug info
	pFullMessage;
#endif // Not logging debug info / logging debug info.
}



FileLogListener::FileLogListener(const String& pFilename, OutputFormat pFormat):
	LogListener(_T("file"), pFormat)
{
	mFile.Open(pFilename, DiskFile::MODE_TEXT_WRITE_APPEND);
}

FileLogListener::~FileLogListener()
{
	mFile.Close();
}

File& FileLogListener::GetFile()
{
	return (mFile);
}

void FileLogListener::WriteLog(const String& pFullMessage, Log::LogLevel)
{
	mFile.WriteData(pFullMessage.c_str(), pFullMessage.length()*sizeof(tchar));
	mFile.Flush();
}



MemFileLogListener::MemFileLogListener(uint64 pMaxSize, OutputFormat pFormat):
	LogListener(_T("memory"), pFormat),
	mFile(),
	mMaxSize(pMaxSize)
{
}

MemFileLogListener::~MemFileLogListener()
{
	mFile.Close();
}

void MemFileLogListener::Clear()
{
	mFile.Clear();
}

bool MemFileLogListener::Dump(const String& pFilename)
{
	DiskFile lFile;
	bool lOk = lFile.Open(pFilename, DiskFile::MODE_WRITE_APPEND);
	if (lOk)
	{
		lOk = Dump(lFile);
	}
	return (lOk);
}

bool MemFileLogListener::Dump(File& pFile)
{
	return (Dump(&pFile, 0, Log::LEVEL_LOWEST_TYPE));
}

bool MemFileLogListener::Dump(LogListener& pLogListener, Log::LogLevel pLevel)
{
	return (Dump(0, &pLogListener, pLevel));
}

bool MemFileLogListener::Dump(File* pFile, LogListener* pLogListener, Log::LogLevel pLevel)
{
	IOError lStatus = IO_OK;
	Lepra::String lLine;
	int64 lPosition = mFile.Tell();
	mFile.SeekSet(0);
	while (lStatus == IO_OK)
	{
		lStatus = mFile.ReadLine(lLine);
		if (lStatus == IO_OK || lStatus == IO_BUFFER_UNDERFLOW)
		{
			if (pFile)
			{
				IOError lWriteStatus = pFile->WriteString(_T("  >>  ")+lLine+_T("\n"));
				if (lWriteStatus != IO_OK)
				{
					lStatus = (lWriteStatus == IO_BUFFER_UNDERFLOW)? IO_ERROR_WRITING_TO_STREAM : lWriteStatus;
				}
			}
			if (pLogListener)
			{
				pLogListener->WriteLog(_T("  >>  ")+lLine+_T("\n"), pLevel);
			}
		}
	}
	mFile.SeekSet(lPosition);
	if (pFile)
	{
		pFile->Flush();
	}
	return (lStatus == IO_OK || lStatus == IO_BUFFER_UNDERFLOW);
}

void MemFileLogListener::WriteLog(const String& pFullMessage, Log::LogLevel)
{
	mFile.WriteData(pFullMessage.c_str(), pFullMessage.length()*sizeof(tchar));

#ifndef NO_ASSERT_ON_LOG_FLOOD
	static size_t lSessionLogSize = 0;
	lSessionLogSize += pFullMessage.length();
	assert(lSessionLogSize < 4*1024*1024);	// TODO: something smarter using time as well (checking rate, opposed to size).
#endif // NO_ASSERT_ON_LOG_FLOOD

	if ((uint64)mFile.GetSize() > mMaxSize)
	{
		mFile.CropHead((unsigned)(mMaxSize*0.8));
	}
}



}
