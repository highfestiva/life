
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Use LogDecorator and pass typeid(class) as constructor parameter.



#pragma once

#include "LepraTypes.h"
#include "LogLevel.h"
#include "LogType.h"
#include "String.h"
#include <typeinfo>


namespace Lepra
{



class Logger;



class LogDecorator
{
public:
	LogDecorator(Logger* pLog, const std::type_info& pTypeId);

	void Print(LogLevel pLogLevel, const str& pText) const;
	void RawPrint(LogLevel pLogLevel, const str& pText) const;
#ifndef NO_LOG_DEBUG_INFO
	void Tracef(const char* pText, ...) const;
	void Trace(const str& pText) const;
	void Debugf(const char* pText, ...) const;
	void Debug(const str& pText) const;
#endif // !NO_LOG_DEBUG_INFO
	void Performancef(const char* pText, ...) const;
	void Performance(const str& pText) const;
	void Infof(const char* pText, ...) const;
	void Info(const str& pText) const;
	void Headlinef(const char* pText, ...) const;
	void Headline(const str& pText) const;
	void Warningf(const char* pText, ...) const;
	void Warning(const str& pText) const;
	void Errorf(const char* pText, ...) const;
	void Error(const str& pText) const;
	void Fatalf(const char* pText, ...) const;
	void Fatal(const str& pText) const;

	const str& GetClassName() const;

private:
	Logger* mLogger;
	str mClassName;
};

#ifdef NO_LOG_DEBUG_INFO
#define log_volatile(x)
#else // !NO_LOG_DEBUG_INFO
#define log_volatile(x)		x
#endif // NO_LOG_DEBUG_INFO / !NO_LOG_DEBUG_INFO
#define log_trace(what)		log_volatile(mLog.Trace(what))
#define log_debug(what)		log_volatile(mLog.Debug(what))
#define log_performance(what)	log_volatile(mLog.Performance(what))

#define LOG_CLASS_DECLARE_NAME(name)		private: static Lepra::LogDecorator name
#define logclass()				LOG_CLASS_DECLARE_NAME(mLog)
#define LOG_CLASS_DEFINE_NAME(Class, to, name)	Lepra::LogDecorator Class::name(to, typeid(Class))
#define LOG_CLASS_DEFINE_TO(Class, to)		LOG_CLASS_DEFINE_NAME(Class, to, mLog)
#define loginstance(subsystem, Class)		LOG_CLASS_DEFINE_TO(Class, Lepra::LogType::GetLogger(Lepra::LogType:: subsystem))



}
