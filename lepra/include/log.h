
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Use LogDecorator and pass typeid(class) as constructor parameter.



#pragma once

#include "lepratypes.h"
#include "loglevel.h"
#include "logtype.h"
#include "string.h"
#include <typeinfo>


namespace lepra {



class Logger;



class LogDecorator {
public:
	LogDecorator(Logger* log, const std::type_info& type_id);

	void Print(LogLevel log_level, const str& text) const;
	void RawPrint(LogLevel log_level, const str& text) const;
#ifndef NO_LOG_DEBUG_INFO
	void Tracef(const char* text, ...) const;
	void Trace(const str& text) const;
	void Debugf(const char* text, ...) const;
	void Debug(const str& text) const;
#endif // !NO_LOG_DEBUG_INFO
	void Performancef(const char* text, ...) const;
	void Performance(const str& text) const;
	void Infof(const char* text, ...) const;
	void Info(const str& text) const;
	void Headlinef(const char* text, ...) const;
	void Headline(const str& text) const;
	void Warningf(const char* text, ...) const;
	void Warning(const str& text) const;
	void Errorf(const char* text, ...) const;
	void Error(const str& text) const;
	void Fatalf(const char* text, ...) const;
	void Fatal(const str& text) const;

	const str& GetClassName() const;

private:
	Logger* logger_;
	str class_name_;
};

#ifdef NO_LOG_DEBUG_INFO
#define log_volatile(x)
#else // !NO_LOG_DEBUG_INFO
#define log_volatile(x)		x
#endif // NO_LOG_DEBUG_INFO / !NO_LOG_DEBUG_INFO
#define log_trace(what)		log_volatile(log_.Trace(what))
#define log_debug(what)		log_volatile(log_.Debug(what))
#define log_performance(what)	log_volatile(log_.Performance(what))

#define LOG_CLASS_DECLARE_NAME(name)		private: static lepra::LogDecorator name
#define logclass()				LOG_CLASS_DECLARE_NAME(log_)
#define LOG_CLASS_DEFINE_NAME(Class, to, name)	lepra::LogDecorator Class::name(to, typeid(Class))
#define LOG_CLASS_DEFINE_TO(Class, to)		LOG_CLASS_DEFINE_NAME(Class, to, log_)
#define loginstance(subsystem, Class)		LOG_CLASS_DEFINE_TO(Class, lepra::LogType::GetLogger(lepra::LogType:: subsystem))



}
