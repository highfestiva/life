
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

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



class Logger
{
public:
	Logger(const str& pName, Logger* pParent, LogLevel pLevel = LEVEL_INFO);
	virtual ~Logger();
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
	void DoPrint(const Logger* pOriginator, const str& pAccount, const str& pMessage, LogLevel pLevel = LEVEL_INFO);
	void DoRawPrint(const str& pMessage, LogLevel pLevel = LEVEL_INFO);

	str mName;
	Logger* mParent;
	std::vector<LogListener*> mLoggerList[LEVEL_TYPE_COUNT];
	SpinLock* mLoggerListLock;
	LogLevel mLevel;
};



}
