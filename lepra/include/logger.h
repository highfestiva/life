
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Use LogDecorator and pass typeid(class) as constructor parameter.



#pragma once

#include <typeinfo>
#include <vector>
#include "lepratypes.h"
#include "loglevel.h"
#include "logtype.h"
#include "string.h"



namespace lepra {



class LogListener;
class SpinLock;



class Logger {
public:
	Logger(const str& name, Logger* parent, LogLevel level = kLevelInfo);
	virtual ~Logger();
	void SetupBasicListeners(LogListener* console = 0, LogListener* debug = 0,
		LogListener* file = 0, LogListener* performance = 0, LogListener* mem = 0);
	void AddListener(LogListener* logger, LogLevel level);
	void RemoveListener(LogListener* logger);
	LogListener* GetListener(const str& name) const;
	const str& GetName() const;
	LogLevel GetLevelThreashold() const;
	void SetLevelThreashold(LogLevel level);
	void Print(const str& account, const str& message, LogLevel level = kLevelInfo);
	void RawPrint(const str& message, LogLevel level = kLevelInfo);

private:
	void DoPrint(const Logger* originator, const str& account, const str& message, LogLevel level = kLevelInfo);
	void DoRawPrint(const str& message, LogLevel level = kLevelInfo);

	str name_;
	Logger* parent_;
	std::vector<LogListener*> logger_list_[kLevelTypeCount];
	SpinLock* logger_list_lock_;
	LogLevel level_;
};



}
