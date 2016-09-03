
// Author: Jonas Byström
// Copyright (c) Jonas Byström



#include "pch.h"
#include "../include/posix/maclog.h"
#include "../include/lepraassert.h"
#include "../include/lepraos.h"
#include "../include/loglistener.h"
#include "../include/time.h"



namespace lepra {



LogListener::LogListener(str name, OutputFormat format):
	log_(0),
	name_(name),
	level_(kLevelLowestType),
	format_(format),
	log_count_(1) {
}

LogListener::~LogListener() {
	KillSelf();	// TRICKY: has to be done in own destructor to avoid purecalls.
}

void LogListener::AddLog(Logger* log) {
	deb_assert(log_ == 0 || log_ == log);
	if (log_ == 0 || log_ == log) {
		log_ = log;
	}
}

void LogListener::RemoveLog(Logger* log) {
	deb_assert(log_ == log);
	if (log_ == log) {
		log_ = 0;
	}
}

void LogListener::KillSelf() {
	if (log_) {
		log_->RemoveListener(this);
		deb_assert(log_ == 0);
	}
}

void LogListener::OnLog(const Logger* originator, const str& account, const str& message, LogLevel level) {
	if (format_ & kFormatClass) {
		OnLog(originator, account+": "+message+"\n", level);
	} else {
		OnLog(originator, message+"\n", level);
	}
}

void LogListener::OnLog(const Logger* originator, const str& message, LogLevel level) {
	if (level < level_) {
		return;	// TRICKY: optimization by early return.
	}

	str output_string;

	if (format_ & kFormatLogcount) {
		output_string = strutil::Format("%.5i. ", log_count_);
	}
	++log_count_;

	if (format_ & kFormatType) {
		switch(level) {
			case kLevelTrace:	output_string += "TRACE:       ";	break;
			case kLevelDebug:	output_string += "DEBUG:       ";	break;
			case kLevelPerformance:	output_string += "PERFORMANCE: ";	break;
			case kLevelInfo:	output_string += "INFO:        ";	break;
			case kLevelHeadline:	output_string += "HEADLINE:    ";	break;
			case kLevelWarning:	output_string += "WARNING:     ";	break;
			case kLevelError:	output_string += "ERROR:       ";	break;
			default:	// Fall through.
			case kLevelFatal:		output_string += "FATAL:       ";	break;
		}
	}

	if (format_ & kFormatSubsystem) {
		output_string += originator->GetName();
	}

	if (format_ & kFormatTime) {
		static const str pre("(");
		static const str post(") ");
		output_string += pre + Time().GetDateTimeAsString() + post;
	}

	if (format_ & kFormatThread) {
		Thread* thread = Thread::GetCurrentThread();
		if (thread) {
			output_string += strutil::Format("%10s: ", thread->GetThreadName().c_str());
		}
	}

	output_string += message;

	WriteLog(output_string, level);
}

LogLevel LogListener::GetLevelThreashold() const {
	return (level_);
}

void LogListener::SetLevelThreashold(LogLevel level) {
	level_ = level;
	log_->UpdateLevelThreashold();
}

const str& LogListener::GetName() const {
	return (name_);
}



StdioConsoleLogListener::StdioConsoleLogListener(OutputFormat format):
	LogListener("console", format) {
}

StdioConsoleLogListener::~StdioConsoleLogListener() {
	KillSelf();	// TRICKY: has to be done in own destructor to avoid purecalls.
}

void StdioConsoleLogListener::WriteLog(const str& full_message, LogLevel level) {
#if defined(LEPRA_WINDOWS)
	HANDLE std_out = ::GetStdHandle(STD_OUTPUT_HANDLE);

	// Set console text color. Default color is light gray.
	WORD attributes;
	switch(level) {
		case kLevelTrace:	// Fall through.
		case kLevelDebug:	attributes = FOREGROUND_BLUE;									break;
		case kLevelInfo:	attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;				break;
		case kLevelPerformance:	attributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN;						break;
		case kLevelHeadline:	attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;	break;
		case kLevelWarning:	attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;				break;
		case kLevelError:	attributes = FOREGROUND_INTENSITY | FOREGROUND_RED;						break;
		default:	// Fall through.
		case kLevelFatal:	attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_INTENSITY | BACKGROUND_RED;	break;
	}
	::SetConsoleTextAttribute(std_out, attributes);
	DWORD chars_written = 0;
#ifdef LEPRA_UTF32
	::WriteConsoleW(std_out, full_message.c_str(), (DWORD)full_message.length(), &chars_written, NULL);
#else
	const wstr w = wstrutil::Encode(full_message);
	::WriteConsoleW(std_out, w.c_str(), (DWORD)w.length(), &chars_written, NULL);
#endif // UTF-16/UTF-8

	// Restore normal text color.
	attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	::SetConsoleTextAttribute(std_out, attributes);
#else // !LEPRA_WINDOWS
	::printf("%s", full_message.c_str());
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
}



InteractiveConsoleLogListener::InteractiveConsoleLogListener(OutputFormat format):
	LogListener("i-console", format) {
}

void InteractiveConsoleLogListener::SetAutoPrompt(const str& auto_prompt) {
	ScopeLock lock(&lock_);
	auto_prompt_ = auto_prompt;
}

void InteractiveConsoleLogListener::StepPage(int) {
	// Default is a dummy, used in graphical applications.
}


InteractiveStdioConsoleLogListener::InteractiveStdioConsoleLogListener() {
}

InteractiveStdioConsoleLogListener::~InteractiveStdioConsoleLogListener() {
	KillSelf();	// TRICKY: has to be done in own destructor to avoid purecalls.
}

void InteractiveStdioConsoleLogListener::WriteLog(const str& full_message, LogLevel level) {
	ScopeLock lock(&lock_);

#ifdef LEPRA_WINDOWS
	CONSOLE_SCREEN_BUFFER_INFO console_info;
	::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &console_info);
#elif defined(LEPRA_POSIX)
	::printf("\033[s");	// Store position.
#else // !known
#error "Cursor stuff not implemented for platform."
#endif // Windows / POSIX / ?

