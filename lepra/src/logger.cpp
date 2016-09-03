
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/lepraassert.h"
#include <iomanip>
#include <iostream>
#include <strstream>
#include "../include/logger.h"
#include "../include/loglistener.h"
#include "../include/spinlock.h"



namespace lepra {



Logger::Logger(const str& name, Logger* parent, LogLevel level):
	name_(name),
	parent_(parent),
	logger_list_lock_(new SpinLock),
	level_(level) {
}

Logger::~Logger() {
	delete logger_list_lock_;
}

void Logger::SetupBasicListeners(LogListener* console, LogListener* debug,
	LogListener* file, LogListener* performance, LogListener* mem) {
	if (console) {
		// Console logs everything user wants.
		for (int x = kLevelLowestType; x < kLevelTypeCount; ++x) {
			AddListener(console, (LogLevel)x);
		}
	}

	if (debug) {
#ifndef NO_LOG_DEBUG_INFO	// Only available in debug and release candidates.
		for (int x = kLevelLowestType; x < kLevelTypeCount; ++x) {
			AddListener(debug, (LogLevel)x);
		}
#endif // !NO_LOG_DEBUG_INFO
	}

	if (file) {
		// File logs everything but performance.
		AddListener(file, kLevelTrace);
		AddListener(file, kLevelDebug);
		AddListener(file, kLevelInfo);
		AddListener(file, kLevelHeadline);
		AddListener(file, kLevelWarning);
		AddListener(file, kLevelError);
		AddListener(file, kLevelFatal);
	}

	if (performance) {
		AddListener(performance, kLevelPerformance);
		AddListener(performance, kLevelHeadline);
	}

	if (mem) {
		for (int x = kLevelLowestType; x < kLevelTypeCount; ++x) {
			AddListener(mem, (LogLevel)x);
		}
	}
}

void Logger::AddListener(LogListener* logger, LogLevel level) {
	deb_assert(logger);
#ifdef NO_LEVEL_DEBUG_INFO	// Final (public release) version gets no debug messages.
	if (level >= kLevelInfo)
#endif // !NO_LEVEL_DEBUG_INFO
	{
		deb_assert(level >= kLevelLowestType && level < kLevelTypeCount);
		if (level >= kLevelLowestType && level < kLevelTypeCount) {
			ScopeSpinLock scope_lock(logger_list_lock_);
			logger_list_[level].push_back(logger);
			logger->AddLog(this);
			if (logger->GetLevelThreashold() < LogDecorator::GetLogLevel()) {
				LogDecorator::SetLogLevel(logger->GetLevelThreashold());
			}
		}
	}
}

void Logger::RemoveListener(LogListener* logger) {
	deb_assert(logger);
	{
		ScopeSpinLock scope_lock(logger_list_lock_);
		for (int x = kLevelLowestType; x < kLevelTypeCount; ++x) {
			std::vector<LogListener*>::iterator y = logger_list_[x].begin();
			for (; y != logger_list_[x].end(); ++y) {
				if (*y == logger) {
					logger_list_[x].erase(y);
					break;
				}
			}
		}
		logger->RemoveLog(this);
	}
	if (logger->GetLevelThreashold() == LogDecorator::GetLogLevel()) {
		UpdateLevelThreashold();
	}
}

LogListener* Logger::GetListener(const str& name) const {
	LogListener* listener = 0;
	ScopeSpinLock scope_lock(logger_list_lock_);
	for (int x = kLevelLowestType; !listener && x < kLevelTypeCount; ++x) {
		std::vector<LogListener*>::const_iterator y = logger_list_[x].begin();
		for (; !listener && y != logger_list_[x].end(); ++y) {
			if ((*y)->GetName() == name) {
				listener = *y;
			}
		}
	}
	return (listener);
}

const str& Logger::GetName() const {
	return (name_);
}

LogLevel Logger::GetLevelThreashold()const {
	return (level_);
}

void Logger::SetLevelThreashold(LogLevel level) {
	if (level < kLevelLowestType) {
		level = kLevelLowestType;
	} else if (level >= kLevelTypeCount) {
		level = (LogLevel)(kLevelTypeCount-1);
	}
	if (level != level_) {
		level_ = level;
		UpdateLevelThreashold();
	}
}

void Logger::UpdateLevelThreashold() {
	LogLevel level = level_;
	ScopeSpinLock scope_lock(logger_list_lock_);
	for (int x = kLevelLowestType; x < kLevelTypeCount; ++x) {
		std::vector<LogListener*>::const_iterator y = logger_list_[x].begin();
		for (; y != logger_list_[x].end(); ++y) {
			LogLevel listener_level = (*y)->GetLevelThreashold();
			level = (level < listener_level) ? level : listener_level;
		}
	}
	LogDecorator::SetLogLevel(level);
}

void Logger::Print(const str& account, const str& message, LogLevel level) {
	if (level >= level_ && level < kLevelTypeCount) {
		DoPrint(this, account, message, level);
	}
}

void Logger::RawPrint(const str& message, LogLevel level) {
	if (level >= level_ && level < kLevelTypeCount) {
		DoRawPrint(message, level);
	}
}

void Logger::DoPrint(const Logger* originator, const str& account, const str& message, LogLevel level) {
	if (parent_) {
		parent_->DoPrint(this, account, message, level);
	}

	ScopeSpinLock scope_lock(logger_list_lock_);
	std::vector<LogListener*>::iterator x = logger_list_[level].begin();
	for (; x != logger_list_[level].end(); ++x) {
		(*x)->OnLog(originator, account, message, level);
	}
}

void Logger::DoRawPrint(const str& message, LogLevel level) {
	if (parent_) {
		parent_->DoRawPrint(message, level);
	}

	ScopeSpinLock scope_lock(logger_list_lock_);
	std::vector<LogListener*>::iterator x = logger_list_[level].begin();
	for (; x != logger_list_[level].end(); ++x) {
		(*x)->WriteLog(message, level);
	}
}



}
