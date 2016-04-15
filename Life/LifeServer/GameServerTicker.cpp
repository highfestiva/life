
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "GameServerTicker.h"
#include "../../Cure/Include/ContextObjectAttribute.h"
#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../ConsoleManager.h"
#include "../LifeApplication.h"
#include "../LifeString.h"
#include "GameServerManager.h"
#include "MasterServerConnection.h"
#include "RtVar.h"
#include "ServerConsoleManager.h"



namespace Life
{



GameServerTicker::GameServerTicker(Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity),
	mResourceManager(pResourceManager),
	mGameManager(0),
	mMasterConnection(0)
{
	v_set(Cure::GetSettings(), RTVAR_APPLICATION_AUTOEXITONEMPTYSERVER, false);
	v_set(Cure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, true);
	v_set(Cure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _("0.0.0.0:16650"));

	Cure::ContextObjectAttribute::SetCreator(&CreateObjectAttribute);

	pResourceManager->InitDefault();

	Cure::RuntimeVariableScope* lVariableScope = new Cure::RuntimeVariableScope(Cure::GetSettings());
	mGameManager = new GameServerManager(GetTimeManager(), lVariableScope, pResourceManager);
	mGameManager->SetTicker(this);
}

GameServerTicker::~GameServerTicker()
{
	delete (mGameManager);
	mGameManager = 0;

	{
		ConsoleManager lConsole(mResourceManager, 0, Cure::GetSettings(), 0, 0);
		lConsole.InitCommands();
		lConsole.ExecuteCommand("save-system-config-file 0 " + Application::GetIoFile("ServerBase", "lsh"));
	}

	mResourceManager = 0;

	mMasterConnection->SetSocketInfo(0, -1);
	delete mMasterConnection;
	mMasterConnection = 0;
}

void GameServerTicker::StartConsole(InteractiveConsoleLogListener* pConsoleLogger)
{
	ConsoleManager lConsole(mResourceManager, 0, Cure::GetSettings(), 0, 0);
	lConsole.InitCommands();
	lConsole.ExecuteCommand("execute-file -i ServerDefault.lsh");
	lConsole.ExecuteCommand("execute-file -i " + Application::GetIoFile("ServerBase", "lsh"));

	mGameManager->StartConsole(pConsoleLogger, new StdioConsolePrompt);
}

void GameServerTicker::SetMasterServerConnection(MasterServerConnection* pConnection)
{
	delete mMasterConnection;
	mMasterConnection = pConnection;
}



bool GameServerTicker::Initialize()
{
	str lServerAddress;
	v_get(lServerAddress, =, Cure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, "localhost:16650");
	return mGameManager->Initialize(mMasterConnection, lServerAddress);
}

bool GameServerTicker::Tick()
{
	mMasterConnection->Tick();

	GetTimeManager()->Tick();

	bool lOk = mGameManager->BeginTick();
	if (lOk)
	{
		StartPhysicsTick();
		mGameManager->PreEndTick();
		WaitPhysicsTick();
		lOk = mGameManager->EndTick();
	}

	mResourceManager->Tick();

	bool lAutoShutdown;
	v_get(lAutoShutdown, =, Cure::GetSettings(), RTVAR_APPLICATION_AUTOEXITONEMPTYSERVER, false);
	if (lAutoShutdown)
	{
		static size_t lMaxLoginCount = 0;
		size_t lUserCount = mGameManager->ListUsers().size();
		lMaxLoginCount = (lUserCount > lMaxLoginCount)? lUserCount : lMaxLoginCount;
		if (lMaxLoginCount > 0 && lUserCount == 0)
		{
			mLog.Warning("Server automatically shuts down since rtvar active and all users now logged off.");
			SystemManager::AddQuitRequest(+1);
		}
	}

	return (lOk);
}

void GameServerTicker::PollRoundTrip()
{
	mGameManager->TickInput();
}

float GameServerTicker::GetTickTimeReduction() const
{
	return GetTimeManager()->GetTickLoopTimeReduction();
}

float GameServerTicker::GetPowerSaveAmount() const
{
	return (mGameManager->GetPowerSaveAmount());
}



void GameServerTicker::WillMicroTick(float pTimeDelta)
{
	mGameManager->MicroTick(pTimeDelta);
}

void GameServerTicker::DidPhysicsTick()
{
	mGameManager->PostPhysicsTick();
}



void GameServerTicker::OnTrigger(Tbc::PhysicsManager::BodyID pTrigger, int pTriggerListenerId, int pOtherObjectId, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal)
{
	mGameManager->OnTrigger(pTrigger, pTriggerListenerId, pOtherObjectId, pBodyId, pPosition, pNormal);
}

void GameServerTicker::OnForceApplied(int pObjectId, int pOtherObjectId, Tbc::PhysicsManager::BodyID pBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		const vec3& pForce, const vec3& pTorque, const vec3& pPosition, const vec3& pRelativeVelocity)
{
	mGameManager->OnForceApplied(pObjectId, pOtherObjectId, pBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);
}



loginstance(GAME, GameServerTicker);



}
