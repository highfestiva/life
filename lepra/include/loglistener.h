
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// An abstract callback class (Observer) used receive log events.
// The derived, implementing class may write to console, file
// or some graphic output of choice.

// Some commonly used implementations are declared here.



#pragma once

#include "diskfile.h"
#include "logger.h"
#include "memfile.h"



namespace lepra {



class LogListener {
public:
	enum OutputFormat {
		kFormatPlain		= 0,
		kFormatSubsystem	= (1 << 0),
		kFormatClass		= (1 << 1),
		kFormatTime		= (1 << 2),
		kFormatType		= (1 << 3),
		kFormatLogcount		= (1 << 4),
		kFormatThread		= (1 << 5),
		kFormatThreadClass	= (kFormatClass|kFormatThread),
		kFormatClassTime	= (kFormatClass|kFormatTime),
		kFormatThreadex		= (kFormatThreadClass|kFormatType|kFormatTime),
	};

	LogListener(str name, OutputFormat format = kFormatThreadex);
	virtual ~LogListener();
	void KillSelf();
	void AddLog(Logger* log);
	void RemoveLog(Logger* log);
	void OnLog(const Logger* originator, const str& account, const str& message, LogLevel level);
	void OnLog(const Logger* originator, const str& message, LogLevel level);
	virtual void WriteLog(const str& message, LogLevel level) = 0;
	LogLevel GetLevelThreashold() const;
	void SetLevelThreashold(LogLevel type);

	const str& GetName() const;

protected:
	Logger* log_;
	str name_;
	LogLevel level_;
	OutputFormat format_;
	int log_count_;
};


class StdioConsoleLogListener: public LogListener {
public:
	StdioConsoleLogListener(OutputFormat format = kFormatThreadClass);
	virtual ~StdioConsoleLogListener();
	void WriteLog(const str& full_message, LogLevel level);
};



// Logger listener for interactive consoles.
class InteractiveConsoleLogListener: public LogListener {
public:
	InteractiveConsoleLogListener(OutputFormat format = kFormatThreadClass);
	void SetAutoPrompt(const str& prompt);
	virtual void StepPage(int page_count);
	virtual void OnLogRawMessage(const str& text) = 0;

protected:
	Lock lock_;
	str auto_prompt_;
};

class InteractiveStdioConsoleLogListener: public InteractiveConsoleLogListener {
public:
	InteractiveStdioConsoleLogListener();
	virtual ~InteractiveStdioConsoleLogListener();

protected:
	void WriteLog(const str& full_message, LogLevel level);
	void OnLogRawMessage(const str& text);

	StdioConsoleLogListener stdio_log_listener_;
};



class DebuggerLogListener: public LogListener {
public:
	DebuggerLogListener(OutputFormat format = kFormatThreadClass);
	virtual ~DebuggerLogListener();

protected:
	void WriteLog(const str& full_message, LogLevel level);
};



class FileLogListener: public LogListener {
public:
	FileLogListener(const str& filename, OutputFormat format = kFormatThreadex);
	~FileLogListener();

	File& GetFile();

	void WriteLog(const str& full_message, LogLevel level);

protected:
	DiskFile file_;
};



class MemFileLogListener: public LogListener {
public:
	MemFileLogListener(uint64 max_size, OutputFormat format = kFormatThreadex);
	~MemFileLogListener();

	void Clear();
	bool Dump(const str& filename);
	bool Dump(File& file);
	bool Dump(LogListener& log_listener, LogLevel level);

protected:
	bool Dump(File* file, LogListener* log_listener, LogLevel level);

	void WriteLog(const str& full_message, LogLevel level);

	MemFile file_;
	uint64 max_size_;
};



}
