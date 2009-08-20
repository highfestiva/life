
// Author: Jonas Bystr�m
// Copyright (c) 2002-2008, Righteous Games

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

	LogListener(Lepra::String pName, OutputFormat pFormat = FORMAT_THREADEX);
	virtual ~LogListener();
	void AddLog(Log* pLog);
	void RemoveLog(Log* pLog);
	void OnLog(const Log* pOriginator, const String& pAccount, const String& pMessage, Log::LogLevel pLevel);
	void OnLog(const Log* pOriginator, const String& pMessage, Log::LogLevel pLevel);
	virtual void WriteLog(const String& pMessage, Log::LogLevel pLevel) = 0;
	Log::LogLevel GetLevelThreashold() const;
	void SetLevelThreashold(Log::LogLevel pType);

	const Lepra::String& GetName() const;

protected:
	Log* mLog;
	Lepra::String mName;
	Log::LogLevel mLevel;
	OutputFormat mFormat;
	int mLogCount;
};


class StdioConsoleLogListener: public LogListener
{
public:
	StdioConsoleLogListener(OutputFormat pFormat = FORMAT_THREAD_CLASS);
	void WriteLog(const String& pFullMessage, Log::LogLevel pLevel);
};



// Log listener for interactive consoles.
class InteractiveConsoleLogListener: public LogListener
{
public:
	InteractiveConsoleLogListener(OutputFormat pFormat = FORMAT_THREAD_CLASS);
	void SetAutoPrompt(const String& pPrompt);
	virtual void StepPage(int pPageCount);
	virtual void OnLogRawMessage(const String& pText) = 0;

protected:
	Lock mLock;
	String mAutoPrompt;
};

class InteractiveStdioConsoleLogListener: public InteractiveConsoleLogListener
{
public:
	InteractiveStdioConsoleLogListener();

protected:
	void WriteLog(const String& pFullMessage, Log::LogLevel pLevel);
	void OnLogRawMessage(const String& pText);

	StdioConsoleLogListener mStdioLogListener;
};



class DebuggerLogListener: public LogListener
{
public:
	DebuggerLogListener(OutputFormat pFormat = FORMAT_THREAD_CLASS);

protected:
	void WriteLog(const String& pFullMessage, Log::LogLevel pLevel);
};



class FileLogListener: public LogListener
{
public:
	FileLogListener(const String& pFilename, OutputFormat pFormat = FORMAT_THREADEX);
	~FileLogListener();

	File& GetFile();

	void WriteLog(const String& pFullMessage, Log::LogLevel pLevel);

protected:
	DiskFile mFile;
};



class MemFileLogListener: public LogListener
{
public:
	MemFileLogListener(uint64 pMaxSize, OutputFormat pFormat = FORMAT_THREADEX);
	~MemFileLogListener();

	void Clear();
	bool Dump(const String& pFilename);
	bool Dump(File& pFile);
	bool Dump(LogListener& pLogListener, Log::LogLevel pLevel);

protected:
	bool Dump(File* pFile, LogListener* pLogListener, Log::LogLevel pLevel);

	void WriteLog(const String& pFullMessage, Log::LogLevel pLevel);

	MemFile mFile;
	uint64 mMaxSize;
};



}
