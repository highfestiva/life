
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



GameTicker::GameTicker():
	mTimeManager(new TimeManager)
{
}

GameTicker::~GameTicker()
{
}

const TimeManager* GameTicker::GetTimeManager() const
{
	return mTimeManager;
}

TimeManager* GameTicker::GetTimeManager()
{
	return mTimeManager;
}

void GameTicker::Profile()
{
}

bool GameTicker::QueryQuit()
{
	return (SystemManager::GetQuitRequest() != 0);
}



GameManager::GameManager(const TimeManager* pTime, RuntimeVariableScope* pVariableScope, ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	mIsThreadSafe(true),
	mVariableScope(pVariableScope),
	mResource(pResourceManager),
	mNetwork(0),
	mTime(pTime),
	mPhysics(TBC::PhysicsManagerFactory::Create(TBC::PhysicsManagerFactory::ENGINE_ODE, pPhysicsRadius, pPhysicsLevels,  pPhysicsSensitivity)),
	mContext(0),
	mTerrain(0),//new TerrainManager(pResourceManager)),
	mConsole(0),
	mPhysicsWorkerThread(0),
	mPhysicsTickStartSemaphore(0),
	mPhysicsTickDoneSemaphore(0)
{
	mContext = new ContextManager(this);
	mPhysics->SetTriggerListener(this);
	mPhysics->SetForceFeedbackListener(this);
	mPhysics->InitCurrentThread();
}

GameManager::~GameManager()
{
	DeletePhysicsThread();

	delete (mConsole);
	mConsole = 0;
	delete (mContext);
	mContext = 0;
	mTime = 0;	// Not owned resource.
	delete (mTerrain);
	mTerrain = 0;
	delete (mPhysics);
	mPhysics = 0;
	delete (mNetwork);
	mNetwork = 0;
	mResource = 0;
	delete (mVariableScope);
	mVariableScope = 0;

	while (mLock.IsOwner())
	{
		mLock.Release();
	}
}



bool GameManager::IsPrimaryManager() const
{
	return true;
}

bool GameManager::BeginTick()
{
	LEPRA_MEASURE_SCOPE(BeginTick);

	bool lPerformanceText;
	CURE_RTVAR_GET(lPerformanceText, =, GetVariableScope(), RTVAR_PERFORMANCE_TEXT_ENABLE, false);
	double lReportInterval;
	CURE_RTVAR_GET(lReportInterval, =, GetVariableScope(), RTVAR_PERFORMANCE_TEXT_INTERVAL, 1.0);
	UpdateReportPerformance(lPerformanceText, lReportInterval);

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
		//LEPRA_MEASURE_SCOPE(AcquireTickLock);
		GetTickLock()->Acquire();
	}

	{
		//LEPRA_MEASURE_SCOPE(NetworkAndInput);

		//mTime->Tick();

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
		//LEPRA_MEASURE_SCOPE(StartPhysics);
		// Physics thread
		// 1. does *NOT* add/delete objects,
		// 2. processes context objects ("scripts"),
		// 3. applies movement indata (from network, UI and script), and
		// 4. integrates physics.
		// Performance determines if this is done by a worker thread, or done synchronously
		// by the current thread. (Single CPU => single thread...)
		StartPhysicsTick();
	}
	else
	{
		log_adebug("Could not afford a physics step.");
	}

	return (true);
}

bool GameManager::EndTick()
{
	//LEPRA_MEASURE_SCOPE(EndTick);

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
		//LEPRA_MEASURE_SCOPE(NetworkSend);

		// Sends network packets. Among other things, movement of locally-controlled objects are sent.
		// This must be run after input processing, otherwise input-physics-output loop won't have
		// the desired effect.
		TickNetworkOutput();
	}

	return (true);
}

bool GameManager::TickNetworkOutput()
{
	mContext->HandleAttributeSend();
	if (mNetwork)
	{
		return (mNetwork->SendAll());
	}
	return true;
}

Lock* GameManager::GetTickLock() const
{
	return (&mLock);
}



