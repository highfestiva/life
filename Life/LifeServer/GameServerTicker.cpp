
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameServerTicker.h"
#include "../../Cure/Include/ContextObjectAttribute.h"
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



GameServerTicker::GameServerTicker(Cure::ResourceManager* pResourceManager,
	InteractiveConsoleLogListener* pConsoleLogger):
	mResourceManager(pResourceManager),
	mGameManager(0),
	mMasterConnection(new MasterServerConnection)
{
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_APPLICATION_AUTOEXITONEMPTYSERVER, false);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_GAME_SPAWNPART, 1.0);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, true);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _("0.0.0.0:16650"));
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_NETWORK_SERVERNAME, _("My Dedicated Server"));
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_NETWORK_LOGINGREETING, _("echo 4 \"Welcome to my dedicated server! Enjoy the ride!\""));

	Cure::ContextObjectAttribute::SetCreator(&CreateObjectAttribute);

	ConsoleManager lConsole(mResourceManager, 0, Cure::GetSettings(), 0, 0);
	lConsole.InitCommands();
	lConsole.ExecuteCommand(_T("execute-file -i ServerDefault.lsh"));
	lConsole.ExecuteCommand(_T("execute-file -i ") + Application::GetIoFile(_T("ServerBase"), _T("lsh")));

	pResourceManager->InitDefault();

	Cure::RuntimeVariableScope* lVariableScope = new Cure::RuntimeVariableScope(Cure::GetSettings());
	mGameManager = new GameServerManager(lVariableScope, pResourceManager);
	mGameManager->StartConsole(pConsoleLogger, new StdioConsolePrompt);
}

GameServerTicker::~GameServerTicker()
{
	delete (mGameManager);
	mGameManager = 0;

	{
		ConsoleManager lConsole(mResourceManager, 0, Cure::GetSettings(), 0, 0);
		lConsole.InitCommands();
		lConsole.ExecuteCommand(_T("save-system-config-file 0 ") + Application::GetIoFile(_T("ServerBase"), _T("lsh")));
	}

	mResourceManager = 0;

	mMasterConnection->SetSocketInfo(0, -1);
	delete mMasterConnection;
	mMasterConnection = 0;
}



bool GameServerTicker::Initialize()
{
	return mGameManager->Initialize(mMasterConnection);
}

bool GameServerTicker::Tick()
{
	mMasterConnection->Tick();

	bool lOk = mGameManager->BeginTick();
	if (lOk)
	{
		lOk = mGameManager->EndTick();
	}

	mResourceManager->Tick();

	bool lAutoShutdown;
	CURE_RTVAR_GET(lAutoShutdown, =, Cure::GetSettings(), RTVAR_APPLICATION_AUTOEXITONEMPTYSERVER, false);
	if (lAutoShutdown)
	{
		static size_t lMaxLoginCount = 0;
		size_t lUserCount = mGameManager->ListUsers().size();
		lMaxLoginCount = (lUserCount > lMaxLoginCount)? lUserCount : lMaxLoginCount;
		if (lMaxLoginCount > 0 && lUserCount == 0)
		{
			mLog.AWarning("Server automatically shuts down since rtvar active and all users now logged off.");
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
	return mGameManager->GetTimeManager()->GetTickLoopTimeReduction();
}

float GameServerTicker::GetPowerSaveAmount() const
{
	return (mGameManager->GetPowerSaveAmount());
}



LOG_CLASS_DEFINE(GAME, GameServerTicker);



}
