
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../Cure/Include/ConsoleManager.h"
#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Network.h"
#include "../Lepra/Include/SystemManager.h"
#include "LifeApplication.h"
#include "LifeDefinitions.h"



namespace Life
{



Application::Application(const Lepra::StringUtility::StringVector& pArgumentList):
	Lepra::Application(pArgumentList),
	mResourceManager(0),
	mGameTicker(0),
	mConsoleLogger(0),
	mDebugLogger(0),
	mFileLogger(0),
	mPerformanceLogger(0),
	mMemLogger(0)
{
}

Application::~Application()
{
	// Kill all loggers, hopefully we don't need to log anything else.
	delete (mConsoleLogger);
	mConsoleLogger = 0;
	delete (mMemLogger);
	mMemLogger = 0;
	delete (mPerformanceLogger);
	mPerformanceLogger = 0;
	delete (mFileLogger);
	mFileLogger = 0;
	delete (mDebugLogger);
	mDebugLogger = 0;
}

void Application::Init()
{
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_PHYSICS_FPS, PHYSICS_FPS);

	mConsoleLogger = CreateConsoleLogListener();
	//mConsoleLogger->SetLevelThreashold(Lepra::Log::LEVEL_INFO);
#ifndef NO_LOG_DEBUG_INFO
	mDebugLogger = new Lepra::DebuggerLogListener();
#endif // Showing debug information.
	mFileLogger = new Lepra::FileLogListener(_TEXT_ALTERNATIVE(GetName()+".log", GetName()+L"U.log"));
	//mFileLogger->SetLevelThreashold(Lepra::Log::LEVEL_INFO);
	mFileLogger->WriteLog(_T("\n\n"), Lepra::Log::LEVEL_INFO);
	mPerformanceLogger = new Lepra::FileLogListener(_TEXT_ALTERNATIVE(GetName()+"Performance.log", GetName()+L"PerformanceU.log"));
	mMemLogger = new Lepra::MemFileLogListener(100*1024);
	Lepra::LogType::GetLog(Lepra::LogType::SUB_ROOT)->SetupBasicListeners(mConsoleLogger, mDebugLogger, mFileLogger, mPerformanceLogger, mMemLogger);

	Lepra::String lStartMessage = _T("---------- Starting ") + GetName() + _T(". Build type: ") _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT) _T(" ----------\n");
	mLog.RawPrint(Lepra::Log::LEVEL_HEADLINE, lStartMessage);

	mResourceManager = new Cure::ResourceManager(1);
	mGameTicker = CreateGameTicker();
}

int Application::Run()
{
	// We like all threads that measure time to be on a single CPU. This is due to high resolution timers,
	// which may differ between different CPU cores. Several seconds can differ between different cores.
	// The main thread is locked to the first CPU. If there are more CPUs, the physics thread will lock to
	// another CPU later on.
	Lepra::Thread::GetCurrentThread()->SetCpuAffinityMask(0x0001);

	bool lOk = true;
	if (lOk)
	{
		lOk = Lepra::Network::Start();
	}
	if (lOk)
	{
		lOk = mGameTicker->Initialize();
	}
	bool lQuit = false;
	Lepra::PerformanceData lTimeInfo;
	lTimeInfo.Set(1/60.0, 1/60.0, 1/60.0);
	while (lOk && !lQuit)
	{
		Lepra::ScopeTimer lTimer(&lTimeInfo);
		lOk = mGameTicker->Tick();
		if (lOk)
		{
			TickSleep(lTimeInfo.GetSlidingAverage());
		}
		lQuit = Lepra::SystemManager::GetQuitRequest();
	}

	if (lQuit)
	{
		mLog.AHeadline("User requested application termination.");
	}
	else if (!lOk)
	{
		if (mMemLogger)
		{
			Lepra::LogType::GetLog(Lepra::LogType::SUB_ROOT)->RemoveListener(mMemLogger);
		}
		mLog.AFatal("Terminating application due to fatal error.");
		if (mMemLogger && mFileLogger)
		{
			mLog.RawPrint(Lepra::Log::LEVEL_FATAL, _T("\nStart dump hires logs.\n--------------------------------------------------------------------------------\n"));
			mMemLogger->Dump(mFileLogger->GetFile());
			mLog.RawPrint(Lepra::Log::LEVEL_FATAL, _T("--------------------------------------------------------------------------------\nEnd dump hires logs.\n\n"));
		}
	}

	return (0);
}

void Application::Destroy()
{
	// Delete in order of priority.

	// Drop all major components first, while keeping all loggers intact.
	delete (mGameTicker);
	mGameTicker = 0;
	delete (mResourceManager);	// Resource manager lives long enough for all volontary resources to disappear.
	mResourceManager = 0;

	Lepra::Network::Stop();
}



Lepra::LogListener* Application::CreateConsoleLogListener() const
{
	return (new Lepra::StdioConsoleLogListener());
}



void Application::TickSleep(double pMeasuredFrameTime) const
{
	const float lPowerSaveAmount = mGameTicker->GetPowerSaveAmount();
	if (lPowerSaveAmount > 0)
	{
		Lepra::Thread::Sleep(1.0*lPowerSaveAmount);
	}
	else
	{
		const int lFps = CURE_RTVAR_GET(Cure::GetSettings(), RTVAR_PHYSICS_FPS, 2);
		double lWantedFrameTime = lFps? 1.0/lFps : 1;
		if (lWantedFrameTime > pMeasuredFrameTime)
		{
			Lepra::Thread::Sleep(lWantedFrameTime-pMeasuredFrameTime);
		}
		else
		{
			Lepra::Thread::YieldCpu();
		}
	}
}


LOG_CLASS_DEFINE(GAME, Application);



}
