
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games

// Use LogDecorator and pass typeid(class) as constructor parameter.



#pragma once

#include <typeinfo>
#include <vector>
#include "LepraTypes.h"
#include "LogLevel.h"
#include "LogType.h"
#include "String.h"



namespace Lepra
{



class LogListener;
class SpinLock;



class Log
{
public:
	Log(const str& pName, Log* pParent, LogLevel pLevel = LEVEL_INFO);
	virtual ~Log();
	void SetupBasicListeners(LogListener* pConsole = 0, LogListener* pDebug = 0,
		LogListener* pFile = 0, LogListener* pPerformance = 0, LogListener* pMem = 0);
	void AddListener(LogListener* pLogger, LogLevel pLevel);
	void RemoveListener(LogListener* pLogger);
	LogListener* GetListener(const str& pName) const;
	const str& GetName() const;
	LogLevel GetLevelThreashold() const;
	void SetLevelThreashold(LogLevel pLevel);
	void Print(const str& pAccount, const str& pMessage, LogLevel pLevel = LEVEL_INFO);
	void RawPrint(const str& pMessage, LogLevel pLevel = LEVEL_INFO);

private:
	void DoPrint(const Log* pOriginator, const str& pAccount, const str& pMessage, LogLevel pLevel = LEVEL_INFO);
	void DoRawPrint(const str& pMessage, LogLevel pLevel = LEVEL_INFO);

	str mName;
	Log* mParent;
	std::vector<LogListener*> mLoggerList[LEVEL_TYPE_COUNT];
	SpinLock* mLoggerListLock;
	LogLevel mLevel;
};



class LogDecorator
{
public:
	LogDecorator(Log* pLog, const std::type_info& pTypeId);

	void Print(LogLevel pLogLevel, const str& pText) const;
	void RawPrint(LogLevel pLogLevel, const str& pText) const;
#ifndef NO_LOG_DEBUG_INFO
	void Tracef(const tchar* pText, ...) const;
	void Trace(const str& pText) const;
	void Debugf(const tchar* pText, ...) const;
	void Debug(const str& pText) const;
#endif // !NO_LOG_DEBUG_INFO
	void Performancef(const tchar* pText, ...) const;
	void Performance(const str& pText) const;
	void Infof(const tchar* pText, ...) const;
	void Info(const str& pText) const;
	void Headlinef(const tchar* pText, ...) const;
	void Headline(const str& pText) const;
	void Warningf(const tchar* pText, ...) const;
	void Warning(const str& pText) const;
	void Errorf(const tchar* pText, ...) const;
	void Error(const str& pText) const;
	void Fatalf(const tchar* pText, ...) const;
	void Fatal(const str& pText) const;

	const str& GetClassName() const;

private:
	Log* mLog;
	str mClassName;
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
