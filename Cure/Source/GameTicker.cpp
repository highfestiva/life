
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/GameTicker.h"
#include "../Include/RuntimeVariableName.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../TBC/Include/PhysicsManagerFactory.h"
#include "../Include/ConsoleManager.h"
#include "../Include/ContextManager.h"
#include "../Include/ContextObject.h"
#include "../Include/NetworkAgent.h"
#include "../Include/RuntimeVariable.h"
#include "../Include/TerrainManager.h"
#include "../Include/TimeManager.h"



namespace Cure
{



ApplicationTicker::ApplicationTicker()
{
}

ApplicationTicker::~ApplicationTicker()
{
}

void ApplicationTicker::Suspend()
{
}

void ApplicationTicker::Resume()
{
	HiResTimer::StepCounterShadow();
}

void ApplicationTicker::Profile()
{
}

bool ApplicationTicker::QueryQuit()
{
	return (SystemManager::GetQuitRequest() != 0);
}



GameTicker::GameTicker(float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	mTimeManager(new TimeManager),
	mPhysicsManager(TBC::PhysicsManagerFactory::Create(TBC::PhysicsManagerFactory::ENGINE_ODE, pPhysicsRadius, pPhysicsLevels,  pPhysicsSensitivity)),
	mPhysicsWorkerThread(0),
	mPhysicsTickStartSemaphore(0),
	mPhysicsTickDoneSemaphore(0)
{
	mPhysicsManager->SetTriggerListener(this);
	mPhysicsManager->SetForceFeedbackListener(this);
	mPhysicsManager->InitCurrentThread();
}

GameTicker::~GameTicker()
{
	DeletePhysicsThread();
	delete (mPhysicsManager);
	mPhysicsManager = 0;
	delete mTimeManager;
	mTimeManager = 0;
}

const TimeManager* GameTicker::GetTimeManager() const
{
	return mTimeManager;
}

TimeManager* GameTicker::GetTimeManager()
{
	return mTimeManager;
}

TBC::PhysicsManager* GameTicker::GetPhysicsManager(bool pIsThreadSafe) const
{
	(void)pIsThreadSafe;
#ifdef LEPRA_DEBUG
	// This is a check to see if the code that fetches the physics manager
	// is thread-safe. We definitely don't want to impose mutex locks on
	// the non-physics threads, since that would just wait and kill all
	// performance on a dual or faster system.

	// Check if we are
	//   a) in thread-safe part of code (=physics thread not running), or
	//   b) we don't have a physics thread (we run physics using the main thread).
	if (!pIsThreadSafe && mPhysicsWorkerThread)
	{
		// We have a physics thread. Are we it?
		size_t lPhysicsThreadId = mPhysicsWorkerThread->GetThreadId();
		size_t lThisThreadId = Thread::GetCurrentThreadId();
		// Make sure we're the physics thread, otherwise we're not allowed to
		// read/write any physical stuff.
		deb_assert(lPhysicsThreadId == lThisThreadId);
	}
#endif // LEPRA_DEBUG
	return mPhysicsManager;
}



void GameTicker::StartPhysicsTick()
{
	if (mTimeManager->GetAffordedPhysicsStepCount() <= 0)
	{
		log_adebug("Could not afford a physics step.");
		return;
	}

	bool lParallelPhysics;
	CURE_RTVAR_GET(lParallelPhysics, =, GetSettings(), RTVAR_PHYSICS_PARALLEL, true);
	if (lParallelPhysics)
	{
		CreatePhysicsThread();
	}
	else
	{
		DeletePhysicsThread();
	}

	//LEPRA_MEASURE_SCOPE(StartPhysics);
	// Physics thread
	// 1. does *NOT* add/delete objects,
	// 2. processes context objects ("scripts"),
	// 3. applies movement indata (from network, UI and script), and
	// 4. integrates physics.
	// System performance determines if this is done by a worker thread, or done synchronously
	// by the current thread. (Single CPU => single thread...)

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

void GameTicker::WaitPhysicsTick()
{
	if (mTimeManager->GetAffordedPhysicsStepCount() > 0)
	{
		if (mPhysicsTickDoneSemaphore)
		{
			LEPRA_MEASURE_SCOPE(WaitPhysics);
#ifdef LEPRA_DEBUG
			mPhysicsTickDoneSemaphore->Wait();
#else // !Debugging
			mPhysicsTickDoneSemaphore->Wait(3.0);
#endif // Debugging/!debugging.
		}

		// We gotta run this from the main thread, as we're working with the Context.
		DidPhysicsTick();
	}
}



void GameTicker::PhysicsTick()
{
	LEPRA_MEASURE_SCOPE(Physics);

	mPhysicsManager->InitCurrentThread();

	int lMicroSteps;
	CURE_RTVAR_GET(lMicroSteps, =, GetSettings(), RTVAR_PHYSICS_MICROSTEPS, 3);
	const int lAffordedStepCount = mTimeManager->GetAffordedPhysicsStepCount();
	const int lAffordedMicroStepCount = lAffordedStepCount * lMicroSteps;
	const float lStepTime = mTimeManager->GetAffordedPhysicsStepTime();
	const float lStepIncrement = lStepTime / lMicroSteps;
	/*if (lAffordedStepCount != 1)
	{
		mLog.Warningf(_T("Game time allows for %i physics steps in increments of %f."),
			lAffordedMicroStepCount, lStepIncrement);
	}*/
	{
		LEPRA_MEASURE_SCOPE(PreSteps);
		mPhysicsManager->PreSteps();
	}
	bool lFastAlgo;
	CURE_RTVAR_GET(lFastAlgo, =, GetSettings(), RTVAR_PHYSICS_FASTALGO, true);
	bool lNoClip;
	CURE_RTVAR_GET(lNoClip, =, GetSettings(), RTVAR_PHYSICS_NOCLIP, false);
	bool lStopPhysics;
	CURE_RTVAR_GET(lStopPhysics, =, GetSettings(), RTVAR_PHYSICS_HALT, false);
	try
	{
		for (int x = 0; !lStopPhysics && x < lAffordedMicroStepCount; ++x)
		{
			WillMicroTick(lStepIncrement);	// Ticks engines, so needs to be run every physics step.
			if (lFastAlgo)
			{
				mPhysicsManager->StepFast(lStepIncrement, !lNoClip);
			}
			else
			{
				mPhysicsManager->StepAccurate(lStepIncrement, !lNoClip);
			}
		}
	}
	catch (...)
	{
		mLog.Errorf(_T("Got some crash or major problem in physics simulation!"));
	}
	{
		//LEPRA_MEASURE_SCOPE(PostSteps);
		mPhysicsManager->PostSteps();
	}
}



void GameTicker::CreatePhysicsThread()
{
	// If we have more than one CPU, we run a separate physics thread.
	if (!mPhysicsWorkerThread && SystemManager::GetCoreCount() > 1)	// TODO: check performance to see if we should check for logical or physical CPUs.
	{
		mPhysicsTickStartSemaphore = new Semaphore();
		mPhysicsTickDoneSemaphore = new Semaphore();

		mPhysicsWorkerThread = new MemberThread<GameTicker>("PhysicsThread");
		mPhysicsWorkerThread->Start(this, &GameTicker::PhysicsThreadEntry);
	}
}

void GameTicker::DeletePhysicsThread()
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

void GameTicker::PhysicsThreadEntry()
{
	// We set affinity to the second processor. This is due to high resolution timers,
	// which may differ between different CPU cores. Several seconds can differ between
	// different cores. The main thread is locked to the first CPU, therefore this one
	// goes to the last one.
	// JB 2009-12: dropped this, probably not a good idea since we need to run multiple
	// physics instances when running split screen.
	// JB 2012-11: this may be a good idea again as the physics now is a singleton
	// again, shared by server+split screen clients.
	//Thread::GetCurrentThread()->SetCpuAffinityMask(1<<(SystemManager::GetCoreCount()-1));

	mPhysicsManager->InitCurrentThread();

	while (mPhysicsWorkerThread && !mPhysicsWorkerThread->GetStopRequest())
	{
		mPhysicsTickStartSemaphore->Wait();
		if (!mPhysicsWorkerThread->GetStopRequest())
		{
			PhysicsTick();
			mPhysicsTickDoneSemaphore->Signal();
		}
	}
}



LOG_CLASS_DEFINE(GAME, GameTicker);



}
