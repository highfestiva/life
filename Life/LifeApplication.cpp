
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "../Cure/Include/ConsoleManager.h"
#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/AntiCrack.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Network.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/SystemManager.h"
#include "Life.h"
#include "LifeApplication.h"
#include "RtVar.h"



// Run before main() is started.
AntiCrack _r__;



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
}

Application::~Application()
{
	mLog.RawPrint(Log::LEVEL_HEADLINE, _T("The end. Baba!\n"));
	mFileLogger->WriteLog(_T("---\n\n"), Log::LEVEL_INFO);

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
	ScopePerformanceData::ClearAll();	// Make all useless.
	ScopePerformanceData::ClearAll();	// Delete all useless.

	LogType::Close();
}

void Application::Init()
{
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_PHYSICS_FPS, PHYSICS_FPS);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_POWERSAVE_FACTOR, 2.0);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_DEBUG_ENABLE, false);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_DEBUG_3D_ENABLEAXES, false);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_DEBUG_3D_ENABLEJOINTS, false);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_DEBUG_3D_ENABLESHAPES, true);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_YOFFSET, 10);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_GRAPH, false);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_NAMES, true);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_COUNT, true);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_DEBUG_EXTRASLEEPTIME, 0.0);
	const str lApplicationName(_T("Life"));
	CURE_RTVAR_INTERNAL(Cure::GetSettings(), RTVAR_APPLICATION_NAME, lApplicationName);

	mConsoleLogger = CreateConsoleLogListener();
	//mConsoleLogger->SetLevelThreashold(Log::LEVEL_INFO);
#ifndef NO_LOG_DEBUG_INFO
	mDebugLogger = new DebuggerLogListener();
#endif // Showing debug information.
	mFileLogger = new FileLogListener(GetIoFile(GetName(), _T("log"), false));
	//mFileLogger->SetLevelThreashold(Log::LEVEL_INFO);
	mFileLogger->WriteLog(_T("\n---\n"), Log::LEVEL_INFO);
	//mPerformanceLogger = new FileLogListener(GetIoFile(GetName()+_T("Performance"), _T("log"), false));
	mMemLogger = new MemFileLogListener(20*1024);
	LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(mConsoleLogger, mDebugLogger, mFileLogger, mPerformanceLogger, mMemLogger);

	str lStartMessage = _T("Starting ") + lApplicationName + _T(" ") + GetName() + _T(", version ") + GetVersion() + _T(", build type: ") _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT) _T(".\n");
	mLog.RawPrint(Log::LEVEL_HEADLINE, lStartMessage);

	str lPathPrefix;
#if defined(LEPRA_MAC)
	lPathPrefix = Path::GetDirectory(mArgumentVector[0]);
	lPathPrefix = Path::GetParentDirectory(lPathPrefix);
	lPathPrefix = Path::JoinPath(lPathPrefix, _T("Resources/"));
#endif // Mac
	mLog.Debugf(_T("Using path prefix: %s"), lPathPrefix.c_str());
	mResourceManager = new Cure::ResourceManager(1, lPathPrefix);
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
	mTimeInfo.Set(1.0/CURE_STANDARD_FRAME_RATE, 1.0/CURE_STANDARD_FRAME_RATE, 1.0/CURE_STANDARD_FRAME_RATE);
	while (lOk && !lQuit)
	{
		LEPRA_MEASURE_SCOPE(AppTick);
		lOk = Tick();
		if (lOk)
		{
			LEPRA_MEASURE_SCOPE(AppSleep);
			TickSleep(mTimeInfo.GetSlidingAverage());
		}
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
			mLog.RawPrint(Log::LEVEL_FATAL, _T("\nStart dump hires logs.\n--------------------------------------------------------------------------------\n"));
			mMemLogger->Dump(mFileLogger->GetFile());
			mLog.RawPrint(Log::LEVEL_FATAL, _T("--------------------------------------------------------------------------------\nEnd dump hires logs.\n\n"));
		}
	}

	return (0);
}

bool Application::Tick()
{
	ScopeTimer lSleepTimer(&mTimeInfo);
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
	return lOk;
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



str Application::GetIoFile(const str& pName, const str& pExt, bool pAddQuotes)
{
	str lAppName;
	CURE_RTVAR_GET(lAppName, =, Cure::GetSettings(), RTVAR_APPLICATION_NAME, _T("?"));
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



void Application::TickSleep(double pMeasuredFrameTime) const
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
		const double lSleepTime = lWantedFrameTime - pMeasuredFrameTime - mGameTicker->GetTickTimeReduction();
		const double MINIMUM_SLEEP_TIME = 0.001;
		const double MAXIMUM_SLEEP_TIME = 0.01;
		if (lSleepTime >= MINIMUM_SLEEP_TIME)
		{
			double lSleepTimeLeft = lSleepTime;
			HiResTimer lSleepTimer;
			while (lSleepTimeLeft >= MINIMUM_SLEEP_TIME)
			{
				if (lSleepTimeLeft > MAXIMUM_SLEEP_TIME*1.5)
				{
					mGameTicker->PollRoundTrip();
					Thread::Sleep(MAXIMUM_SLEEP_TIME);
				}
				else if (lSleepTimeLeft <= SystemManager::GetSleepResolution())
				{
					Thread::Sleep(lSleepTimeLeft);
				}
				else
				{
					Thread::YieldCpu();	// For lousy computers that have very big sleep intervals.
				}
				lSleepTimeLeft = lSleepTime - lSleepTimer.QueryTimeDiff();
			}
		}
		else
		{
			Thread::YieldCpu();	// Play nice even though time's up!
		}
	}
}



LOG_CLASS_DEFINE(GAME, Application);



}
