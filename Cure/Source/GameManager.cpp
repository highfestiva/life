
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariableName.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../TBC/Include/PhysicsManagerFactory.h"
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

void GameTicker::Profile()
{
}



GameManager::GameManager(RuntimeVariableScope* pVariableScope, ResourceManager* pResourceManager, bool pForceSynchronous):
	mIsThreadSafe(true),
	mVariableScope(pVariableScope),
	mResource(pResourceManager),
	mNetwork(0),
	mTime(new TimeManager),
	mPhysics(TBC::PhysicsManagerFactory::Create(TBC::PhysicsManagerFactory::ENGINE_ODE)),
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
	LEPRA_MEASURE_SCOPE(BeginTick);

	if (CURE_RTVAR_GET(GetVariableScope(), RTVAR_PERFORMANCE_TEXT_ENABLE, false))
	{
		const double lReportInterval = CURE_RTVAR_GET(GetVariableScope(), RTVAR_PERFORMANCE_TEXT_INTERVAL, 10.0);
		TryReportPerformance(lReportInterval);
	}

	GetTickLock()->Acquire();

	{
		LEPRA_MEASURE_SCOPE(NetworkAndInput);

		mTime->TickTime();
		mTime->TickPhysics();

		// Sorts up incoming network data; adds/removes objects (for instance via remote create/delete).
		// On UI-based managers we handle user input here as well.
		TickInput();
	}

	{
		LEPRA_MEASURE_SCOPE(PhysicsPropagation);
		ScriptPhysicsTick();
	}

	if (mTime->GetAffordedPhysicsStepCount() > 0)
	{
		LEPRA_MEASURE_SCOPE(StartPhysics);

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
	LEPRA_MEASURE_SCOPE(EndTick);

	if (mTime->GetAffordedPhysicsStepCount() > 0)
	{
		LEPRA_MEASURE_SCOPE(WaitPhysics);
		GetTickLock()->Release();
		// Wait for physics thread to complete integration. If we run physics synchronously,
		// this does nothing.
		WaitPhysicsTick();
	}
	else
	{
		GetTickLock()->Release();
	}


	{
		LEPRA_MEASURE_SCOPE(NetworkSend);

		// Sends network packets. Among other things, movement of locally-controlled objects are sent.
		// This must be run after input processing, otherwise input-physics-output loop won't have
		// the desired effect.
		TickNetworkOutput();
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

TBC::PhysicsManager* GameManager::GetPhysicsManager() const
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



ContextObject* GameManager::CreateContextObject(const Lepra::String& pClassId, NetworkObjectType pNetworkType,
	GameObjectId pInstanceId)
{
	ContextObject* lObject = CreateContextObject(pClassId);
	lObject->SetManager(GetContext());
	lObject->SetNetworkObjectType(pNetworkType);
	if (pInstanceId)
	{
		lObject->SetInstanceId(pInstanceId);
	}
	else
	{
		lObject->SetInstanceId(GetContext()->AllocateGameObjectId(pNetworkType));
	}
	GetContext()->AddObject(lObject);
	return (lObject);
}



void GameManager::TryReportPerformance(double pReportInterval)
{
	mPerformanceReportTimer.UpdateTimer();
	const double lTimeDiff = mPerformanceReportTimer.GetTimeDiffF();
	if (lTimeDiff >= pReportInterval)
	{
		mPerformanceReportTimer.ClearTimeDiff();

		if (mNetwork->IsOpen())
		{
			mSendBandwidth.Append(lTimeDiff, 0, mNetwork->GetTotalSentByteCount());
			mReceiveBandwidth.Append(lTimeDiff, 0, mNetwork->GetTotalReceivedByteCount());
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

		const Lepra::ScopePerformanceData::NodeArray lRoots = Lepra::ScopePerformanceData::GetRoots();
		ReportPerformance(Lepra::ScopePerformanceData::GetRoots(), 0);
	}
}

void GameManager::ClearPerformanceData()
{
	mSendBandwidth.Clear();
	mReceiveBandwidth.Clear();

	Lepra::ScopePerformanceData::ClearAll(Lepra::ScopePerformanceData::GetRoots());
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



void GameManager::ReportPerformance(const Lepra::ScopePerformanceData::NodeArray& pNodes, int pRecursion)
{
	const Lepra::String lIndent = Lepra::String(pRecursion*3, ' ');
	Lepra::ScopePerformanceData::NodeArray::const_iterator x = pNodes.begin();
	for (; x != pNodes.end(); ++x)
	{
		const Lepra::ScopePerformanceData* lNode = *x;
		Lepra::String lName = Lepra::StringUtility::Split(lNode->GetName(), _T(";"))[0];
		mLog.Performancef((lIndent+lName+_T(" Min: %ss, last: %ss, savg: %ss, max: %ss.")).c_str(), 
			Lepra::Number::ConvertToPostfixNumber(lNode->GetMinimum(), 2).c_str(),
			Lepra::Number::ConvertToPostfixNumber(lNode->GetLast(), 2).c_str(),
			Lepra::Number::ConvertToPostfixNumber(lNode->GetSlidingAverage(), 2).c_str(),
			Lepra::Number::ConvertToPostfixNumber(lNode->GetMaximum(), 2).c_str());
		ReportPerformance(lNode->GetChildren(), pRecursion+1);
	}
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
	LEPRA_MEASURE_SCOPE(Physics);

	const int lMicroSteps = CURE_RTVAR_GET(GetVariableScope(), RTVAR_PHYSICS_MICROSTEPS, 3);
	const int lAffordedStepCount = mTime->GetAffordedPhysicsStepCount() * lMicroSteps;
	const float lStepIncrement = mTime->GetAffordedPhysicsStepTime() / lMicroSteps;
	/*if (lAffordedStepCount != 1 && !Lepra::Math::IsEpsEqual(lStepIncrement, 1/60.0f))
	{
		mLog.Warningf(_T("Game time allows for %i physics steps in increments of %f."),
			lAffordedStepCount, lStepIncrement);
	}*/
	for (int x = 0; x < lAffordedStepCount; ++x)
	{
		ScriptTick(lStepIncrement);
		mPhysics->StepFast(lStepIncrement);
	}

	mContext->HandleIdledBodies();
	mContext->HandlePhysicsSend();
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

void GameManager::ScriptTick(float pTimeDelta)
{
	mContext->Tick(pTimeDelta);
}

void GameManager::ScriptPhysicsTick()
{
	//if (mTime->GetAffordedPhysicsStepCount() > 0)
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
	// JB 2009-12: dropped this, probably not a good idea since we need to run multiple
	// physics instances when running split screen.
	//Lepra::Thread::GetCurrentThread()->SetCpuAffinityMask(1<<(Lepra::SystemManager::GetLogicalCpuCount()-1));

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
