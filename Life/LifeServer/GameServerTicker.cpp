
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "GameServerTicker.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../ConsoleManager.h"
#include "../LifeApplication.h"
#include "RtVar.h"
#include "GameServerManager.h"
#include "ServerConsoleManager.h"



namespace Life
{



GameServerTicker::GameServerTicker(Cure::ResourceManager* pResourceManager,
	InteractiveConsoleLogListener* pConsoleLogger):
	mResourceManager(pResourceManager),
	mGameManager(0)
{
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_ALLLOGGEDOUT_AUTOSHUTDOWN, false);
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_GAME_SPAWNPART, 1.0);

	ConsoleManager lConsole(0, Cure::GetSettings(), 0, 0);
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
		ConsoleManager lConsole(0, Cure::GetSettings(), 0, 0);
		lConsole.InitCommands();
		lConsole.ExecuteCommand(_T("save-system-config-file 0 ") + Application::GetIoFile(_T("ServerBase"), _T("lsh")));
	}

	mResourceManager = 0;
}



bool GameServerTicker::Initialize()
{
	return (mGameManager->Initialize());
}

bool GameServerTicker::Tick()
{
	bool lOk = mGameManager->BeginTick();
	if (lOk)
	{
		lOk = mGameManager->EndTick();
	}

	mResourceManager->Tick();

	bool lAutoShutdown;
	CURE_RTVAR_GET(lAutoShutdown, =, Cure::GetSettings(), RTVAR_ALLLOGGEDOUT_AUTOSHUTDOWN, false);
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

float GameServerTicker::GetPowerSaveAmount() const
{
	return (mGameManager->GetPowerSaveAmount());
}



LOG_CLASS_DEFINE(GAME, GameServerTicker);



}
