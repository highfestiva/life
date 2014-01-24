
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/GameManager.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../TBC/Include/PhysicsManagerFactory.h"
#include "../../TBC/Include/PhysicsSpawner.h"
#include "../Include/ConsoleManager.h"
#include "../Include/ContextManager.h"
#include "../Include/ContextObject.h"
#include "../Include/GameTicker.h"
#include "../Include/NetworkAgent.h"
#include "../Include/RuntimeVariable.h"
#include "../Include/RuntimeVariableName.h"
#include "../Include/Spawner.h"
#include "../Include/TerrainManager.h"
#include "../Include/TimeManager.h"



namespace Cure
{



GameManager::GameManager(const TimeManager* pTime, RuntimeVariableScope* pVariableScope, ResourceManager* pResourceManager):
	mIsThreadSafe(true),
	mVariableScope(pVariableScope),
	mResource(pResourceManager),
	mNetwork(0),
	mTicker(0),
	mTime(pTime),
	mContext(0),
	mTerrain(0),//new TerrainManager(pResourceManager)),
	mConsole(0)
{
	mContext = new ContextManager(this);
}

GameManager::~GameManager()
{
	delete (mConsole);
	mConsole = 0;
	delete (mContext);
	mContext = 0;

	// Free after killing all game objects.
	mResource->ForceFreeCache();
	mResource->ForceFreeCache();

	mTime = 0;	// Not owned resource.
	mTicker = 0;	// Not owned resource.
	delete (mTerrain);
	mTerrain = 0;
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

const GameTicker* GameManager::GetTicker() const
{
	return mTicker;
}

void GameManager::SetTicker(const GameTicker* pTicker)
{
	mTicker = pTicker;
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

	{
		//LEPRA_MEASURE_SCOPE(AcquireTickLock);
		GetTickLock()->Acquire();	// Lock for physics propagation, user input, etc.
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

	mIsThreadSafe = false;

	return true;
}

void GameManager::PreEndTick()
{
	mIsThreadSafe = true;
	GetTickLock()->Release();
}

bool GameManager::EndTick()
{
	{
		//LEPRA_MEASURE_SCOPE(NetworkSend);

		// Sends network packets. Among other things, movement of locally-controlled objects are sent.
		// This must be run after input processing, otherwise input-physics-output loop won't have
		// the desired effect.
		TickNetworkOutput();
	}

	GetConsoleManager()->ExecuteYieldCommand();

	return true;
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
	return mTicker->GetPhysicsManager(mIsThreadSafe);
}

ConsoleManager* GameManager::GetConsoleManager() const
{
	return (mConsole);
}

void GameManager::SetConsoleManager(ConsoleManager* pConsole)
{
	mConsole = pConsole;
}



void GameManager::MicroTick(float pTimeDelta)
{
	ScopeLock lLock(GetTickLock());
	mContext->MicroTick(pTimeDelta);
}

void GameManager::PostPhysicsTick()
{
	mContext->HandleIdledBodies();
	mContext->HandlePhysicsSend();
	HandleWorldBoundaries();
}



bool GameManager::IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const
{
	(void)pPosition;
	(void)pDistance;
	return true;
}

ContextObject* GameManager::CreateContextObject(const str& pClassId, NetworkObjectType pNetworkType, GameObjectId pInstanceId)
{
	ContextObject* lObject = CreateContextObject(pClassId);
	AddContextObject(lObject, pNetworkType, pInstanceId);
	return (lObject);
}

void GameManager::DeleteContextObject(GameObjectId pInstanceId)
{
	mContext->DeleteObject(pInstanceId);
}

void GameManager::DeleteContextObjectDelay(ContextObject* pObject, float pDelay)
{
	mContext->AddAlarmCallback(pObject, ContextManager::SYSTEM_ALARM_ID_KILL, pDelay, 0);
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

Spawner* GameManager::GetAvatarSpawner(GameObjectId pLevelId) const
{
	ContextObject* lLevel = GetContext()->GetObject(pLevelId);
	if (!lLevel)
	{
		return 0;
	}
	const ContextObject::Array& lChildArray = lLevel->GetChildArray();
	ContextObject::Array::const_iterator x = lChildArray.begin();
	for (; x != lChildArray.end(); ++x)
	{
		if ((*x)->GetClassId() != _T("Spawner"))
		{
			continue;
		}
		Spawner* lSpawner = (Spawner*)*x;
		const TBC::PhysicsSpawner* lSpawnShape = lSpawner->GetSpawner();
		if (lSpawnShape->GetNumber() == 0)
		{
			return lSpawner;
		}
	}
	return 0;
}

bool GameManager::IsUiMoveForbidden(GameObjectId) const
{
	return false;	// Non-UI implementors need not bother.
}

void GameManager::OnStopped(ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId)
{
#ifdef LEPRA_DEBUG
	const unsigned lRootIndex = 0;
	deb_assert(pObject->GetStructureGeometry(lRootIndex));
	deb_assert(pObject->GetStructureGeometry(lRootIndex)->GetBodyId() == pBodyId);
#endif // Debug / !Debug
	(void)pBodyId;

	if (pObject->GetNetworkObjectType() == NETWORK_OBJECT_LOCALLY_CONTROLLED)
	{
		log_volatile(mLog.Debugf(_T("Object %u/%s stopped, sending position."), pObject->GetInstanceId(), pObject->GetClassId().c_str()));
		GetContext()->AddPhysicsSenderObject(pObject);
	}
}

void GameManager::OnAlarm(int pAlarmId, ContextObject* pObject, void* pExtraData)
{
	(void)pExtraData;
	if (pAlarmId == ContextManager::SYSTEM_ALARM_ID_KILL)
	{
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}



bool GameManager::ValidateVariable(int pSecurityLevel, const str& pVariable, str& pValue) const
{
	if (pSecurityLevel < 1 && (pVariable == _T(RTVAR_PHYSICS_FPS) ||
		pVariable == _T(RTVAR_PHYSICS_RTR) ||
		pVariable == _T(RTVAR_PHYSICS_HALT)))
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
	else if (pVariable == _T(RTVAR_PHYSICS_HALT))
	{
		bool lValue = false;
		if (!strutil::StringToBool(pValue, lValue)) return false;
		pValue = strutil::BoolToString(lValue);
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



void GameManager::OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherObjectId, TBC::PhysicsManager::BodyID pBodyId, const Vector3DF& pNormal)
{
	ContextObject* lObject1 = GetContext()->GetObject(pTriggerListenerId);
	if (lObject1)
	{
		ContextObject* lObject2 = GetContext()->GetObject(pOtherObjectId);
		if (lObject2)
		{
			lObject1->OnTrigger(pTrigger, lObject2, pBodyId, pNormal);
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



NetworkAgent* GameManager::GetNetworkAgent() const
{
	return (mNetwork);
}

void GameManager::SetNetworkAgent(NetworkAgent* pNetwork)
{
	delete (mNetwork);
	mNetwork = pNetwork;
}




void GameManager::ScriptPhysicsTick()
{
	//if (mTime->GetAffordedPhysicsStepCount() > 0)
	{
		mContext->HandlePostKill();
		mContext->TickPhysics();
	}
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



bool GameManager::IsThreadSafe() const
{
	return (mIsThreadSafe);
}

void GameManager::HandleWorldBoundaries()
{
}



LOG_CLASS_DEFINE(GAME, GameManager);



}
