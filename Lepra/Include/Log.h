
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games

// Use LogDecorator and pass typeid(class) as constructor parameter.



#pragma once

#include <typeinfo>
#include <vector>
#include "LepraTypes.h"
#include "LogType.h"
#include "String.h"



namespace Lepra
{



class LogListener;



class Log
{
public:
	enum LogLevel
	{
		LEVEL_LOWEST_TYPE = 0,
		LEVEL_TRACE = LEVEL_LOWEST_TYPE,	// Use for tracing where execution goes.
		LEVEL_DEBUG,				// Used for developer information messages. Will not be logged in the final version.
		LEVEL_PERFORMANCE,			// Use when printing performance data.
		LEVEL_INFO,				// Used for normal logging.
		LEVEL_HEADLINE,				// Used for important events.
		LEVEL_WARNING,				// Used for non critical errors.
		LEVEL_ERROR,				// Used for critical errors.
		LEVEL_FATAL,				// Even more critial, when it's time to panic or something... :)
		LEVEL_TYPE_COUNT
	};

	Log(const String& pName, Log* pParent, LogLevel pLevel = LEVEL_LOWEST_TYPE);
	void SetupBasicListeners(LogListener* pConsole = 0, LogListener* pDebug = 0,
		LogListener* pFile = 0, LogListener* pPerformance = 0, LogListener* pMem = 0);
	void AddListener(LogListener* pLogger, LogLevel pLevel);
	void RemoveListener(LogListener* pLogger);
	LogListener* GetListener(const Lepra::String& pName) const;
	String GetName() const;
	LogLevel GetLevelThreashold() const;
	void SetLevelThreashold(LogLevel pLevel);
	void Print(const String& pAccount, const String& pMessage, LogLevel pLevel = LEVEL_INFO);
	void RawPrint(const String& pMessage, LogLevel pLevel = LEVEL_INFO);

private:
	void DoPrint(const Log* pOriginator, const String& pAccount, const String& pMessage, LogLevel pLevel = LEVEL_INFO);
	void DoRawPrint(const String& pMessage, LogLevel pLevel = LEVEL_INFO);

	String mName;
	Log* mParent;
	std::vector<LogListener*> mLoggerList[LEVEL_TYPE_COUNT];
	LogLevel mLevel;
};



class LogDecorator
{
public:
	LogDecorator(Log* pLog, const std::type_info& pTypeId);

	void Print(Log::LogLevel pLogLevel, const String& pText) const;
	void RawPrint(Log::LogLevel pLogLevel, const String& pText) const;
#ifndef NO_LOG_DEBUG_INFO
	void Tracef(const tchar* pText, ...) const;
	void Trace(const String& pText) const;
	void Debugf(const tchar* pText, ...) const;
	void Debug(const String& pText) const;
#endif // !NO_LOG_DEBUG_INFO
	void Performancef(const tchar* pText, ...) const;
	void Performance(const String& pText) const;
	void Infof(const tchar* pText, ...) const;
	void Info(const String& pText) const;
	void Headlinef(const tchar* pText, ...) const;
	void Headline(const String& pText) const;
	void Warningf(const tchar* pText, ...) const;
	void Warning(const String& pText) const;
	void Errorf(const tchar* pText, ...) const;
	void Error(const String& pText) const;
	void Fatalf(const tchar* pText, ...) const;
	void Fatal(const String& pText) const;

	const String& GetClassName() const;

private:
	Log* mLog;
	String mClassName;
};

// Ansi comfort zone.
#define AInfo(what)		Info(_T(what))
#define AHeadline(what)		Headline(_T(what))
#define AWarning(what)		Warning(_T(what))
#define AError(what)		Error(_T(what))
#define AFatal(what)		Fatal(_T(what))

#ifdef NO_LOG_DEBUG_INFO
#define log_volatile(x)
#else // !NO_LOG_DEBUG_INFO
#define log_volatile(x)		x
#endif // NO_LOG_DEBUG_INFO / !NO_LOG_DEBUG_INFO
#define log_atrace(what)	log_volatile(mLog.Trace(_T(what)))
#define log_adebug(what)	log_volatile(mLog.Debug(_T(what)))
#define log_aperformance(what)	log_volatile(mLog.Performance(_T(what)))
#define log_trace(what)		log_volatile(mLog.Trace(what))
#define log_debug(what)		log_volatile(mLog.Debug(what))
#define log_performance(what)	log_volatile(mLog.Performance(what))

#define LOG_CLASS_DECLARE_NAME(name)		private: static Lepra::LogDecorator name
#define LOG_CLASS_DECLARE()			LOG_CLASS_DECLARE_NAME(mLog)
#define LOG_CLASS_DEFINE_NAME(Class, to, name)	Lepra::LogDecorator Class::name(to, typeid(Class))
#define LOG_CLASS_DEFINE_TO(Class, to)		LOG_CLASS_DEFINE_NAME(Class, to, mLog)
#define LOG_CLASS_DEFINE(subsystem, Class)	LOG_CLASS_DEFINE_TO(Class, Lepra::LogType::GetLog(Lepra::LogType::SUB_ ## subsystem))



}
