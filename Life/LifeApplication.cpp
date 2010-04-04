
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Cure/Include/ConsoleManager.h"
#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Network.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "Life.h"
#include "LifeApplication.h"
#include "RtVar.h"



namespace Life
{



Application::Application(const strutil::strvec& pArgumentList):
	Lepra::Application(pArgumentList),
	mResourceManager(0),
	mGameTicker(0),
	mConsoleLogger(0),
	mIsPowerSaving(false),
	mDebugLogger(0),
	mFileLogger(0),
	mPerformanceLogger(0),
	mMemLogger(0)
{
	mApplication = this;
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
	CURE_RTVAR_INTERNAL(Cure::GetSettings(), RTVAR_APPLICATION_NAME, _T("Life"));

	mConsoleLogger = CreateConsoleLogListener();
	//mConsoleLogger->SetLevelThreashold(Log::LEVEL_INFO);
#ifndef NO_LOG_DEBUG_INFO
	mDebugLogger = new DebuggerLogListener();
#endif // Showing debug information.
	mFileLogger = new FileLogListener(GetIoFile(_T(""), _T("log"), false));
	//mFileLogger->SetLevelThreashold(Log::LEVEL_INFO);
	mFileLogger->WriteLog(_T("\n\n"), Log::LEVEL_INFO);
	mPerformanceLogger = new FileLogListener(GetIoFile(_T("Performance"), _T("log"), false));
	mMemLogger = new MemFileLogListener(100*1024);
	LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(mConsoleLogger, mDebugLogger, mFileLogger, mPerformanceLogger, mMemLogger);

	str lStartMessage = _T("---------- Starting ") + GetName() + _T(". Build type: ") _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT) _T(" ----------\n");
	mLog.RawPrint(Log::LEVEL_HEADLINE, lStartMessage);

	mResourceManager = new Cure::ResourceManager(1);
	mGameTicker = CreateGameTicker();
}

int Application::Run()
{
	// We like all threads that measure time to be on a single CPU. This is due to high resolution timers,
	// which may differ between different CPU cores. Several seconds can differ between different cores.
	// The main thread is locked to the first CPU. If there are more CPUs, the physics thread will lock to
	// another CPU later on.
	// JB 2009-12: dropped this, probably not a good idea since we need to run multiple
	// physics instances when running split screen.
	//Thread::GetCurrentThread()->SetCpuAffinityMask(0x0001);

	bool lOk = true;
	if (lOk)
	{
		lOk = Network::Start();
	}
	if (lOk)
	{
		lOk = mGameTicker->Initialize();
	}
	bool lQuit = false;
	PerformanceData lTimeInfo;
	lTimeInfo.Set(1/60.0, 1/60.0, 1/60.0);
	while (lOk && !lQuit)
	{
		mGameTicker->Profile();

		LEPRA_MEASURE_SCOPE(AppTick);
		{
			ScopeTimer lSleepTimer(&lTimeInfo);
			lOk = mGameTicker->Tick();
			const float lExtraSleep = (float)CURE_RTVAR_TRYGET(Cure::GetSettings(), RTVAR_DEBUG_EXTRASLEEPTIME, 0.0);
			if (lExtraSleep > 0)
			{
				Thread::Sleep(lExtraSleep);
			}
		}
		if (lOk)
		{
			LEPRA_MEASURE_SCOPE(AppSleep);
			TickSleep(lTimeInfo.GetSlidingAverage());
		}
		lQuit = (SystemManager::GetQuitRequest() != 0);
	}

	if (lQuit)
	{
		mLog.AHeadline("User requested application termination.");
	}
	else if (!lOk)
	{
		if (mMemLogger)
		{
			LogType::GetLog(LogType::SUB_ROOT)->RemoveListener(mMemLogger);
		}
		mLog.AFatal("Terminating application due to fatal error.");
		if (mMemLogger && mFileLogger)
		{
			mLog.RawPrint(Log::LEVEL_FATAL, _T("\nStart dump hires logs.\n--------------------------------------------------------------------------------\n"));
			mMemLogger->Dump(mFileLogger->GetFile());
			mLog.RawPrint(Log::LEVEL_FATAL, _T("--------------------------------------------------------------------------------\nEnd dump hires logs.\n\n"));
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

	Network::Stop();
}



str Application::GetIoFile(const str& pEnd, const str& pExt, bool pAddQuotes)
{
	str lIoName = Path::JoinPath(
		SystemManager::GetIoDirectory(CURE_RTVAR_GET(Cure::GetSettings(), RTVAR_APPLICATION_NAME, _T("?"))),
		mApplication->GetName()+pEnd, pExt);
	if (pAddQuotes)
	{
		lIoName = _T("\"") + lIoName + _T("\"");
	}
	return (lIoName);
}



LogListener* Application::CreateConsoleLogListener() const
{
	return (new StdioConsoleLogListener());
}



void Application::TickSleep(double pMeasuredFrameTime) const
{
	const float lPowerSaveFactor = (float)CURE_RTVAR_TRYGET(Cure::GetSettings(), RTVAR_DEBUG_POWERSAVEFACTOR, 1.0);
	const float lPowerSaveAmount = mGameTicker->GetPowerSaveAmount() * lPowerSaveFactor;
	if (lPowerSaveAmount > 0)
	{
		if (!mIsPowerSaving)
		{
			mLog.AInfo("Entering power save mode.");
			mIsPowerSaving = true;
		}
		Thread::Sleep(1.0*lPowerSaveAmount);
	}
	else
	{
		if (mIsPowerSaving)
		{
			mLog.AInfo("Leaving power save mode.");
			mIsPowerSaving = false;
		}

		const int lFps = CURE_RTVAR_GET(Cure::GetSettings(), RTVAR_PHYSICS_FPS, 2);
		double lWantedFrameTime = lFps? 1.0/lFps : 1;
		double lSleepTime = lWantedFrameTime - pMeasuredFrameTime;
		const double MINIMUM_SLEEP_TIME = 0.001;
		if (lSleepTime >= MINIMUM_SLEEP_TIME)
		{
			HiResTimer lSleepTimer;
			while (lSleepTime >= MINIMUM_SLEEP_TIME)
			{
				Thread::Sleep(lSleepTime);
				lSleepTime -= lSleepTimer.PopTimeDiff();
			}
		}
		else
		{
			Thread::YieldCpu();	// Play nice!
		}
	}
}



Application* Application::mApplication;



LOG_CLASS_DEFINE(GAME, Application);



}
