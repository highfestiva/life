
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

// An abstract callback class (Observer) used receive log events.
// The derived, implementing class may write to console, file
// or some graphic output of choice.

// Some commonly used implementations are declared here.



#pragma once

#include "DiskFile.h"
#include "Log.h"
#include "MemFile.h"



namespace Lepra
{



class LogListener
{
public:
	enum OutputFormat
	{
		FORMAT_PLAIN		= 0,
		FORMAT_SUBSYSTEM	= (1 << 0),
		FORMAT_CLASS		= (1 << 1),
		FORMAT_TIME		= (1 << 2),
		FORMAT_TYPE		= (1 << 3),
		FORMAT_LOGCOUNT		= (1 << 4),
		FORMAT_THREAD		= (1 << 5),
		FORMAT_THREAD_CLASS	= (FORMAT_CLASS|FORMAT_THREAD),
		FORMAT_CLASS_TIME	= (FORMAT_CLASS|FORMAT_TIME),
		FORMAT_THREADEX		= (FORMAT_THREAD_CLASS|FORMAT_TYPE|FORMAT_TIME),
	};

	LogListener(str pName, OutputFormat pFormat = FORMAT_THREADEX);
	virtual ~LogListener();
	void AddLog(Log* pLog);
	void RemoveLog(Log* pLog);
	void OnLog(const Log* pOriginator, const str& pAccount, const str& pMessage, Log::LogLevel pLevel);
	void OnLog(const Log* pOriginator, const str& pMessage, Log::LogLevel pLevel);
	virtual void WriteLog(const str& pMessage, Log::LogLevel pLevel) = 0;
	Log::LogLevel GetLevelThreashold() const;
	void SetLevelThreashold(Log::LogLevel pType);

	const str& GetName() const;

protected:
	Log* mLog;
	str mName;
	Log::LogLevel mLevel;
	OutputFormat mFormat;
	int mLogCount;
};


class StdioConsoleLogListener: public LogListener
{
public:
	StdioConsoleLogListener(OutputFormat pFormat = FORMAT_THREAD_CLASS);
	void WriteLog(const str& pFullMessage, Log::LogLevel pLevel);
};



// Log listener for interactive consoles.
class InteractiveConsoleLogListener: public LogListener
{
public:
	InteractiveConsoleLogListener(OutputFormat pFormat = FORMAT_THREAD_CLASS);
	void SetAutoPrompt(const str& pPrompt);
	virtual void StepPage(int pPageCount);
	virtual void OnLogRawMessage(const str& pText) = 0;

protected:
	Lock mLock;
	str mAutoPrompt;
};

class InteractiveStdioConsoleLogListener: public InteractiveConsoleLogListener
{
public:
	InteractiveStdioConsoleLogListener();

protected:
	void WriteLog(const str& pFullMessage, Log::LogLevel pLevel);
	void OnLogRawMessage(const str& pText);

	StdioConsoleLogListener mStdioLogListener;
};



class DebuggerLogListener: public LogListener
{
public:
	DebuggerLogListener(OutputFormat pFormat = FORMAT_THREAD_CLASS);

protected:
	void WriteLog(const str& pFullMessage, Log::LogLevel pLevel);
};



class FileLogListener: public LogListener
{
public:
	FileLogListener(const str& pFilename, OutputFormat pFormat = FORMAT_THREADEX);
	~FileLogListener();

	File& GetFile();

	void WriteLog(const str& pFullMessage, Log::LogLevel pLevel);

protected:
	DiskFile mFile;
};



class MemFileLogListener: public LogListener
{
public:
	MemFileLogListener(uint64 pMaxSize, OutputFormat pFormat = FORMAT_THREADEX);
	~MemFileLogListener();

	void Clear();
	bool Dump(const str& pFilename);
	bool Dump(File& pFile);
	bool Dump(LogListener& pLogListener, Log::LogLevel pLevel);

protected:
	bool Dump(File* pFile, LogListener* pLogListener, Log::LogLevel pLevel);

	void WriteLog(const str& pFullMessage, Log::LogLevel pLevel);

	MemFile mFile;
	uint64 mMaxSize;
};



}
