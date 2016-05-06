
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/log.h"
#include "../include/logger.h"



namespace lepra {



LogDecorator::LogDecorator(Logger* logger, const std::type_info& type_id):
	logger_(logger)
#ifdef LEPRA_MSVC
	// Skip "class " in beginning of name.
	, class_name_(str(type_id.name(+6))) {
#elif defined(LEPRA_POSIX)
{
	// Parse up to class name.
	// The format looks something like "1w5Lepra14TestBajjaMajjaE", thus (((BCD length)(data))...).
	// If in global namespace, the namespace (len)(data) pair is left out.
	const char* s = type_id.name();
	const size_t length = ::strlen(s);
	int word_count = 0;
	int target_word_count = 1;
	size_t x = 0;
	size_t start_of_word = 0;
	size_t step_length = 1;
	for (; word_count < target_word_count && x < length && step_length > 0; ++word_count, x += step_length) {
		for (; x < length && ::isalpha(s[x]); ++x)
			;
		char* end_ptr;
		long word_length = strutil::StrToL(&s[x], &end_ptr, 10);
		step_length = end_ptr-&s[x];
		start_of_word = x+step_length;
		step_length = (step_length < 1)? 1 : step_length;
		step_length += word_length;
		if (word_count == 0 && x+step_length < length && ::isdigit(s[x+step_length])) {
			++target_word_count;
		}
	}
	if (x <= length) {
		str crop(&s[start_of_word], x-start_of_word);
		class_name_ = crop;
	} else {
		class_name_ = s;
	}
#else // !MSVC
#error typeid parsing not implemented.
#endif // MSVC/!MSVC
}

void LogDecorator::Print(LogLevel log_level, const str& text) const {
	logger_->Print(class_name_, text, log_level);
}

void LogDecorator::RawPrint(LogLevel log_level, const str& text) const {
	logger_->RawPrint(text, log_level);
}

#define StrV()	\
	va_list	arguments;	\
	va_start(arguments, text);	\
	str _text = strutil::VFormat(text, arguments);	\
	va_end(arguments)

#define StrVLog(level)	\
	StrV();	\
	logger_->Print(class_name_, _text, level)

#ifndef NO_LOG_DEBUG_INFO
void LogDecorator::Tracef(const char* text, ...) const {
	StrVLog(kLevelTrace);
}

void LogDecorator::Trace(const str& text) const {
	Print(kLevelTrace, text);
}

void LogDecorator::Debugf(const char* text, ...) const {
	StrVLog(kLevelDebug);
}

void LogDecorator::Debug(const str& text) const {
	Print(kLevelDebug, text);
}
#endif // !NO_LOG_DEBUG_INFO

void LogDecorator::Performancef(const char* text, ...) const {
	StrVLog(kLevelPerformance);
}

void LogDecorator::Performance(const str& text) const {
	Print(kLevelPerformance, text);
}

void LogDecorator::Infof(const char* text, ...) const {
	StrVLog(kLevelInfo);
}

void LogDecorator::Info(const str& text) const {
	Print(kLevelInfo, text);
}

void LogDecorator::Headlinef(const char* text, ...) const {
	StrVLog(kLevelHeadline);
}

void LogDecorator::Headline(const str& text) const {
	Print(kLevelHeadline, text);
}

void LogDecorator::Warningf(const char* text, ...) const {
	StrVLog(kLevelWarning);
}

void LogDecorator::Warning(const str& text) const {
	Print(kLevelWarning, text);
}

void LogDecorator::Errorf(const char* text, ...) const {
	StrVLog(kLevelError);
}

void LogDecorator::Error(const str& text) const {
	Print(kLevelError, text);
}

void LogDecorator::Fatalf(const char* text, ...) const {
	StrVLog(kLevelFatal);
}

void LogDecorator::Fatal(const str& text) const {
	Print(kLevelFatal, text);
}

const str& LogDecorator::GetClassName() const {
	return (class_name_);
}



}
