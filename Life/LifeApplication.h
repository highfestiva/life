
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Lepra/Include/Application.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Performance.h"
#include "../Lepra/Include/String.h"
#include "Life.h"



namespace Cure
{
class ApplicationTicker;
class ResourceManager;
}



namespace Life
{



class Application: public Lepra::Application
{
public:
	Application(const str& pBaseName, const strutil::strvec& pArgumentList);
	virtual ~Application();
	void Init();
	int Run();
	virtual bool MainLoop();
	virtual bool Tick();
	void Destroy();

	virtual str GetName() const = 0;
	virtual str GetVersion() const = 0;
	virtual Cure::ApplicationTicker* CreateTicker() const = 0;

	static str GetIoFile(const str& pName, const str& pExt, bool pAddQuotes = true);

protected:
	virtual LogListener* CreateConsoleLogListener() const;

	Cure::ResourceManager* mResourceManager;
	Cure::ApplicationTicker* mGameTicker;
	LogListener* mConsoleLogger;

private:
	void TickSleep(double pMeasuredFrameTime) const;

	str mBaseName;
	mutable bool mIsPowerSaving;

	LogListener* mDebugLogger;
	FileLogListener* mFileLogger;
	LogListener* mPerformanceLogger;
	MemFileLogListener* mMemLogger;

	PerformanceData mTimeInfo;

	LOG_CLASS_DECLARE();
};



}