	::printf("\r");
	stdio_log_listener_.WriteLog(full_message, level);

	::printf("%s", auto_prompt_.c_str());

#ifdef LEPRA_WINDOWS
	SHORT x = console_info.dwCursorPosition.X;
	::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &console_info);
	console_info.dwCursorPosition.X = x;
	::SetConsoleCursorPosition(::GetStdHandle(STD_OUTPUT_HANDLE), console_info.dwCursorPosition);
#elif defined(LEPRA_POSIX)
	::printf("\033[u");	// Pop position.
	for (size_t x = 0; x < full_message.length(); ++x) {
		if (full_message[x] == '\n') {
			::printf("\033[B");	// Move down.
		}
	}
	::fflush(stdout);
#else // !known
#error "Cursor stuff not implemented for platform."
#endif // Windows / Posix / ?
}

void InteractiveStdioConsoleLogListener::OnLogRawMessage(const str& text) {
	::printf("%s", text.c_str());
}



DebuggerLogListener::DebuggerLogListener(OutputFormat format):
	LogListener("debug", format) {
}

DebuggerLogListener::~DebuggerLogListener() {
	KillSelf();	// TRICKY: has to be done in own destructor to avoid purecalls.
}

void DebuggerLogListener::WriteLog(const str& full_message, LogLevel) {
#if !defined(NO_LOG_DEBUG_INFO)
#if defined(LEPRA_WINDOWS)
#ifdef LEPRA_UTF32
	OutputDebugStringW((L">>>"+full_message).c_str());
#else
	const wstr w = wstrutil::Encode(">>>"+full_message);
	OutputDebugStringW(w.c_str());
#endif // UTF-16/UTF-8
#elif defined(LEPRA_MAC)
	MacLog::Write(">>>"+full_message);
#else // !Windows
	// Usually "console" is equivalent to "debug console" on other systems.
#endif // Windows/!Windows
#else // Logging debug info
	full_message;
#endif // Not logging debug info / logging debug info.
}



FileLogListener::FileLogListener(const str& filename, OutputFormat format):
	LogListener("file", format) {
	file_.Open(filename, DiskFile::kModeTextWriteAppend);
	if (file_.GetSize() > 3*1024*1024) {	// If the log starts getting big, do something about it.
		file_.Open(filename, DiskFile::kModeTextWrite);
	}
}

FileLogListener::~FileLogListener() {
	KillSelf();	// TRICKY: has to be done in own destructor to avoid purecalls.
	file_.Close();
}

File& FileLogListener::GetFile() {
	return (file_);
}

void FileLogListener::WriteLog(const str& full_message, LogLevel) {
	file_.WriteString(full_message);
	file_.Flush();
}



MemFileLogListener::MemFileLogListener(uint64 max_size, OutputFormat format):
	LogListener("memory", format),
	file_(),
	max_size_(max_size) {
}

MemFileLogListener::~MemFileLogListener() {
	KillSelf();	// TRICKY: has to be done in own destructor to avoid purecalls.
	file_.Close();
}

void MemFileLogListener::Clear() {
	file_.Clear();
}

bool MemFileLogListener::Dump(const str& filename) {
	DiskFile _file;
	bool ok = _file.Open(filename, DiskFile::kModeWriteAppend);
	if (ok) {
		ok = Dump(_file);
	}
	return (ok);
}

bool MemFileLogListener::Dump(File& file) {
	return (Dump(&file, 0, kLevelLowestType));
}

bool MemFileLogListener::Dump(LogListener& log_listener, LogLevel level) {
	return (Dump(0, &log_listener, level));
}

bool MemFileLogListener::Dump(File* file, LogListener* log_listener, LogLevel level) {
	IOError status = kIoOk;
	str line;
	int64 position = file_.Tell();
	file_.SeekSet(0);
	while (status == kIoOk) {
		status = file_.ReadLine(line);
		if (status == kIoOk || status == kIoBufferUnderflow) {
			if (file) {
				IOError write_status = file->WriteString("  >>  "+line+"\n");
				if (write_status != kIoOk) {
					status = (write_status == kIoBufferUnderflow)? kIoErrorWritingToStream : write_status;
				}
			}
			if (log_listener) {
				log_listener->WriteLog("  >>  "+line+"\n", level);
			}
		}
	}
	file_.SeekSet(position);
	if (file) {
		file->Flush();
	}
	return (status == kIoOk || status == kIoBufferUnderflow);
}

void MemFileLogListener::WriteLog(const str& full_message, LogLevel log_level) {
	file_.WriteString(full_message);

#ifndef NO_ASSERT_ON_LOG_FLOOD
	if (log_level >= kLevelInfo) {
		static size_t session_log_size = 0;
		session_log_size += full_message.length();
		deb_assert(session_log_size < 4*1024*1024);	// TODO: something smarter using time as well (checking rate, opposed to size).
	}
#endif // NO_ASSERT_ON_LOG_FLOOD

	if ((uint64)file_.GetSize() > max_size_) {
		file_.CropHead((unsigned)(max_size_*0.8));
	}
}



}