RuntimeVariableScope* GameManager::GetVariableScope() const
{
	return (mVariableScope);
}

void GameManager::SetVariableScope(RuntimeVariableScope* pScope)
{
	mVariableScope = pScope;
}

ResourceManager* GameManager::GetResourceManager() const
{
	return (mResource);
}

ContextManager* GameManager::GetContext() const
{
	return (mContext);
}

const TimeManager* GameManager::GetTimeManager() const
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



bool GameManager::IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const
{
	pPosition;
	pDistance;
	return true;
}

ContextObject* GameManager::CreateContextObject(const str& pClassId, NetworkObjectType pNetworkType, GameObjectId pInstanceId)
{
	ContextObject* lObject = CreateContextObject(pClassId);
	AddContextObject(lObject, pNetworkType, pInstanceId);
	return (lObject);
}

void GameManager::AddContextObject(ContextObject* pObject, NetworkObjectType pNetworkType, GameObjectId pInstanceId)
{
	pObject->SetNetworkObjectType(pNetworkType);
	if (pInstanceId)
	{
		pObject->SetInstanceId(pInstanceId);
	}
	else
	{
		pObject->SetInstanceId(GetContext()->AllocateGameObjectId(pNetworkType));
	}
	pObject->SetManager(GetContext());
	GetContext()->AddObject(pObject);
}

