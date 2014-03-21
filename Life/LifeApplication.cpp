
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "LifeApplication.h"
#include "../Cure/Include/ConsoleManager.h"
#include "../Cure/Include/GameTicker.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/AntiCrack.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Network.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/SystemManager.h"
#include "Life.h"
#include "RtVar.h"



namespace
{
// Run before main() is started.
AntiCrack _r__;

Life::Application* gApplication = 0;
}



namespace Life
{



Application::Application(const str& pBaseName, const strutil::strvec& pArgumentList):
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
	gApplication = this;
	mBaseName = pBaseName;
}

Application::~Application()
{
	mLog.RawPrint(LEVEL_HEADLINE, _T("The end. Baba!\n"));
	if (mFileLogger)
	{
		mFileLogger->WriteLog(_T("---\n\n"), LEVEL_INFO);
	}

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

	// Drop performance measurement resources.
	ScopePerformanceData::EraseAll();

	LogType::Close();
}

void Application::Init()
{
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_PHYSICS_FPS, PHYSICS_FPS);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_POWERSAVE_FACTOR, 2.0);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_DEBUG_EXTRASLEEPTIME, 0.0);

	mConsoleLogger = CreateConsoleLogListener();
	//mConsoleLogger->SetLevelThreashold(LEVEL_INFO);
#ifndef NO_LOG_DEBUG_INFO
#ifndef LEPRA_MAC
	mDebugLogger = new DebuggerLogListener();
#endif // !Apple
#endif // Showing debug information.
#ifdef LEPRA_DEBUG
	//mFileLogger = new FileLogListener(GetIoFile(GetTypeName(), _T("log"), false));
	//mFileLogger->SetLevelThreashold(LEVEL_INFO);
	//mFileLogger->WriteLog(_T("\n---\n"), LEVEL_INFO);
	//mPerformanceLogger = new FileLogListener(GetIoFile(GetTypeName()+_T("Performance"), _T("log"), false));
	mMemLogger = new MemFileLogListener(20*1024);
#endif // Debug
	LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(mConsoleLogger, mDebugLogger, mFileLogger, mPerformanceLogger, mMemLogger);

	str lStartMessage = _T("Starting ") + mBaseName + _T(" ") + GetTypeName() + _T(", version ") + GetVersion() + _T(", build type: ") _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT) _T(".\n");
	mLog.RawPrint(LEVEL_HEADLINE, lStartMessage);

	mResourceManager = new Cure::ResourceManager(1);
	mGameTicker = CreateTicker();
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
	if (lOk)
	{
		lOk = MainLoop();
	}
	return lOk? 0 : 1;
}

bool Application::MainLoop()
{
	bool lOk = true;
	bool lQuit = false;
	while (lOk && !lQuit)
	{
		lOk = Tick();
		lQuit = mGameTicker->QueryQuit();
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
			mLog.RawPrint(LEVEL_FATAL, _T("\nStart dump hires logs.\n--------------------------------------------------------------------------------\n"));
			mMemLogger->Dump(mFileLogger->GetFile());
			mLog.RawPrint(LEVEL_FATAL, _T("--------------------------------------------------------------------------------\nEnd dump hires logs.\n\n"));
		}
	}
	return lOk;
}

bool Application::Tick()
{
	HiResTimer::StepCounterShadow();

	LEPRA_DO_MEASURE_SCOPE(AppTick);
	HiResTimer lLoopTime(false);

	bool lDebug;
	CURE_RTVAR_GET(lDebug, =, Cure::GetSettings(), RTVAR_DEBUG_ENABLE, false);
	if (lDebug)
	{
		mGameTicker->Profile();
	}
	Random::GetRandomNumber();	// To move seed ahead.
	bool lOk = mGameTicker->Tick();
	float lExtraSleep;
	CURE_RTVAR_GET(lExtraSleep, =(float), Cure::GetSettings(), RTVAR_DEBUG_EXTRASLEEPTIME, 0.0);
	if (lExtraSleep > 0)
	{
		Thread::Sleep(lExtraSleep);
	}
	if (lOk)
	{
		LEPRA_DO_MEASURE_SCOPE(AppSleep);
		TickSleep(lLoopTime.QueryTimeDiff());
	}
	if (lOk)
	{
		HandleZombieMode();
	}
	return lOk;
}



Cure::ApplicationTicker* Application::GetTicker() const
{
	return mGameTicker;
}

void Application::SetZombieTick(const ZombieTick& pZombieTick)
{
	mZombieTick = pZombieTick;
}



Application* Application::GetApplication()
{
	return gApplication;
}

str Application::GetIoFile(const str& pName, const str& pExt, bool pAddQuotes)
{
	str lAppName = mBaseName;
	str lIoName = Path::JoinPath(
		SystemManager::GetIoDirectory(lAppName),
		pName, pExt);
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



void Application::TickSleep(double pMainLoopTime) const
{
	float lPowerSaveFactor;
	CURE_RTVAR_GET(lPowerSaveFactor, =(float), Cure::GetSettings(), RTVAR_POWERSAVE_FACTOR, 2.0);
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

		int lFps;
		CURE_RTVAR_GET(lFps, =, Cure::GetSettings(), RTVAR_PHYSICS_FPS, 2);
		double lWantedFrameTime = lFps? 1.0/lFps : 1;
		const double lSleepTime = lWantedFrameTime - pMainLoopTime - mGameTicker->GetTickTimeReduction();
		const double MAXIMUM_SLEEP_TIME = 0.01;
		if (lSleepTime >= 0)
		{
			double lSleepTimeLeft = lSleepTime;
			HiResTimer lSleepTimer(false);
			while (lSleepTimeLeft >= 0)
			{
				if (lSleepTimeLeft > MAXIMUM_SLEEP_TIME*1.7)
				{
					mGameTicker->PollRoundTrip();
					Thread::Sleep(MAXIMUM_SLEEP_TIME);
				}
				else if (lSleepTimeLeft > SystemManager::GetSleepResolution())
				{
					Thread::Sleep(lSleepTimeLeft - SystemManager::GetSleepResolution()/2);
				}
				else
				{
					Thread::YieldCpu();	// For lousy computers that have very big sleep intervals.
				}
				lSleepTimeLeft = lSleepTime - lSleepTimer.QueryTimeDiff();
			}
			if (lSleepTimeLeft < -0.01)
			{
				mLog.Warningf(_T("Overslept %f s!"), -lSleepTimeLeft);
			}
			//log_volatile(mLog.Debugf(_T("Done tick sleeping, %f s left in sleep loop (measured), %f s reduction, %f s measured."), lSleepTimeLeft, mGameTicker->GetTickTimeReduction(), lSleepTime));
		}
		else
		{
#ifndef LEPRA_IOS
			//log_volatile(mLog.Debugf(_T("Skipping tick sleep (yield only), %f s left in sleep loop, %f s reduction."), lSleepTime, mGameTicker->GetTickTimeReduction()));
			Thread::YieldCpu();	// Play nice even though time's up!
#endif	// Not on iOS (which sleeps during timer ticks).
		}
	}
}

void Application::HandleZombieMode()
{
	if (mZombieTick)
	{
		mZombieTick();
		Destroy();
		mZombieTick();
		SystemManager::AddQuitRequest(-1);
		ScopePerformanceData::EraseAll();
		Init();
		Network::Start();
		mGameTicker->Initialize();
		mZombieTick();
	}
}



str Application::mBaseName;

LOG_CLASS_DEFINE(GAME, Application);



}
