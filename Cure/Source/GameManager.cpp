
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../../Cure/Include/RuntimeVariableName.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../TBC/Include/PhysicsEngineFactory.h"
#include "../Include/ConsoleManager.h"
#include "../Include/ContextManager.h"
#include "../Include/ContextObject.h"
#include "../Include/GameManager.h"
#include "../Include/NetworkAgent.h"
#include "../Include/RuntimeVariable.h"
#include "../Include/TerrainManager.h"
#include "../Include/TimeManager.h"



namespace Cure
{



GameTicker::~GameTicker()
{
}



GameManager::GameManager(RuntimeVariableScope* pVariableScope, ResourceManager* pResourceManager, bool pForceSynchronous):
	mIsThreadSafe(true),
	mVariableScope(pVariableScope),
	mResource(pResourceManager),
	mNetwork(0),
	mTime(new TimeManager(CURE_RTVAR_GET(pVariableScope, RTVAR_PHYSICS_FPS, 60))),
	mPhysics(TBC::PhysicsEngineFactory::Create(TBC::PhysicsEngineFactory::ENGINE_ODE)),
	mContext(0),
	mTerrain(new TerrainManager(pResourceManager)),
	mConsole(0),
	mPhysicsWorkerThread(0),
	mPhysicsTickStartSemaphore(0),
	mPhysicsTickDoneSemaphore(0)
{
	mContext = new ContextManager(this);

	if (!pForceSynchronous)
	{
		CreatePhysicsThread();
	}
}

GameManager::~GameManager()
{
	DeletePhysicsThread();

	delete (mConsole);
	mConsole = 0;
	delete (mContext);
	mContext = 0;
	delete (mTime);
	mTime = 0;
	delete (mTerrain);
	mTerrain = 0;
	delete (mPhysics);
	mPhysics = 0;
	delete (mNetwork);
	mNetwork = 0;
	mResource = 0;
	delete (mVariableScope);
	mVariableScope = 0;
}



void GameManager::ResetPhysicsTime(int pStartPhysicsFrame)
{
	mTime->Clear(pStartPhysicsFrame);
}

bool GameManager::BeginTick()
{
	if (CURE_RTVAR_GET(GetVariableScope(), RTVAR_PERFORMANCE_TEXT_ENABLE, false))
	{
		const double lReportInterval = CURE_RTVAR_GET(GetVariableScope(), RTVAR_PERFORMANCE_TEXT_INTERVAL, 10.0);
		ReportPerformance(lReportInterval);
	}

	Lepra::ScopeTimer lWallTimer(&mWallTime);

	GetTickLock()->Acquire();

	{
		Lepra::ScopeTimer lTime(&mNetworkAndInputTime);

		mTime->TickTime();

		mTime->TickPhysics();

		// Sorts up incoming network data; adds/removes objects (for instance via remote create/delete).
		// On UI-based managers we handle user input here as well.
		TickInput();

		// Sends network packets. Among other things, movement of locally-controlled objects are sent.
		TickNetworkOutput();
	}

	{
		Lepra::ScopeTimer lTime(&mPhysicsPropagationTime);
		ScriptPhysicsTick();
	}

	if (mTime->GetCurrentPhysicsStepCount() > 0)
	{
		// Physics thread
		// 1. does *NOT* add/delete objects,
		// 2. processes context objects ("scripts"),
		// 3. applies movement indata (from network, UI and script), and
		// 4. integrates physics.
		// Performance determines if this is done by a worker thread, or done synchronously
		// by the current thread. (Single CPU => single thread...)
		StartPhysicsTick();
	}

	return (true);
}

bool GameManager::EndTick()
{
	if (mTime->GetCurrentPhysicsStepCount() > 0)
	{
		Lepra::ScopeTimer lTime(&mWaitPhysicsTime);
		GetTickLock()->Release();
		// Wait for physics thread to complete integration. If we run physics synchronously,
		// this does nothing.
		WaitPhysicsTick();
	}
	else
	{
		GetTickLock()->Release();
	}


	return (true);
}

Lepra::Lock* GameManager::GetTickLock() const
{
	return (&mLock);
}



RuntimeVariableScope* GameManager::GetVariableScope() const
{
	return (mVariableScope);
}

ResourceManager* GameManager::GetResourceManager() const
{
	return (mResource);
}

ContextManager* GameManager::GetContext() const
{
	return (mContext);
}

const TimeManager* GameManager::GetConstTimeManager() const
{
	return (mTime);
}

TBC::PhysicsEngine* GameManager::GetPhysicsManager() const
{
#ifdef LEPRA_DEBUG
	// This is a check to see if the code that fetches the physics manager
	// is thread-safe. We definitely don't want to impose mutex locks on
	// the non-physics threads, since that would just wait and kill all
	// performance on a dual or faster system.

	// Check if we are
	//   a) in thread-safe part of code (=physics thread not running), or
	//   b) we don't have a physics thread (we run physics using the main thread).
	if (!mIsThreadSafe && mPhysicsWorkerThread)
	{
		// We have a physics thread. Are we it?
		size_t lPhysicsThreadId = mPhysicsWorkerThread->GetThreadId();
		size_t lThisThreadId = Lepra::Thread::GetCurrentThreadId();
		// Make sure we're the physics thread, otherwise we're not allowed to
		// read/write any physical stuff.
		assert(lPhysicsThreadId == lThisThreadId);
	}
#endif // LEPRA_DEBUG
	return (mPhysics);
}

ConsoleManager* GameManager::GetConsoleManager() const
{
	return (mConsole);
}



void GameManager::ReportPerformance(double pReportInterval)
{
	mPerformanceReportTimer.UpdateTimer();
	const double lTimeDiff = mPerformanceReportTimer.GetTimeDiffF();
	if (lTimeDiff >= pReportInterval)
	{
		mPerformanceReportTimer.ClearTimeDiff();

		if (mNetwork->IsOpen())
		{
			mSendBandwidth.Update(lTimeDiff, mNetwork->GetTotalSentByteCount());
			mReceiveBandwidth.Update(lTimeDiff, mNetwork->GetTotalReceivedByteCount());
			mLog.Performancef(_T("Network bandwith. Up: %sB/s (peak %sB/s). Down: %sB/s (peak %sB/s)."), 
				Lepra::Number::ConvertToPostfixNumber(mSendBandwidth.GetLast(), 2).c_str(),
				Lepra::Number::ConvertToPostfixNumber(mSendBandwidth.GetMaximum(), 2).c_str(),
				Lepra::Number::ConvertToPostfixNumber(mReceiveBandwidth.GetLast(), 2).c_str(),
				Lepra::Number::ConvertToPostfixNumber(mReceiveBandwidth.GetMaximum(), 2).c_str());
		}
		else
		{
			mSendBandwidth.Clear();
			mReceiveBandwidth.Clear();
		}

		ReportPerformance(_T("Physics time."), mPhysicsTime);
		ReportPerformance(_T("Network, user input time."), mNetworkAndInputTime);
		ReportPerformance(_T("Wait physics time."), mWaitPhysicsTime);
		ReportPerformance(_T("Wall time."), mWallTime);
		if (mWallTime.GetLast())
		{
			mLog.Performancef(_T("FPS: %f, worst: %f."), 1/mWallTime.GetLast(), 1/mWallTime.GetMaximum());
		}
	}
}

void GameManager::ClearPerformanceData()
{
	mPhysicsTime.Clear();
	mNetworkAndInputTime.Clear();
	mWaitPhysicsTime.Clear();
	mWallTime.Clear();
}



void GameManager::SetConsoleManager(ConsoleManager* pConsole)
{
	mConsole = pConsole;
}



NetworkAgent* GameManager::GetNetworkAgent() const
{
	return (mNetwork);
}

void GameManager::SetNetworkAgent(NetworkAgent* pNetwork)
{
	delete (mNetwork);
	mNetwork = pNetwork;
}

TimeManager* GameManager::GetTimeManager()
{
	return (mTime);
}



bool GameManager::TickNetworkOutput()
{
	return (mNetwork->SendAll());
}



void GameManager::ReportPerformance(const Lepra::String& pHead, const Lepra::PerformanceData& pPerformance)
{
	mLog.Performancef((pHead+_T(" Min: %ss, last: %ss, savg: %ss, max: %ss.")).c_str(), 
		Lepra::Number::ConvertToPostfixNumber(pPerformance.GetMinimum(), 2).c_str(),
		Lepra::Number::ConvertToPostfixNumber(pPerformance.GetLast(), 2).c_str(),
		Lepra::Number::ConvertToPostfixNumber(pPerformance.GetSlidingAverage(), 2).c_str(),
		Lepra::Number::ConvertToPostfixNumber(pPerformance.GetMaximum(), 2).c_str());
}



void GameManager::StartPhysicsTick()
{
	mIsThreadSafe = false;
	if (mPhysicsTickStartSemaphore)
	{
		// We use a bool to check if all accesses are thread-safe.
		mPhysicsTickStartSemaphore->Signal();
	}
	else
	{
		PhysicsTick();
	}
}

void GameManager::WaitPhysicsTick()
{
	if (mPhysicsTickDoneSemaphore)
	{
#ifdef LEPRA_DEBUG
		mPhysicsTickDoneSemaphore->Wait();
#else // !Debugging
		mPhysicsTickDoneSemaphore->Wait(3.0);
#endif // Debugging/!debugging.
	}
	mIsThreadSafe = true;	// For error-checking.
}



void GameManager::PhysicsTick()
{
	Lepra::HiResTimer lTime;

	const int lAffordedStepCount = mTime->GetAffordedPhysicsStepCount();
	const float lStepIncrement = mTime->GetAffordedStepPeriod();
	for (int x = 0; x < lAffordedStepCount; ++x)
	{
		ScriptTick();
		mPhysics->StepFast(lStepIncrement);
	}

	mContext->HandleIdledBodies();
	mContext->HandlePhysicsSend();

	mPhysicsTime.Update(lTime.PopTimeDiff());
}

bool GameManager::IsHighImpact(float pScaleFactor, const ContextObject* pObject, const Lepra::Vector3DF& pForce,
	const Lepra::Vector3DF& pTorque) const
{
	const float lMassFactor = 1/pObject->GetMass();
	Lepra::Vector3DF lGravityDirection = GetPhysicsManager()->GetGravity();
	lGravityDirection.Normalize();
	// High angle against direction of gravity means high impact.
	const float lForceWithoutGravityFactor = (pForce * lGravityDirection) - pForce.Cross(lGravityDirection).GetLength();
	const float lNormalizedForceFactor = lForceWithoutGravityFactor * lMassFactor;
	const float lNormalizedTorqueFactor = pTorque.GetLength() * lMassFactor;
	bool lIsHighImpact = (lNormalizedForceFactor < -16*pScaleFactor || lNormalizedForceFactor >= 2*pScaleFactor || lNormalizedTorqueFactor > 3*pScaleFactor);
	if (lIsHighImpact)
	{
		log_volatile(mLog.Debugf(_T("Collided hard with something dynamic. F=%f, T=%f"),
			lNormalizedForceFactor, lNormalizedTorqueFactor));
	}
	return (lIsHighImpact);
}



bool GameManager::IsThreadSafe() const
{
	return (mIsThreadSafe);
}

void GameManager::ScriptTick()
{
	mContext->Tick();
}

void GameManager::ScriptPhysicsTick()
{
	//if (mTime->GetCurrentPhysicsStepCount() > 0)
	{
		mContext->TickPhysics();
	}
}


void GameManager::PhysicsThreadEntry()
{
	// We set affinity to the second processor. This is due to high resolution timers,
	// which may differ between different CPU cores. Several seconds can differ between
	// different cores. The main thread is locked to the first CPU, therefore this one
	// goes to the last one.
	Lepra::Thread::GetCurrentThread()->SetCpuAffinityMask(1<<(Lepra::SystemManager::GetLogicalCpuCount()-1));

	while (mPhysicsWorkerThread && !mPhysicsWorkerThread->GetStopRequest())
	{
		mPhysicsTickStartSemaphore->Wait();
		if (!mPhysicsWorkerThread->GetStopRequest())
		{
			assert(!mIsThreadSafe);
			PhysicsTick();
			assert(!mIsThreadSafe);
			mPhysicsTickDoneSemaphore->Signal();
		}
	}
}

void GameManager::CreatePhysicsThread()
{
	// If we have more than one CPU, we run a separate physics thread.
	if (Lepra::SystemManager::GetLogicalCpuCount() > 1)	// TODO: check performance to see if we should check for logical or physical CPUs.
	{
		assert(!mPhysicsWorkerThread);

		mPhysicsTickStartSemaphore = new Lepra::Semaphore();
		mPhysicsTickDoneSemaphore = new Lepra::Semaphore();

		mPhysicsWorkerThread = new Lepra::MemberThread<GameManager>(_T("PhysicsThread"));
		mPhysicsWorkerThread->Start(this, &GameManager::PhysicsThreadEntry);
	}
}

void GameManager::DeletePhysicsThread()
{
	if (mPhysicsWorkerThread)
	{
		mPhysicsWorkerThread->RequestStop();
		mPhysicsTickStartSemaphore->Signal();
		mPhysicsWorkerThread->Join(3.0);
		delete (mPhysicsWorkerThread);
		mPhysicsWorkerThread = 0;

		delete (mPhysicsTickStartSemaphore);
		mPhysicsTickStartSemaphore = 0;
		delete (mPhysicsTickDoneSemaphore);
		mPhysicsTickDoneSemaphore = 0;
	}
}



LOG_CLASS_DEFINE(GAME, GameManager);



}