ContextObject* GameManager::CreateLogicHandler(const str&)
{
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

void GameManager::OnStopped(ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId)
{
#ifdef LEPRA_DEBUG
	const unsigned lRootIndex = 0;
	assert(pObject->GetStructureGeometry(lRootIndex));
	assert(pObject->GetStructureGeometry(lRootIndex)->GetBodyId() == pBodyId);
#endif // Debug / !Debug
	(void)pBodyId;

	if (pObject->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED)
	{
		log_volatile(mLog.Debugf(_T("Object %u/%s stopped, sending position."), pObject->GetInstanceId(), pObject->GetClassId().c_str()));
		GetContext()->AddPhysicsSenderObject(pObject);
	}
}

void GameManager::OnAlarm(int, ContextObject*, void*)
{
}



bool GameManager::ValidateVariable(int pSecurityLevel, const str& pVariable, str& pValue) const
{
	if (pSecurityLevel < 1 && (pVariable == _T(RTVAR_PHYSICS_FPS) ||
		pVariable == _T(RTVAR_PHYSICS_RTR)))
	{
		mLog.Warning(_T("You're not authorized to change this variable."));
		return false;
	}
	if (pVariable == _T(RTVAR_PHYSICS_FPS) || pVariable == _T(RTVAR_PHYSICS_MICROSTEPS))
	{
		int lValue = 0;
		if (!strutil::StringToInt(pValue, lValue)) return false;
		lValue = (pVariable == _T(RTVAR_PHYSICS_FPS))? Math::Clamp(lValue, 5, 10000) : Math::Clamp(lValue, 1, 10);
		pValue = strutil::IntToString(lValue, 10);
	}
	else if (pVariable == _T(RTVAR_PHYSICS_RTR))
	{
		double lValue = 0;
		if (!strutil::StringToDouble(pValue, lValue)) return false;
		lValue = Math::Clamp(lValue, 0.01, 4.0);
		strutil::DoubleToString(lValue, 4, pValue);
	}
	return true;
}



void GameManager::UpdateReportPerformance(bool pReport, double pReportInterval)
{
	mPerformanceReportTimer.UpdateTimer();
	const double lTimeDiff = mPerformanceReportTimer.GetTimeDiff();
	if (lTimeDiff >= pReportInterval)
	{
		mPerformanceReportTimer.ClearTimeDiff();

		if (mNetwork && mNetwork->IsOpen())
		{
			mSendBandwidth.Append(lTimeDiff, 0, mNetwork->GetSentByteCount());
			mReceiveBandwidth.Append(lTimeDiff, 0, mNetwork->GetReceivedByteCount());
			if (pReport)
			{
				mLog.Performancef(_T("Network bandwith. Up: %sB/s (peak %sB/s). Down: %sB/s (peak %sB/s)."), 
					Number::ConvertToPostfixNumber(mSendBandwidth.GetLast(), 2).c_str(),
					Number::ConvertToPostfixNumber(mSendBandwidth.GetMaximum(), 2).c_str(),
					Number::ConvertToPostfixNumber(mReceiveBandwidth.GetLast(), 2).c_str(),
					Number::ConvertToPostfixNumber(mReceiveBandwidth.GetMaximum(), 2).c_str());
			}
		}
		else
		{
			mSendBandwidth.Clear();
			mReceiveBandwidth.Clear();
		}

		if (pReport)
		{
			const ScopePerformanceData::NodeArray lRoots = ScopePerformanceData::GetRoots();
			ReportPerformance(ScopePerformanceData::GetRoots(), 0);
		}
	}
}

void GameManager::ClearPerformanceData()
{
	ScopeLock lLock(&mLock);

	mSendBandwidth.Clear();
	mReceiveBandwidth.Clear();

	ScopePerformanceData::ClearAll();
}

void GameManager::GetBandwidthData(BandwidthData& pSent, BandwidthData& pReceived)
{
	pSent = mSendBandwidth;
	pReceived = mReceiveBandwidth;
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
	const int lAffordedStepCount = mTime->GetAffordedPhysicsStepCount();
	const int lAffordedMicroStepCount = lAffordedStepCount * lMicroSteps;
	const float lStepTime = mTime->GetAffordedPhysicsStepTime();
	const float lStepIncrement = lStepTime / lMicroSteps;
	/*if (lAffordedStepCount != 1)
	{
		mLog.Warningf(_T("Game time allows for %i physics steps in increments of %f."),
			lAffordedMicroStepCount, lStepIncrement);
	}*/
	{
		LEPRA_MEASURE_SCOPE(PreSteps);
		mPhysics->PreSteps();
	}
	bool lFastAlgo;
	CURE_RTVAR_GET(lFastAlgo, =, GetVariableScope(), RTVAR_PHYSICS_FASTALGO, true);
	try
	{
		for (int x = 0; x < lAffordedMicroStepCount; ++x)
		{
			ScriptTick(lStepIncrement);	// Ticks engines, so needs to be run every physics step.
			if (lFastAlgo)
			{
				mPhysics->StepFast(lStepIncrement);
			}
			else
			{
				mPhysics->StepAccurate(lStepIncrement);
			}
		}
	}
	catch (...)
	{
		mLog.Errorf(_T("Got some crash or major problem in physics simulation!"));
	}
	{
		//LEPRA_MEASURE_SCOPE(PostSteps);
		mPhysics->PostSteps();
	}

	{
		//LEPRA_MEASURE_SCOPE(Handles);
		mContext->HandleIdledBodies();
		mContext->HandlePhysicsSend();
		HandleWorldBoundaries();
	}
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
		mContext->HandlePostKill();
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

		mPhysicsWorkerThread = new MemberThread<GameManager>("PhysicsThread");
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

void GameManager::OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherBodyId)
{
	ContextObject* lObject1 = GetContext()->GetObject(pTriggerListenerId);
	if (lObject1)
	{
		ContextObject* lObject2 = GetContext()->GetObject(pOtherBodyId);
		if (lObject2)
		{
			lObject1->OnTrigger(pTrigger, lObject2);
		}
	}
}

void GameManager::OnForceApplied(int pObjectId, int pOtherObjectId, TBC::PhysicsManager::BodyID pBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	ContextObject* lObject1 = GetContext()->GetObject(pObjectId);
	if (lObject1)
	{
		ContextObject* lObject2 = GetContext()->GetObject(pOtherObjectId);
		if (lObject2)
		{
			lObject1->OnForceApplied(lObject2, pBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);
		}
	}
}


LOG_CLASS_DEFINE(GAME, GameManager);



}
