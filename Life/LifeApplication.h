
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
class GameTicker;
class ResourceManager;
}



namespace Life
{



class Application: public Lepra::Application
{
public:
	Application(const strutil::strvec& pArgumentList);
	virtual ~Application();
	void Init();
	int Run();
	virtual bool Tick();
	void Destroy();

	virtual str GetName() const = 0;
	virtual Cure::GameTicker* CreateGameTicker() const = 0;

	static str GetIoFile(const str& pName, const str& pExt, bool pAddQuotes = true);

protected:
	virtual LogListener* CreateConsoleLogListener() const;

	Cure::ResourceManager* mResourceManager;
	Cure::GameTicker* mGameTicker;
	LogListener* mConsoleLogger;

private:
	void TickSleep(double pMeasuredFrameTime) const;

	mutable bool mIsPowerSaving;

	LogListener* mDebugLogger;
	FileLogListener* mFileLogger;
	LogListener* mPerformanceLogger;
	MemFileLogListener* mMemLogger;

	PerformanceData mTimeInfo;

	LOG_CLASS_DECLARE();
};



}
