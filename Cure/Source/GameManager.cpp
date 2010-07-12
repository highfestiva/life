
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

bool GameTicker::QueryQuit()
{
	return (SystemManager::GetQuitRequest() != 0);
}



GameManager::GameManager(RuntimeVariableScope* pVariableScope, ResourceManager* pResourceManager):
	mIsThreadSafe(true),
	mVariableScope(pVariableScope),
	mResource(pResourceManager),
	mNetwork(0),
	mTime(new TimeManager),
	mPhysics(TBC::PhysicsManagerFactory::Create(TBC::PhysicsManagerFactory::ENGINE_ODE)),
	mContext(0),
	mTerrain(0),//new TerrainManager(pResourceManager)),
	mConsole(0),
	mPhysicsWorkerThread(0),
	mPhysicsTickStartSemaphore(0),
	mPhysicsTickDoneSemaphore(0)
{
	mContext = new ContextManager(this);
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

	bool lPerformanceText;
	CURE_RTVAR_GET(lPerformanceText, =, GetVariableScope(), RTVAR_PERFORMANCE_TEXT_ENABLE, false);
	if (lPerformanceText)
	{
		double lReportInterval;
		CURE_RTVAR_GET(lReportInterval, =, GetVariableScope(), RTVAR_PERFORMANCE_TEXT_INTERVAL, 10.0);
		TryReportPerformance(lReportInterval);
	}

	bool lParallelPhysics;
	CURE_RTVAR_GET(lParallelPhysics, =, GetVariableScope(), RTVAR_PHYSICS_PARALLEL, true);
	if (lParallelPhysics)
	{
		CreatePhysicsThread();
	}
	else
	{
		DeletePhysicsThread();
	}

	{
		LEPRA_MEASURE_SCOPE(AcquireTickLock);
		GetTickLock()->Acquire();
	}

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

bool GameManager::TickNetworkOutput()
{
	return (mNetwork->SendAll());
}

Lock* GameManager::GetTickLock() const
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
		size_t lThisThreadId = Thread::GetCurrentThreadId();
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

TimeManager* GameManager::GetTimeManager()
{
	return (mTime);
}



ContextObject* GameManager::CreateContextObject(const str& pClassId, NetworkObjectType pNetworkType,
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

ContextObject* GameManager::CreateLogicHandler(const str&) const
{
	assert(false);
	return 0;
}

bool GameManager::IsUiMoveForbidden(GameObjectId) const
{
	return (false);	// Non-UI implementors need not bother.
}

void GameManager::GetSiblings(GameObjectId pInstanceId, ContextObject::Array& pSiblingArray) const
{
	// Only self if no override/no more than one game manager instance.
	pSiblingArray.push_back(GetContext()->GetObject(pInstanceId));
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
			mSendBandwidth.Append(lTimeDiff, 0, mNetwork->GetSentByteCount());
			mReceiveBandwidth.Append(lTimeDiff, 0, mNetwork->GetReceivedByteCount());
			mLog.Performancef(_T("Network bandwith. Up: %sB/s (peak %sB/s). Down: %sB/s (peak %sB/s)."), 
				Number::ConvertToPostfixNumber(mSendBandwidth.GetLast(), 2).c_str(),
				Number::ConvertToPostfixNumber(mSendBandwidth.GetMaximum(), 2).c_str(),
				Number::ConvertToPostfixNumber(mReceiveBandwidth.GetLast(), 2).c_str(),
				Number::ConvertToPostfixNumber(mReceiveBandwidth.GetMaximum(), 2).c_str());
		}
		else
		{
			mSendBandwidth.Clear();
			mReceiveBandwidth.Clear();
		}

		const ScopePerformanceData::NodeArray lRoots = ScopePerformanceData::GetRoots();
		ReportPerformance(ScopePerformanceData::GetRoots(), 0);
	}
}

