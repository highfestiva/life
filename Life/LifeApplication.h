
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "../Lepra/Include/Application.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/String.h"



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
	Application(const Lepra::StringUtility::StringVector& pArgumentList);
	virtual ~Application();
	void Init();
	int Run();
	void Destroy();

	virtual Lepra::String GetName() const = 0;
	virtual Cure::GameTicker* CreateGameTicker() const = 0;

	static Lepra::String GetIoFile(const Lepra::String& pEnd, const Lepra::String& pExt, bool pAddQuotes = true);

protected:
	virtual Lepra::LogListener* CreateConsoleLogListener() const;

	Cure::ResourceManager* mResourceManager;
	Cure::GameTicker* mGameTicker;
	Lepra::LogListener* mConsoleLogger;

private:
	void TickSleep(double pMeasuredFrameTime) const;

	mutable bool mIsPowerSaving;

	Lepra::LogListener* mDebugLogger;
	Lepra::FileLogListener* mFileLogger;
	Lepra::LogListener* mPerformanceLogger;
	Lepra::MemFileLogListener* mMemLogger;

	static Application* mApplication;

	LOG_CLASS_DECLARE();
};



}
