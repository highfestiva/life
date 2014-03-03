
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/Application.h"
#include "../ThirdParty/FastDelegate/FastDelegate.h"
#include "../Lepra/Include/LogListener.h"
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
	typedef fastdelegate::FastDelegate0<void> ZombieTick;

	Application(const str& pBaseName, const strutil::strvec& pArgumentList);
	virtual ~Application();
	virtual void Init();
	virtual void Destroy();
	virtual int Run();
	virtual bool MainLoop();
	virtual bool Tick();

	virtual str GetTypeName() const = 0;
	virtual str GetVersion() const = 0;
	virtual Cure::ApplicationTicker* CreateTicker() const = 0;
	Cure::ApplicationTicker* GetTicker() const;
	void SetZombieTick(const ZombieTick& pZombieTick);

	static Application* GetApplication();
	static str GetIoFile(const str& pName, const str& pExt, bool pAddQuotes = true);

protected:
	virtual LogListener* CreateConsoleLogListener() const;

	Cure::ResourceManager* mResourceManager;
	Cure::ApplicationTicker* mGameTicker;
	LogListener* mConsoleLogger;

private:
	void TickSleep(double pMainLoopTime) const;
	void HandleZombieMode();

	static str mBaseName;
	ZombieTick mZombieTick;
	mutable bool mIsPowerSaving;

	LogListener* mDebugLogger;
	FileLogListener* mFileLogger;
	LogListener* mPerformanceLogger;
	MemFileLogListener* mMemLogger;

	LOG_CLASS_DECLARE();
};



}