void GameManager::ClearPerformanceData()
{
	ScopeLock lLock(&mLock);

	mSendBandwidth.Clear();
	mReceiveBandwidth.Clear();

	ScopePerformanceData::ClearAll();
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




void GameManager::ReportPerformance(const ScopePerformanceData::NodeArray& pNodes, int pRecursion)
{
	const str lIndent = str(pRecursion*3, ' ');
	ScopePerformanceData::NodeArray::const_iterator x = pNodes.begin();
	for (; x != pNodes.end(); ++x)
	{
		const ScopePerformanceData* lNode = *x;
		str lName = strutil::Split(lNode->GetName(), _T(";"))[0];
		mLog.Performancef((lIndent+lName+_T(" Min: %ss, last: %ss, savg: %ss, max: %ss.")).c_str(), 
			Number::ConvertToPostfixNumber(lNode->GetMinimum(), 2).c_str(),
			Number::ConvertToPostfixNumber(lNode->GetLast(), 2).c_str(),
			Number::ConvertToPostfixNumber(lNode->GetSlidingAverage(), 2).c_str(),
			Number::ConvertToPostfixNumber(lNode->GetMaximum(), 2).c_str());
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

	mPhysics->InitCurrentThread();

	int lMicroSteps;
	CURE_RTVAR_GET(lMicroSteps, =, GetVariableScope(), RTVAR_PHYSICS_MICROSTEPS, 3);
	const int lAffordedStepCount = mTime->GetAffordedPhysicsStepCount() * lMicroSteps;
	const float lStepIncrement = mTime->GetAffordedPhysicsStepTime() / lMicroSteps;
	/*if (lAffordedStepCount != 1 && !Math::IsEpsEqual(lStepIncrement, 1/60.0f))
	{
		mLog.Warningf(_T("Game time allows for %i physics steps in increments of %f."),
			lAffordedStepCount, lStepIncrement);
	}*/
	{
		LEPRA_MEASURE_SCOPE(PreSteps);
		mPhysics->PreSteps();
	}
	for (int x = 0; x < lAffordedStepCount; ++x)
	{
		ScriptTick(lStepIncrement);
		mPhysics->StepFast(lStepIncrement);
	}
	{
		LEPRA_MEASURE_SCOPE(PostSteps);
		mPhysics->PostSteps();
	}

	{
		LEPRA_MEASURE_SCOPE(Handles);
		HandleWorldBoundaries();
		mContext->HandleIdledBodies();
		mContext->HandlePhysicsSend();
	}
}

bool GameManager::IsHighImpact(float pScaleFactor, const ContextObject* pObject, const Vector3DF& pForce,
	const Vector3DF& pTorque) const
{
	const float lMassFactor = 1/pObject->GetMass();
	Vector3DF lGravityDirection = GetPhysicsManager()->GetGravity();
	lGravityDirection.Normalize();
	// High angle against direction of gravity means high impact.
	const float lForceWithoutGravityFactor = (pForce * lGravityDirection) - pForce.Cross(lGravityDirection).GetLength();
	const float lNormalizedForceFactor = lForceWithoutGravityFactor * lMassFactor;
	const float lNormalizedTorqueFactor = pTorque.GetLength() * lMassFactor;
	bool lIsHighImpact = (lNormalizedForceFactor < -16*pScaleFactor || lNormalizedForceFactor >= 2*pScaleFactor || lNormalizedTorqueFactor > 3*pScaleFactor);
	if (lIsHighImpact)
	{
		log_volatile(mLog.Tracef(_T("Collided hard with something dynamic. F=%f, T=%f"),
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

void GameManager::HandleWorldBoundaries()
{
}



void GameManager::PhysicsThreadEntry()
{
	// We set affinity to the second processor. This is due to high resolution timers,
	// which may differ between different CPU cores. Several seconds can differ between
	// different cores. The main thread is locked to the first CPU, therefore this one
	// goes to the last one.
	// JB 2009-12: dropped this, probably not a good idea since we need to run multiple
	// physics instances when running split screen.
	//Thread::GetCurrentThread()->SetCpuAffinityMask(1<<(SystemManager::GetLogicalCpuCount()-1));

	mPhysics->InitCurrentThread();

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
	if (!mPhysicsWorkerThread && SystemManager::GetLogicalCpuCount() > 1)	// TODO: check performance to see if we should check for logical or physical CPUs.
	{
		mPhysicsTickStartSemaphore = new Semaphore();
		mPhysicsTickDoneSemaphore = new Semaphore();

		mPhysicsWorkerThread = new MemberThread<GameManager>(_T("PhysicsThread"));
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
